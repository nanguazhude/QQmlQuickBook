#include "MultimediaPlayer.hpp"
#include <QtGui/qimage.h>
/**********************************/
/*muti thread*/
#include <thread>
#include <mutex>
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

    class VideoStreamCodec {
    public:
        AVCodec * codec;
    };

    class AudioStreamCodec {
    public:
        AVCodec * codec;
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
        public virtual sstd::memory_lock::VirtualClassBasic {
    public:

        std::thread video_thread;
        std::thread audio_thread;
        
        sstd::vector<VideoStreamCodec *> videoStreamCodec;
        sstd::vector<AudioStreamCodec *> audioStreamCodec;

        inline FFMPEGDecoder() {
            initFFMPEG();
        }

        inline void stop() {
            /****************************************/
            if (video_thread.joinable()) {

            }
            /****************************************/
            if (audio_thread.joinable()) {

            }
            /****************************************/
            if (av_contex) {
                avformat_close_input(&av_contex);
                av_contex = nullptr;
            }
        }
        using av_error_tmp_buffer_type = std::array<char,1024*1024>;
        inline static av_error_tmp_buffer_type & getStringTmpBuffer() {
            static av_error_tmp_buffer_type varAns;
            return varAns;
        }
        QByteArray av_url_data;
        AVFormatContext *av_contex{nullptr};
        const char *av_url{nullptr};
        AVDictionary * av_information{nullptr};
        QString * mmm_ErrorString;
        /*AVInputFormat *av_input_fmt{nullptr};*/
        /*AVDictionary * av_options{nullptr};*/
        double av_length{0};
        /**/
        inline bool construct_local_decode( const QString & arg ) {
            assert(av_contex==nullptr);
            av_url_data = arg.toLocal8Bit();
            av_url = av_url_data.constData();
            auto varError = -1;
            /*try to open the stream ...*/
            varError = ffmpeg::avformat_open_input(&av_contex, av_url, nullptr, nullptr);
            if ((varError!=0)||(av_contex == nullptr)) {
                auto & varTmp = getStringTmpBuffer();
                ffmpeg::av_strerror(varError,varTmp.data(),varTmp.size());
                *mmm_ErrorString = QString::fromLocal8Bit(varTmp.data());
                return false;
            }
            /*try to read the stream information ...*/
            varError = ffmpeg::avformat_find_stream_info(av_contex,&av_information);
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
            av_length =( av_contex->duration )  * (1'000'000.0/ AV_TIME_BASE) ;
        }

        void constructCodec() {
            assert(av_contex);
            for (unsigned int i = 0; i < av_contex->nb_streams;++i ) {
                auto * codec_contex = av_contex->streams[i]->codec;
                if(codec_contex->codec_type == AVMEDIA_TYPE_VIDEO ) {
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
                   
                }  else if(codec_contex->codec_type == AVMEDIA_TYPE_AUDIO) {
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
                    
                } else {
                    continue;
                }
            }
        }

        double getAVLength() const {
            return av_length;
        }

        void start() {

        }

    public:

    private:
        SSTD_MEMORY_DEFINE(FFMPEGDecoder)
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

    bool Player::start() {
        return false;
    }
    
    bool Player::ppp_construct_local() {
        return mmm_Private->construct_local_decode(*mmm_LocalFileName);
    }

    bool Player::ppp_construct_net() {
        return false;
    }

}/*namespace sstd*/













