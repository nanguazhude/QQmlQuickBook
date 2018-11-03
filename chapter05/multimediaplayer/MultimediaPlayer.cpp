#include "MultimediaPlayer.hpp"
#include <QtGui/qimage.h>
#include <QtCore/qcoreapplication.h>
/**********************************/
#include <QtMultimedia/qaudiooutput.h> 
/**********************************/
/*muti thread*/
#include <thread>
#include <mutex>
#include <shared_mutex>
/**********************************/
/*include ffmpeg*/
extern "C" {
#include <libavutil/avstring.h>
#include <libavutil/eval.h>
#include <libavutil/mathematics.h>
#include <libavutil/pixdesc.h>
#include <libavutil/imgutils.h>
#include <libavutil/dict.h>
#include <libavutil/parseutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avassert.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavcodec/avfft.h>
#include <libswresample/swresample.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
}
/**********************************/
namespace this_cpp_file {
    
#ifndef ffmpeg
#define ffmpeg
#endif
      
    class AudioPlayer : public QAudioOutput {
    public:
        AudioPlayer() {
            this->setNotifyInterval(1);
        }
    };

    class VideoStreamCodec {
    public:
        AVCodec * codec;
    };

    class AudioStreamCodec {
    public:
        AVCodec * codec;
        int sample_rate;
    };

    class FrameImage {
    public:
        int width {0}/*宽*/;
        int height{0}/*高*/;
        double pts{0}/*播放时间戳*/;

    };

    class FrameAudio {
    public:
        double pts{ 0 }/*播放时间戳*/;
    };

    /*初始化ffmpeg时使用的环境*/
    inline std::once_flag & getInitFFMPEGOnceFlag() {
        static std::once_flag varAns;
        return varAns;
    }

    /*初始化ffmpeg环境*/
    inline void initFFMPEG() {
        std::call_once(getInitFFMPEGOnceFlag(), []() {
            ffmpeg::av_register_all();
        });
    }

    class FFMPEGDecoder :
        public QObject,
        public virtual sstd::memory_lock::VirtualClassBasic {
    public:

        std::thread video_thread;
        std::thread audio_thread;
        std::thread read_next_thread;
        std::atomic_bool video_thread_quit{ false };
        std::atomic_bool audio_thread_quit{ false };
        std::atomic_bool read_next_thread_quit{ false };
        std::condition_variable video_thread_wait;
        std::condition_variable audio_thread_wait;
        std::condition_variable read_next_thread_wait;

        sstd::vector<VideoStreamCodec *> videoStreamCodec;
        sstd::vector<AudioStreamCodec *> audioStreamCodec;

        inline FFMPEGDecoder() {
            initFFMPEG();
        }

        inline void stop() {
            video_thread_quit.store(true);
            audio_thread_quit.store(true);
            read_next_thread_quit.store(true);
            video_thread_wait.notify_all();
            audio_thread_wait.notify_all();
            read_next_thread_wait.notify_all();
            /****************************************/
            if (audio_player) {
                audio_player->stop();
            }
            /****************************************/
            if (video_thread.joinable()) {
                video_thread.join();
            }
            /****************************************/
            if (audio_thread.joinable()) {
                audio_thread.join();
            }
            /****************************************/
            if (read_next_thread.joinable()) {
                read_next_thread.join();
            }
            /****************************************/
            if (av_contex) {
                avformat_close_input(&av_contex);
                av_contex = nullptr;
            }
        }
        using av_error_tmp_buffer_type = std::array<char, 1024 * 1024>;
        inline static av_error_tmp_buffer_type & getStringTmpBuffer() {
            static av_error_tmp_buffer_type varAns;
            return varAns;
        }
        QByteArray av_url_data;
        AVFormatContext *av_contex{ nullptr };
        const char *av_url{ nullptr };
        AVDictionary * av_information{ nullptr };
        QString * mmm_ErrorString;
        /*AVInputFormat *av_input_fmt{nullptr};*/
        /*AVDictionary * av_options{nullptr};*/
        double av_length{ 0 };
        /**/
        inline bool construct_local_decode(const QString & arg) {
            assert(av_contex == nullptr);
            av_url_data = arg.toLocal8Bit();
            av_url = av_url_data.constData();
            auto varError = -1;
            /*try to open the stream ...*/
            varError = ffmpeg::avformat_open_input(&av_contex, av_url, nullptr, nullptr);
            if ((varError != 0) || (av_contex == nullptr)) {
                auto & varTmp = getStringTmpBuffer();
                ffmpeg::av_strerror(varError, varTmp.data(), varTmp.size());
                *mmm_ErrorString = QString::fromLocal8Bit(varTmp.data());
                return false;
            }
            /*try to read the stream information ...*/
            varError = ffmpeg::avformat_find_stream_info(av_contex, &av_information);
            if ((varError != 0) || (av_information == nullptr)) {
                auto & varTmp = getStringTmpBuffer();
                ffmpeg::av_strerror(varError, varTmp.data(), varTmp.size());
                *mmm_ErrorString = QString::fromLocal8Bit(varTmp.data());
                return false;
            }
            constructAVLength();
            return true;
        }

        inline ~FFMPEGDecoder() {
            stop();
        }

        void constructAVLength() {
            assert(av_contex);
            av_length = (av_contex->duration)  * (1'000'000.0 / AV_TIME_BASE);
        }

        void constructCodec() {
            assert(av_contex);
            for (unsigned int i = 0; i < av_contex->nb_streams; ++i) {
                auto * codec_contex = av_contex->streams[i]->codec;
                if (codec_contex->codec_type == AVMEDIA_TYPE_VIDEO) {
                    /*初始化视频解码器...*/
                    auto varCodec = ffmpeg::avcodec_find_decoder(codec_contex->codec_id);
                    if (varCodec == nullptr) {
                        continue;
                    }
                    if (ffmpeg::avcodec_open2(codec_contex, varCodec, nullptr) < 0) {
                        continue;
                    }
                    auto codec = create_object_in_this_class< VideoStreamCodec >();
                    videoStreamCodec.push_back(codec);
                    codec->codec = varCodec;
                    /*set stream index ...*/
                    if ( video_stream_index.load()<0 ) {
                        video_stream_index.store( static_cast<int>(i) );
                    }
                } else if (codec_contex->codec_type == AVMEDIA_TYPE_AUDIO) {
                    /*初始化音频解码器...*/
                    auto varCodec = ffmpeg::avcodec_find_decoder(codec_contex->codec_id);
                    if (varCodec == nullptr) {
                        continue;
                    }
                    if (ffmpeg::avcodec_open2(codec_contex, varCodec, nullptr) < 0) {
                        continue;
                    }
                    auto codec = create_object_in_this_class< AudioStreamCodec >();
                    audioStreamCodec.push_back(codec);
                    codec->codec = varCodec;
                    /*******************************************/
                    codec->sample_rate = codec_contex->sample_rate;
                    /*set stream index ...*/
                    if (audio_stream_index.load() < 0) {
                        audio_stream_index.store(static_cast<int>(i));
                    }
                } else {
                    continue;
                }
            }
        }

        double getAVLength() const {
            return av_length;
        }

        std::atomic<std::uint64_t> current_player_pos{ 0 };

        void seek(double) {
        }

        bool start(double arg) {
            const auto varWanted = static_cast<std::uint64_t>(arg * 1000);
            assert(this->av_contex);
            if (varWanted == current_player_pos.load()) {
                goto play_goto;
            } else if (arg > av_length) {
                arg = 0;
            } else {

            }
            seek(arg);
        play_goto:
            return ppp_start();
        }

        AudioPlayer * audio_player{ nullptr };
        class AudioStream : public QIODevice {
        public:
            void construct(FFMPEGDecoder *) {
            }

            qint64 readData(char *data, qint64 maxSize) override {
                return maxSize;
            }

            qint64 writeData(const char *, qint64) override {
                return 0;
            }

        } *audio_stream{ nullptr };

        class ThreadSafePacketList {
            mutable std::shared_mutex mmm_Mutex;
            sstd::list< std::shared_ptr<AVPacket> > mmm_Data;
        public:
            void push_pack(std::shared_ptr<AVPacket> arg) {
                std::unique_lock varWriteLock{ mmm_Mutex };
                mmm_Data.push_back(std::move(arg));
            }
            std::shared_ptr<AVPacket> pop_front() {
                std::shared_ptr<AVPacket> varAns;
                do{
                    std::unique_lock varWriteLock{ mmm_Mutex };
                    if (mmm_Data.empty()) {
                        break;
                    }
                    varAns = std::move( mmm_Data.front() );
                    mmm_Data.pop_front();
                } while (false);
                return std::move(varAns);
            }
            std::size_t size() const {
                std::shared_lock varReadLock{ mmm_Mutex };
                return mmm_Data.size();
            }
        };
        ThreadSafePacketList list_audio;
        ThreadSafePacketList list_video;

        void start_audio() {
            audio_thread = std::thread([this]() mutable { decode_audio(); });
        }

        void start_video() {

        }

        void decode_audio() try {
            std::mutex varMutex;
            while ( false == audio_thread_quit.load() ) {
                std::unique_lock varLock{ varMutex };
                audio_thread_wait.wait_for(varLock,10ms);
                auto varPack = list_audio.pop_front();
                if (false == bool(varPack)) {
                    continue;
                }

            }
        } catch (...) {
        }

        void decode_video() {

        }

        void get_audio_pack(std::shared_ptr<AVPacket> arg) {
            list_audio.push_pack(std::move(arg));
            audio_thread_wait.notify_all();
        }

        void get_video_pack(std::shared_ptr<AVPacket> arg) {
            list_video.push_pack(std::move(arg));
            video_thread_wait.notify_all();
        }

        void read_next() try {
            std::mutex varMutex;
            while (false == read_next_thread_quit.load()) {
                std::unique_lock varLock{varMutex};
                read_next_thread_wait.wait_for(varLock,10ms);

                auto varPack = sstd::make_shared< AVPacket >();
                if (ffmpeg::av_read_frame(av_contex, varPack.get()) > 0) {
                    if ( varPack->stream_index == audio_stream_index.load()) {
                        get_audio_pack(std::move(varPack));
                    } else if (varPack->stream_index == video_stream_index.load() ) {
                        get_video_pack(std::move(varPack));
                    }
                }

            }
        } catch (...) {
        }

        void start_read_next() {
            read_next_thread = std::thread([this]() mutable { this->read_next(); });
        }

        bool ppp_start( ) {
           
            start_audio();
            start_video();
            start_read_next();

            /*初始化音频*/
            /***************************************/
            if (audio_player == nullptr) {
                audio_player = create_object_in_this_class<AudioPlayer>();
            }
            if (audio_stream == nullptr) {
                audio_stream = create_object_in_this_class<AudioStream>();
                audio_stream->construct(this);
            }
            QObject::connect(audio_player,&QAudioOutput::notify,this,
                &FFMPEGDecoder::onNotify,Qt::DirectConnection);
            audio_player->start(audio_stream);
            /***************************************/

        }

        std::atomic< int > video_stream_index{-1};
        std::atomic< int > audio_stream_index{-1};

    public:
        void onNotify() {
            ++current_player_pos;
        }
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(FFMPEGDecoder)
    };

}/*namespace this_cpp_file*/

namespace sstd {

    class PlayerPrivate : public this_cpp_file::FFMPEGDecoder {
    public:
    };

    Player::Player() {

        mmm_ErrorString = create_object_in_this_class<QString>();
        mmm_IsStart = create_object_in_this_class<bool>(false);
        mmm_ISLocalFile = create_object_in_this_class<bool>(true);
        mmm_IsConstruct = create_object_in_this_class<bool>(false);
        mmm_LocalFileName = create_object_in_this_class<QString>();
        mmm_Private = create_object_in_this_class<PlayerPrivate>();
        mmm_Private->mmm_ErrorString = mmm_ErrorString;

    }
    
    void Player::setLocalFile(const QString & arg) {
        assert((*mmm_IsStart)==false);
        (*mmm_ISLocalFile) = true;
        (*mmm_LocalFileName) = arg;
    }

    bool Player::ppp_construct() {
        assert((*mmm_IsStart) == false);
        assert((*mmm_IsConstruct)==false);
        (*mmm_IsConstruct) = true;
        if (*mmm_ISLocalFile) {
           return ppp_construct_local();
        } else {
           return ppp_construct_net();
        }
    }

    bool Player::start(double arg) {
        return mmm_Private->start(arg);
    }
    
    bool Player::ppp_construct_local() {
        return mmm_Private->construct_local_decode(*mmm_LocalFileName);
    }

    bool Player::ppp_construct_net() {
        return false;
    }

}/*namespace sstd*/


namespace {
    void on_start() {
        this_cpp_file::initFFMPEG();
    }
    Q_COREAPP_STARTUP_FUNCTION(on_start)
}/*namespace*/










