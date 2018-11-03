#include "MultimediaPlayer.hpp"
#include <QtGui/qimage.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>
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

    class CPPAVPacket : public ffmpeg::AVPacket {
    public:
        CPPAVPacket() {
            ffmpeg::av_init_packet(this);
        }
        ~CPPAVPacket() {
            ffmpeg::av_packet_unref(this);
        }
    };

    class CPPAVFrame : public ffmpeg::AVFrame {
    public:
        static std::shared_ptr<CPPAVFrame> create() {
            auto varAnsPointer = ffmpeg::av_frame_alloc();
            auto varAns = std::shared_ptr<ffmpeg::AVFrame>{ varAnsPointer, [](auto * d) {
                ffmpeg::av_frame_free(&d);
            } };
            return { varAns,reinterpret_cast<CPPAVFrame *>(varAnsPointer) };
        }
        /*av_frame_unref*/
    };
      
    inline QAudioFormat getAudioFormat(int rate = 8000 ) {
        static const auto varAns = []() {
            QAudioFormat varFormat;
            varFormat.setSampleRate(8000);
            varFormat.setChannelCount(2);
            varFormat.setSampleSize(16);
            varFormat.setCodec("audio/pcm");
            varFormat.setByteOrder(QAudioFormat::LittleEndian);
            varFormat.setSampleType(QAudioFormat::UnSignedInt);
            return varFormat;
        }();
        auto var = varAns;
        var.setSampleRate(rate);
        return var;
    }

    inline QAudioDeviceInfo getAudioDeveceInfo(int rate = 8000) {

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

        if (!info.isFormatSupported(getAudioFormat(rate ))) {
            qWarning() << "Raw audio format not supported by backend, cannot play audio.";
            std::exit(-999);
        }

        return info;
    }

    class AudioPlayer : public QAudioOutput {
    public:
        AudioPlayer(int rate ) : QAudioOutput(getAudioDeveceInfo(rate ),getAudioFormat( rate )){
            this->setNotifyInterval(1);
        }
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(AudioPlayer)
    };

    class VideoStreamCodec {
    public:
        AVCodec *        codec{nullptr};
        AVCodecContext * contex{nullptr};
    private:
        SSTD_MEMORY_DEFINE(VideoStreamCodec)
    };

    class AudioStreamCodec {
    public:
        AVCodec * codec{nullptr};
        AVCodecContext * contex{nullptr};
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
            ffmpeg::avformat_network_init();
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

        sstd::map<int,VideoStreamCodec *> videoStreamCodec;
        sstd::map<int,AudioStreamCodec *> audioStreamCodec;

        inline FFMPEGDecoder() {
            initFFMPEG();
        }

        inline void stop() {
            need_data.store(false);
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
        /*AVDictionary * av_information{ nullptr };*/
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
            varError = ffmpeg::avformat_find_stream_info(av_contex, nullptr );
            if ( varError != 0  ) {
                auto & varTmp = getStringTmpBuffer();
                ffmpeg::av_strerror(varError, varTmp.data(), varTmp.size());
                *mmm_ErrorString = QString::fromLocal8Bit(varTmp.data());
                return false;
            }
            constructCodec();
            constructAVLength();
            return true;
        }

        inline ~FFMPEGDecoder() {
            stop();
        }

        void constructAVLength() {
            assert(av_contex);
            av_length = (av_contex->duration)  * (1.0 / AV_TIME_BASE);
        }

        /*error!*/
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
                    videoStreamCodec.emplace(static_cast<int>(i),codec);
                    codec->codec = varCodec;
                    codec->contex = codec_contex;
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
                    /*avcodec_close*/
                    if (ffmpeg::avcodec_open2(codec_contex, varCodec, nullptr) < 0) {
                        continue;
                    }
                    auto codec = create_object_in_this_class< AudioStreamCodec >();
                    audioStreamCodec.emplace( static_cast<int>(i) , codec );
                    codec->codec = varCodec;
                    codec->contex = codec_contex;
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

        union AudioChar {
            std::uint16_t uData;
            std::uint8_t cData[2];
        };

        std::mutex mutexAudioRawData;
        sstd::vector< AudioChar > audioRawData;

        AudioPlayer * audio_player{ nullptr };
        class AudioStream : public QIODevice {
            FFMPEGDecoder * super{nullptr};
        public:

            void construct(FFMPEGDecoder *a) {
                super = a;
            }

            qint64 readData(char *data, qint64 maxSize) override {
                
                qint64 varRawSize{0};
                
                {
                    auto varSize = maxSize >> 2;
                    std::unique_lock varReadLock{ super->mutexAudioRawData };
                    varRawSize = static_cast<qint64> (super->audioRawData.size());
                    if (varSize <= varRawSize) {
                        ::memcpy(data, super->audioRawData.data(), maxSize);
                        super->audioRawData.erase(
                            super->audioRawData.begin(),
                            super->audioRawData.begin() + varSize);
                    } else {
                        return 0;
                    }
                }

                if ( varRawSize > 1600*4 ) {
                    super->need_data.store(false);
                } else {
                    super->setNeedData( );
                }

                return maxSize;
            }

            qint64 writeData(const char *, qint64) override {
                return 0;
            }

        } *audio_stream{ nullptr };

        class ThreadSafePacketList {
            mutable std::shared_mutex mmm_Mutex;
            sstd::list< std::shared_ptr<CPPAVPacket> > mmm_Data;
        public:
            void push_pack(std::shared_ptr<CPPAVPacket> arg) {
                std::unique_lock varWriteLock{ mmm_Mutex };
                mmm_Data.push_back(std::move(arg));
            }
            std::shared_ptr<CPPAVPacket> pop_front() {
                std::shared_ptr<CPPAVPacket> varAns;
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
                const auto & varCodec = this->audioStreamCodec[ this->audio_stream_index.load()];
                std::unique_lock varLock{ varMutex };
                audio_thread_wait.wait_for(varLock, 10ms, 
                    [this]() {return need_data.load(); });
                auto varPack = list_audio.pop_front();
                if (false == bool(varPack)) {
                    continue;
                }
                auto varError = ffmpeg::avcodec_send_packet(varCodec->contex , varPack.get()  );
                auto varFrame = CPPAVFrame::create() ;
                varError = ffmpeg::avcodec_receive_frame(varCodec->contex , varFrame.get() );
                if (varError) {
                    continue;
                }
                
            }
        } catch (...) {
        }

        void decode_video() {

        }

        void get_audio_pack(std::shared_ptr<CPPAVPacket> arg) {
            list_audio.push_pack(std::move(arg));
            audio_thread_wait.notify_all();
        }

        void get_video_pack(std::shared_ptr<CPPAVPacket> arg) {
            return;
            list_video.push_pack(std::move(arg));
            video_thread_wait.notify_all();
        }

        void read_next() try {
            std::mutex varMutex;
            while (false == read_next_thread_quit.load()) {
                std::unique_lock varLock{varMutex};
                read_next_thread_wait.wait_for(varLock,10ms,
                    [this]() { return need_data.load();  } );
                int varReadNext = 16;
                auto varPack = sstd::make_shared< CPPAVPacket >();
                if ((ffmpeg::av_read_frame(av_contex, varPack.get()) == 0)&&(varReadNext>0)) {
                    if ( varPack->stream_index == audio_stream_index.load()) {
                        --varReadNext;
                        get_audio_pack(std::move(varPack));
                    } else if (varPack->stream_index == video_stream_index.load() ) {
                        --varReadNext;
                        get_video_pack(std::move(varPack));
                    }
                }

            }
        } catch (...) {
        }

        void start_read_next() {
            setNeedData();
            read_next_thread = std::thread([this]() mutable { this->read_next(); });
        }

        bool ppp_start( ) {
           
            if (audio_stream_index.load() < 0) {
                (*mmm_ErrorString) = QStringLiteral("empty audo");
                return false;
            }

            start_audio();
            start_video();
            start_read_next();

            /*初始化音频*/
            /***************************************/
            if (audio_player == nullptr) {
                audio_player = create_object_in_this_class<AudioPlayer>(
                    /*simple*/audioStreamCodec[audio_stream_index.load()]->sample_rate);
            }
            if (audio_stream == nullptr) {
                audio_stream = create_object_in_this_class<AudioStream>();
                audio_stream->construct(this);
            }
            QObject::connect(audio_player,&QAudioOutput::notify,this,
                &FFMPEGDecoder::onNotify,Qt::DirectConnection);
            /*wait for decode some data ...*/
            std::this_thread::sleep_for(32ms);
            /*start ...*/
            audio_player->start(audio_stream);
            /***************************************/

            return true;
        }

        std::atomic< int > video_stream_index{-1};
        std::atomic< int > audio_stream_index{-1};
        std::atomic_bool need_data{false};
        void setNeedData() {
            need_data.store(true);
            video_thread_wait.notify_all();
            audio_thread_wait.notify_all();
            read_next_thread_wait.notify_all();
        }

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

//http://www.cnblogs.com/wangguchangqing/p/5900426.html
//https://blog.csdn.net/chuyouyinghe/article/details/78611121
//https://blog.csdn.net/xfgryujk/article/details/52986137
//https://blog.csdn.net/xfgryujk/article/details/52986137?locationNum=11&fps=1
//http://hasanaga.info/tag/ffmpeg-libavcodec-avformat_open_input-example/
//https://blog.csdn.net/leixiaohua1020/article/details/8652605
//ffmpeg avformat_open_input  avcodec_send_packet avcodec_receive_frame






