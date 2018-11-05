#include "MultimediaPlayer.hpp"
#include "private_FFMPEGDecoder.hpp"
/**********************************/
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
#define ffmpeg /**//*模拟c++命名空间*//**/
#endif

    /*将C语言AVPcket包装成C++模式*/
    class CPPAVPacket : public ffmpeg::AVPacket {
        CPPAVPacket() = default;
    public:
        static std::shared_ptr<CPPAVPacket> create() {
            auto varAnsPointer = ffmpeg::av_packet_alloc();
            ffmpeg::av_init_packet(varAnsPointer);
            auto varAns = std::shared_ptr<ffmpeg::AVPacket>{ varAnsPointer, [](auto * d) {
               ffmpeg::av_packet_unref(d);
               ffmpeg::av_packet_free(&d);
           } , sstd::allocator<ffmpeg::AVPacket>{} };
            return { varAns,reinterpret_cast<CPPAVPacket *>(varAnsPointer) };
        }
    };

    /*将C语言AVFrame包装成C++模式*/
    class CPPAVFrame : public ffmpeg::AVFrame {
        CPPAVFrame() = default;
    public:
        static std::shared_ptr<CPPAVFrame> create() {
            auto varAnsPointer = ffmpeg::av_frame_alloc();
            auto varAns = std::shared_ptr<ffmpeg::AVFrame>{ varAnsPointer, [](auto * d) {
                ffmpeg::av_frame_unref(d);
                ffmpeg::av_frame_free(&d);
            },sstd::allocator<ffmpeg::AVFrame>{} };
            return { varAns,reinterpret_cast<CPPAVFrame *>(varAnsPointer) };
        }
    };

    /*构造QAudioFormat*/
    inline QAudioFormat getAudioFormat(int rate = 8000) {
        /****************************************************************/
        /*统一将音频转为双声道，signed 16 整型，但采样频率不变*/
        static const auto varAns = []() {
            QAudioFormat varFormat;
            varFormat.setSampleRate(8000);
            varFormat.setChannelCount(2);
            varFormat.setSampleSize(16);
            varFormat.setCodec(QStringLiteral("audio/pcm"));
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
            varFormat.setByteOrder(QAudioFormat::BigEndian);
#else
            varFormat.setByteOrder(QAudioFormat::LittleEndian);
#endif
            varFormat.setSampleType(QAudioFormat::SignedInt);
            return varFormat;
        }();
        /****************************************************************/
        auto var = varAns;
        var.setSampleRate(rate);
        return var;
    }

    /*构造QAudioDeviceInfo*/
    inline QAudioDeviceInfo getAudioDeveceInfo(int rate = 8000) {

        /*采用默认输出设备*/
        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

        if (!info.isFormatSupported(getAudioFormat(rate))) {
            qWarning() << "Raw audio format not supported by backend, cannot play audio.";
            std::exit(-999);
        }

        return info;
    }

    /*音频输出设备*/
    class AudioPlayer : public QAudioOutput {
    public:
        AudioPlayer(int rate) :
            QAudioOutput(getAudioDeveceInfo(rate), getAudioFormat(rate)) {
            this->setNotifyInterval(1);
        }
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(AudioPlayer)
    };

    /*video stream 解码类*/
    class VideoStreamCodec {
    public:
        ffmpeg::AVCodec *        codec{ nullptr };
        ffmpeg::AVCodecContext * contex{ nullptr };
        int width{ 512 };
        int height{ 512 };
        ffmpeg::SwsContext *  reSampleContex{ nullptr };
        std::shared_ptr< CPPAVFrame > frame;
        void constructReSampleContex() {
            reSampleContex = ffmpeg::sws_getContext(
                width, height, contex->pix_fmt,
                width, height, AV_PIX_FMT_RGBA/**/,
                /**/SWS_FAST_BILINEAR/**/,
                nullptr, nullptr, nullptr
            );
        }

        void destoryReSampleContex() {
            if (reSampleContex) {
                ffmpeg::sws_freeContext(reSampleContex);
                reSampleContex = nullptr;
            }
        }

        ~VideoStreamCodec() {
            destoryReSampleContex();
            frame.reset();
        }
    private:
        SSTD_MEMORY_DEFINE(VideoStreamCodec)
    };

    /*audio stream 解码类*/
    class AudioStreamCodec {
    public:
        ffmpeg::AVCodec * codec{ nullptr };
        ffmpeg::AVCodecContext * contex{ nullptr };
        int sample_rate{ 44100 };
        ffmpeg::SwrContext * reSampleContex{ nullptr };
        std::shared_ptr< CPPAVFrame > frame;
        void constructReSampleContex() {
            //https://blog.csdn.net/timesir/article/details/52904024
            reSampleContex = ffmpeg::swr_alloc_set_opts(
                /*s*/nullptr,
                /*双声道*/AV_CH_LAYOUT_STEREO,
                /*signed 16*/AV_SAMPLE_FMT_S16,
                this->sample_rate,
                contex->channel_layout,
                contex->sample_fmt,
                contex->sample_rate,
                0,
                nullptr
            );
            ffmpeg::swr_init(reSampleContex);
        }

        void destoryReSampleContex() {
            if (reSampleContex) {
                ffmpeg::swr_free(&reSampleContex);
            }
        }

        ~AudioStreamCodec() {
            destoryReSampleContex();
            frame.reset();
        }

    public:
        SSTD_MEMORY_DEFINE(AudioStreamCodec)
    };

    /*解码视频数据包装*/
    class FrameImage {
    public:
        double pts{ 0 }/*播放时间戳 : 单位s */;
        QImage data/*数据*/;
    };

    /*初始化ffmpeg时使用的环境变量*/
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

    /*解码器*/
    class FFMPEGDecoder :
        public sstd_private::FFMPEGDecoderPrivate,
        public virtual sstd::memory_lock::VirtualClassBasic {
    public:

        /*video 解码线程*/
        std::thread video_thread;
        std::atomic_bool video_thread_quit{ false };
        std::condition_variable video_thread_wait;

        /*audio 解码线程*/
        std::thread audio_thread;
        std::atomic_bool audio_thread_quit{ false };
        std::condition_variable audio_thread_wait;

        /*解包线程*/
        std::thread read_next_thread;
        std::atomic_bool read_next_thread_quit{ false };
        std::condition_variable read_next_thread_wait;

        /*可用解码器*/
        sstd::map<int, VideoStreamCodec *> videoStreamCodec;
        sstd::map<int, AudioStreamCodec *> audioStreamCodec;

        inline FFMPEGDecoder() {
            initFFMPEG();
            connect(this, &FFMPEGDecoder::checkImageChange,
                this, &FFMPEGDecoder::doCheckImageChange, 
                Qt::QueuedConnection);
        }

        std::atomic<double> last_image_pts{ -1 };
        void doCheckImageChange() {
            auto varCurrentAudio = audio_stream_index.load();
            if (varCurrentAudio < 0) {
                return;
            }
            int varSampleInBuffer{ 0 };
            if (audio_player) {
                varSampleInBuffer = ((audio_player->bytesFree()) >> 2);
            }
            auto & varCodec = audioStreamCodec[varCurrentAudio];
            auto varTimeNotInBuffer = varSampleInBuffer / double(varCodec->contex->sample_rate)*1000.0;
            auto varTimeInBuffer = this->time_in_audio_buffer.load();
            auto varStartPos = start_player_pos.load();
            auto varCurrentTime = varTimeNotInBuffer - varTimeInBuffer + varStartPos;
            varCurrentTime /= 1000/*convert to s*/;

            QImage varAnsImage;
            double varCurrentPtsTime{ 0 };
            double varFirstPts{ 0 };
            QImage varFirstImage;
            {
                std::shared_lock varReadLock{ mutexVideoRawData };
                if (videoRawData.empty()) {
                    return;
                }
                varFirstPts = videoRawData[0]->pts;
                varFirstImage = videoRawData[0]->data;
            }

            do {

                if (varCurrentTime > varFirstPts) {

                    std::unique_lock varWriteLock{ mutexVideoRawData };
                    auto varPos = videoRawData.begin();
                    const auto varEnd = videoRawData.end();

                    for (; varPos != varEnd; ++varPos) {
                        if (varCurrentTime > ((*varPos)->pts)) {
                            continue;
                        } else {
                            break/*for*/;
                        }
                    }

                    auto varAns = varPos - 1;
                    if (varAns == videoRawData.begin()) {

                    } else {
                        varCurrentPtsTime = (*varAns)->pts;
                        varAnsImage = (*varAns)->data;
                        videoRawData.erase(videoRawData.begin(), varAns);
                        break/*do*/;

                    }

                }

                varCurrentPtsTime = varFirstPts;
                varAnsImage = varFirstImage;
                break;

            } while (false);

            if (varCurrentPtsTime != last_image_pts.load()) {
                last_image_pts.store(varCurrentPtsTime);
                imageChanged(varAnsImage);
            }

        }/*doCheckImageChange*/

        bool is_stoped{ false };
        inline void stop() {
            if (is_stoped) {
                return;
            }
            is_stoped = true;
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
            /*the av contex destruct must run in main thread ... */
            sstd::runInMainThread([this]() {
                if (av_contex) {
                    avformat_close_input(&av_contex);
                }
            });
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
        /*this must run in main thread ...*/
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
            varError = ffmpeg::avformat_find_stream_info(av_contex, nullptr);
            if (varError != 0) {
                auto & varTmp = getStringTmpBuffer();
                ffmpeg::av_strerror(varError, varTmp.data(), varTmp.size());
                *mmm_ErrorString = QString::fromLocal8Bit(varTmp.data());
                return false;
            }
            /***********************************/
            ffmpeg::av_find_best_stream(av_contex, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
            ffmpeg::av_find_best_stream(av_contex, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
            /***********************************/
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
                    videoStreamCodec.emplace(static_cast<int>(i), codec);
                    codec->codec = varCodec;
                    codec->contex = codec_contex;
                    codec->width = codec_contex->width;
                    codec->height = codec_contex->height;
                    /*set stream index ...*/
                    if (video_stream_index.load() < 0) {
                        video_stream_index.store(static_cast<int>(i));
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
                    audioStreamCodec.emplace(static_cast<int>(i), codec);
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

        std::atomic_bool need_reinit_current_player_pos{ false };
        std::atomic<std::uint64_t> current_player_pos{ 0 }/*当前播放时间，单位 ：ms*/;
        std::atomic<std::uint64_t> start_player_pos{ 0 }/*开始播放时刻，单位 ： ms*/;
        void seek(double arg) {
            /*初始化一个值...*/
            current_player_pos.store(arg * 1000);
            start_player_pos.store(current_player_pos.load());
            time_in_audio_buffer.store(0);
            /*标记current_player_pos为dirty*/
            need_reinit_current_player_pos = true;
            /*进行定位...*/

        }

        bool start(QThread * argt, double arg) {
            this->moveToThread(argt);
            notify_count.store(9999);
            const auto varWanted = static_cast<std::uint64_t>(arg * 1000);
            assert(this->av_contex);
            if (varWanted == current_player_pos.load()) /*继续播放...*/ {
                goto play_goto;
            } else if (arg > av_length) {/*从头播放...*/
                arg = 0;
            }
            this->seek(arg);
        play_goto:
            return ppp_start();
        }

        union AudioChar {
            std::int16_t uData;
            std::uint8_t cData[2];
        };

        std::shared_mutex mutexAudioRawData;
        sstd::deque< std::pair< AudioChar, AudioChar > > audioRawData;
        std::atomic<double> time_in_audio_buffer{ 0 };

        AudioPlayer * audio_player{ nullptr };
        class AudioStream : public QIODevice {
            FFMPEGDecoder * super{ nullptr };
        public:

            void construct(FFMPEGDecoder *a) {
                super = a;
                this->open(QIODevice::ReadOnly);
            }

            qint64 bytesAvailable() const {
                std::shared_lock varLock{ super->mutexAudioRawData };
                return super->audioRawData.size() * sizeof(super->audioRawData[0]);
            }

            qint64 readData(char *data, qint64 maxSize) override {

                qint64 varRawSize{ 0 };
                const auto varSize = (maxSize >> 2);
                auto audioCodec = super->audioStreamCodec[super->audio_stream_index];

                {
                    static_assert(4 == sizeof(std::pair< AudioChar, AudioChar  >));
                    sstd::vector< std::pair< AudioChar, AudioChar > > varTmp;
                    assert(0 == (maxSize & 3));
                    {
                        std::unique_lock varReadLock{ super->mutexAudioRawData };
                        varRawSize = static_cast<qint64> (super->audioRawData.size());
                        if (varSize <= varRawSize) {
                            auto varPosBegin = super->audioRawData.begin();
                            auto varPosEnd = super->audioRawData.begin() + varSize;
                            varTmp = sstd::vector< std::pair< AudioChar, AudioChar > >{
                                varPosBegin,
                                varPosEnd };
                            super->audioRawData.erase(
                                varPosBegin,
                                varPosEnd);
                            varRawSize -= varSize;
                        } else if (varRawSize > 0) {
                            auto varPosBegin = super->audioRawData.begin();
                            auto varPosEnd = super->audioRawData.begin() + varRawSize;
                            varTmp = sstd::vector< std::pair< AudioChar, AudioChar > >{
                               varPosBegin,
                               varPosEnd };
                            super->audioRawData.erase(
                                varPosBegin,
                                varPosEnd);
                            varRawSize = 0;
                        } else {
                            qDebug() << "data not enough ... ";
                            super->setNeedData();
                            return 0;
                        }
                    }
                    maxSize = (varTmp.size() << 2);
                    ::memcpy(data, varTmp.data(), maxSize);
                }

                if (varRawSize > std::max<qint64>(varSize, audioCodec->sample_rate / 36)) {
                    super->need_data.store(false);
                } else {
                    super->setNeedData();
                }

                /************************************/
                auto varReadMS = (maxSize >> 2) / double(audioCodec->contex->sample_rate)*1000.0;
                super->time_in_audio_buffer = varReadMS + super->time_in_audio_buffer.load();
                /************************************/

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
                do {
                    std::unique_lock varWriteLock{ mmm_Mutex };
                    if (mmm_Data.empty()) {
                        break;
                    }
                    varAns = std::move(mmm_Data.front());
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
            video_thread = std::thread([this]()mutable { decode_video(); });
        }

        void decode_audio() try {
            std::mutex varMutex;
            while (false == audio_thread_quit.load()) {
                const auto & varCodec = this->audioStreamCodec[this->audio_stream_index.load()];
                std::unique_lock varLock{ varMutex };
                audio_thread_wait.wait_for(varLock, 10ms,
                    [this]() mutable {return need_data.load(); });
                {
                    if (false == need_data.load()) {
                        continue;
                    }
                    if (audio_thread_quit.load()) {
                        return;
                    }
                }
                auto varPack = list_audio.pop_front();
                if (false == bool(varPack)) {
                    continue;
                }
                /*about to decode ...*/
                auto varError = ffmpeg::avcodec_send_packet(varCodec->contex, varPack.get());
                if (varError) {
                    continue;
                }
                if (false == bool(varCodec->frame)) {
                    varCodec->frame = CPPAVFrame::create();
                }
                auto varFrame = varCodec->frame;
                class FrameLock {
                    CPPAVFrame * const d;
                    CPPAVPacket * const p;
                public:
                    FrameLock(CPPAVFrame * f, CPPAVPacket *b) :d(f), p(b) {
                    }
                    ~FrameLock() {
                        ffmpeg::av_frame_unref(d);
                        ffmpeg::av_packet_unref(p);
                    }
                }varLockFrame{ varFrame.get(),varPack.get() };
                /*decode ...*/
                varError = ffmpeg::avcodec_receive_frame(varCodec->contex, varFrame.get());
                if (varError) {
                    continue;
                }
                /*scale ...*/
                //get the really pts ...
                if (need_reinit_current_player_pos.load()) {
                    auto varPts = varFrame->pts * ffmpeg::av_q2d(varCodec->contex->time_base);
                    current_player_pos.store(varPts * 1000);
                    start_player_pos.store(current_player_pos.load());
                }
                //重采样
                sstd::vector< std::pair<AudioChar, AudioChar> > varData;
                varData.resize(varFrame->nb_samples);
                std::uint8_t * varDataRaw[1]{ reinterpret_cast<std::uint8_t *>(varData.data()) };
                if (varCodec->reSampleContex == nullptr) {
                    varCodec->constructReSampleContex();
                }
                ffmpeg::swr_convert(
                    varCodec->reSampleContex,
                    varDataRaw,
                    varFrame->nb_samples,
                    const_cast<const uint8_t **>(varFrame->extended_data),
                    varFrame->nb_samples
                );
                std::unique_lock varReadLock{ this->mutexAudioRawData };
                this->audioRawData.insert(audioRawData.end(),
                    varData.begin(),
                    varData.end());
                if (std::rand() & 1) {/*remove data do not used*/
                    this->audioRawData.shrink_to_fit();
                }
            }
        } catch (...) {
        }

        std::shared_mutex mutexVideoRawData;
        sstd::vector< std::shared_ptr<FrameImage> > videoRawData;
        void decode_video() {
            const auto varVideoStreamIndex = this->video_stream_index.load();
            if (varVideoStreamIndex < 0) {
                return;
            }
            std::mutex varMutex;
            while (false == video_thread_quit.load()) {
                const auto & varCodec = this->videoStreamCodec[varVideoStreamIndex];
                std::unique_lock varLock{ varMutex };
                video_thread_wait.wait_for(varLock, 10ms,
                    [this]() mutable {return need_data.load(); });
                {
                    if (false == need_data.load()) {
                        continue;
                    }
                    if (video_thread_quit.load()) {
                        return;
                    }
                }
                auto varPack = list_video.pop_front();
                if (false == bool(varPack)) {
                    continue;
                }
                /*about to decode ...*/
                auto varError = ffmpeg::avcodec_send_packet(varCodec->contex, varPack.get());
                if (varError) {
                    continue;
                }
                if (false == bool(varCodec->frame)) {
                    varCodec->frame = CPPAVFrame::create();
                }
                auto varFrame = varCodec->frame;
                class FrameLock {
                    CPPAVFrame * const d;
                    CPPAVPacket * const p;
                public:
                    FrameLock(CPPAVFrame * f, CPPAVPacket *b) :d(f), p(b) {
                    }
                    ~FrameLock() {
                        ffmpeg::av_frame_unref(d);
                        ffmpeg::av_packet_unref(p);
                    }
                }varLockFrame{ varFrame.get(),varPack.get() };
                /*decode ...*/
                varError = ffmpeg::avcodec_receive_frame(varCodec->contex, varFrame.get());
                if (varError) {
                    continue;
                }
                /*重采样*/
                if (nullptr == varCodec->reSampleContex) {
                    varCodec->constructReSampleContex();
                }
                QImage varAnsImage{ varCodec->width,varCodec->height ,QImage::Format_RGBA8888 };
                ffmpeg::uint8_t * out[1]{ const_cast<ffmpeg::uint8_t *>(varAnsImage.constBits()) };
                int out_size[4]{ varAnsImage.bytesPerLine(),
                    0,
                    0,
                    0 };
                //https://stackoverflow.com/questions/13088749/efficient-conversion-of-avframe-to-qimage
                /*av_image_fill_linesizes(out_size, AV_PIX_FMT_RGBA, varFrame->width);*/
                ffmpeg::sws_scale(
                    varCodec->reSampleContex,
                    varFrame->data,
                    varFrame->linesize,
                    0,
                    varFrame->height,
                    out,
                    out_size);

                {
                    auto varFrameImage = sstd::make_shared<FrameImage>();
                    varFrameImage->data = varAnsImage;
                    varFrameImage->pts = varFrame->pts * ffmpeg::av_q2d(varCodec->contex->time_base);
                    std::unique_lock varWriteLock{ mutexVideoRawData };
                    videoRawData.push_back(std::move(varFrameImage));
                }

            }/*while...*/
        }

        void get_audio_pack(std::shared_ptr<CPPAVPacket> arg) {
            list_audio.push_pack(std::move(arg));
            audio_thread_wait.notify_all();
        }

        void get_video_pack(std::shared_ptr<CPPAVPacket> arg) {
            list_video.push_pack(std::move(arg));
            video_thread_wait.notify_all();
        }

        class PackPool {
            enum {
                raise_size = 8
            };
            std::shared_mutex mmm_pool_Mutex;
            sstd::list< std::shared_ptr<CPPAVPacket> > mmm_pool_Data;
            static void init_size_(sstd::list< std::shared_ptr<CPPAVPacket> > * arg) {
                for (int i = 0; i < raise_size; ++i) {
                    arg->push_back(CPPAVPacket::create());
                }
            }
        public:
            PackPool() {
                init_size_(&mmm_pool_Data);
            }
            std::shared_ptr<CPPAVPacket> getAPacket() {
                std::unique_lock varWriteLock{ mmm_pool_Mutex };
                std::shared_ptr<CPPAVPacket> varAns;
                for (auto & i : mmm_pool_Data) {
                    if (i.use_count() > 1) {
                        continue;
                    }
                    ffmpeg::av_packet_unref(i.get());
                    if (bool(varAns) == false) {
                        varAns = i;
                    }
                }/*for...*/
                if (bool(varAns) == false) {
                    init_size_(&mmm_pool_Data);
                    varAns = *mmm_pool_Data.rbegin();
                }
                return std::move(varAns);
            }
        } mmm_PackPool;

        void read_next() try {
            std::mutex varMutex;
            while (false == read_next_thread_quit.load()) {
                std::unique_lock varLock{ varMutex };
                read_next_thread_wait.wait_for(varLock, 10ms,
                    [this]() mutable {return need_data.load(); });
                {
                    if (false == need_data.load()) {
                        continue;
                    }
                    if (read_next_thread_quit.load()) {
                        return;
                    }
                }
                int varReadNext = 16;
                auto varPack = mmm_PackPool.getAPacket();
                bool isReadNoError = (ffmpeg::av_read_frame(av_contex, varPack.get()) == 0);
                if (isReadNoError && (varReadNext > 0)) {
                    if (varPack->stream_index == audio_stream_index.load()) {
                        --varReadNext;
                        get_audio_pack(std::move(varPack));
                    } else if (varPack->stream_index == video_stream_index.load()) {
                        --varReadNext;
                        get_video_pack(std::move(varPack));
                    } else {/*the pack will never be used ...*/
                        ffmpeg::av_packet_unref(varPack.get());
                    }
                }

            }
        } catch (...) {
        }

        void start_read_next() {
            setNeedData();
            read_next_thread = std::thread([this]() mutable { this->read_next(); });
        }

        QMetaObject::Connection on_finished_connect;
        bool ppp_start() {

            if (on_finished_connect) {
                disconnect(on_finished_connect);
            }

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
                const auto var = audioStreamCodec[audio_stream_index.load()]->sample_rate;
                audio_player = create_object_in_this_class<AudioPlayer>(
                    /*simple*/var);
            }
            if (audio_stream == nullptr) {
                audio_stream = create_object_in_this_class<AudioStream>();
                audio_stream->construct(this);
            }
            connect(audio_player, &QAudioOutput::notify, this,
                &FFMPEGDecoder::onNotify, Qt::DirectConnection);
            if constexpr (false) {/*调试*/
                connect(audio_player, &QAudioOutput::stateChanged,
                    [this](auto s) {
                    qDebug() << s;
                    qDebug() << audio_player->error(); }
                );
            }

            {
                on_finished_connect = connect(audio_player, &QAudioOutput::stateChanged,
                    [this](auto s) {
                    if (on_finished_connect) {
                        disconnect(on_finished_connect);
                    }
                    if (s == QAudio::StoppedState) {
                        super->finished();
                    }
                });
            }

            /*wait for decode some data ...*/
            std::this_thread::sleep_for(32ms);
            /*start ...*/
            audio_player->setVolume(1);
            audio_player->start(audio_stream);
            /***************************************/

            return true;
        }

        std::atomic< int > video_stream_index{ -1 };
        std::atomic< int > audio_stream_index{ -1 };
        std::atomic_bool need_data{ false };

        void setNeedData() {
            need_data.store(true);
            video_thread_wait.notify_all();
            audio_thread_wait.notify_all();
            read_next_thread_wait.notify_all();
        }

    public:
        sstd::Player * super;
        std::atomic<int> notify_count{ 9999 };
        void onNotify() {
            const auto varValue = ++current_player_pos;
            if ((varValue / 1000.0) > av_length) {
                super->finished();
                this->stop();
            }
            ++notify_count;
            if (notify_count.load() > 10) {
                this->checkImageChange();
                notify_count = 0;
            }
        }
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(FFMPEGDecoder)
    };

}/*namespace this_cpp_file*/

namespace sstd {

    class PlayerPrivate : public this_cpp_file::FFMPEGDecoder {
    public:
        SSTD_MEMORY_QOBJECT_DEFINE(PlayerPrivate)
    };

    Player::Player() {

        mmm_ErrorString = create_object_in_this_class<QString>();
        mmm_IsStart = create_object_in_this_class<bool>(false);
        mmm_ISLocalFile = create_object_in_this_class<bool>(true);
        mmm_IsConstruct = create_object_in_this_class<bool>(false);
        mmm_LocalFileName = create_object_in_this_class<QString>();
        mmm_Private = create_object_in_this_class<PlayerPrivate>();
        mmm_Private->mmm_ErrorString = mmm_ErrorString;
        mmm_Private->super = this;
    }

    void Player::setLocalFile(const QString & arg) {
        assert((*mmm_IsStart) == false);
        (*mmm_ISLocalFile) = true;
        (*mmm_LocalFileName) = arg;
    }

    bool Player::ppp_construct() {
        assert((*mmm_IsStart) == false);
        assert((*mmm_IsConstruct) == false);
        (*mmm_IsConstruct) = true;
        if (*mmm_ISLocalFile) {
            return ppp_construct_local();
        } else {
            return ppp_construct_net();
        }
    }

    bool Player::start(QThread * argt, double arg) {
        this->moveToThread(argt);
        (*mmm_IsStart) = mmm_Private->start(argt, arg);
        return *mmm_IsStart;
    }

    bool Player::ppp_construct_local() {
        /*this function must run in main thread ... */
        assert( QThread::currentThread() == ( qApp->thread() ) )      ;
        return mmm_Private->construct_local_decode(*mmm_LocalFileName);
    }

    bool Player::ppp_construct_net() {
        return false;
    }

    PlayerThread::PlayerThread() {

    }

    void PlayerThread::stop() {
        this->runInThisThread([this]() {
            if (mmm_Player) {
                delete mmm_Player   ;
                mmm_Player = nullptr;
            }
            this->quit();
        });
    }

    void PlayerThread::start(Player * argPlayer, double argTime) {
        if (argPlayer == nullptr) {
            return;
        }
        mmm_isPlayerSet = true;
        mmm_Player = argPlayer;
        argPlayer->moveToThread(this);
        connect(argPlayer, &Player::finished, this, &PlayerThread::stop);
        connect(this, &PlayerThread::finished, this, &PlayerThread::stop);
        this->runInThisThread([argPlayer, argTime,this]() {
            argPlayer->start(this,argTime);
        });
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
//https://blog.csdn.net/encoder1234/article/details/78832618
//ffmpeg avformat_open_input  avcodec_send_packet avcodec_receive_frame
//struct SwsContext （software scale） 主要用于视频图像的转换，比如格式转换： 参考    ffmpeg中的sws_scale算法性能测试
//struct SwrContext （software resample） 主要用于音频重采样，比如采样率转换，声道转换。 参考： SwrContext重采样结构体
//https://blog.csdn.net/jammg/article/details/52688506
//https://blog.csdn.net/Chasing_Chasing/article/details/79092190

