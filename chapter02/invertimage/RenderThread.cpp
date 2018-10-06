#include <sstd_glew.hpp>
#include "RenderThread.hpp"
#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtGui/qimage.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qwindow.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qopenglcontext.h>
#include <QtGui/qoffscreensurface.h>
#include <ConstructQSurface.hpp>
extern bool glewInitialize();

sstd::RenderThread::RenderThread() {
    assert(qApp);
    {
        /*thread完成时自删除*/
        this->moveToThread(qApp->thread());
        connect(this, &QThread::finished, this, &QThread::deleteLater);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
            if (this->isRunning()) {
                this->quit();
                return;
            }
            this->deleteLater();
        });
    }
    /*此函数必须在main thread调用*/
    mmm_Surface = sstdNew<QOffscreenSurface>();
    mmm_Surface->setFormat(sstd::getDefaultOpenGLFormat());
    mmm_Surface->create();
}

void sstd::RenderThread::start(const QString & arg) {
    mmm_ImageFileName = arg;
    Super::start();
}

namespace {

    class Render {
        QOpenGLContext * mmm_OpenGLContex = nullptr;
    public:
        Render(sstd::RenderThread * arg) {
            /*this function will run in any thread*/
            mmm_OpenGLContex = sstdNew<QOpenGLContext>();
            mmm_OpenGLContex->setFormat(sstd::getDefaultOpenGLFormat());
            mmm_OpenGLContex->create();
            mmm_OpenGLContex->makeCurrent(arg->getSurface());
            glewInitialize();
        }
        ~Render() {
            mmm_OpenGLContex->doneCurrent();
            delete mmm_OpenGLContex;
        }
        SSTD_MEMORY_DEFINE(Render)
    };

}/*namespace*/

void sstd::RenderThread::run() try {

    class RenderState {
    public:
        sstd::RenderThread * super = nullptr;
        QImage image;

        RenderState(sstd::RenderThread * a, QImage && arg) :super(a), image(std::move(arg)) {
            image = image.convertToFormat(QImage::Format::Format_RGBA8888);
        }

        void checkImageFormat() {
            if (image.width() < 1) { throw 1; }
            if (image.height() < 1) { throw 2; }
        }

        ~RenderState() { super->renderFinished(image); }

    }varState(this, [this]() { return QImage(this->mmm_ImageFileName); }());
    varState.checkImageFormat();

    /*create a render ... */
    std::unique_ptr<Render> varRender{ sstdNew<Render>(this) };

    FINAL_CLASS_TYPE_ASSIGN(ProgramType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ImageTextureType, sstd::NumberWrapType<GLuint>);

    using OpenGLResourceData = std::tuple<ImageTextureType, ProgramType>;
    class OpenGLResource : public OpenGLResourceData {
    public:
        OpenGLResource() :OpenGLResourceData(0, 0) {}
        ~OpenGLResource() {
            glDeleteProgram(std::get<ProgramType>(*this).value());
            glDeleteTextures(1, std::get<ImageTextureType>(*this).pointer());
        }
    };

    OpenGLResource varOpenGLResource;
    auto & varTexture = std::get<ImageTextureType>(varOpenGLResource);
    auto & varProgram = std::get<ProgramType>(varOpenGLResource);

    /*初始化OpenGL Program*/
    {
        auto varShaderSource = u8R"___(
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding = 0,rgba8ui) uniform uimage2D argImageInputOutput;

void main(void) {

    ivec2 varPos = ivec2( gl_WorkGroupID.xy ) ;
    uvec4 varColor = imageLoad(argImageInputOutput,varPos);
    varColor.r = 255 - varColor.r;
    varColor.g = 255 - varColor.g;
    varColor.b = 255 - varColor.b;
    imageStore(argImageInputOutput,varPos,varColor);

}
/*一个简单的图像取反shader*/
)___"sv;

        auto varShader = glCreateShader(GL_COMPUTE_SHADER);

        {
            GLint varSL = static_cast<GLint>(varShaderSource.size());
            const char *varS[]{ varShaderSource.data() };
            glShaderSource(varShader, 1, varS, &varSL);
            glCompileShader(varShader);
        }

        if constexpr (false) {
            auto printErrorDetail = [](GLuint e) {
                GLint log_length;
                glGetShaderiv(e, GL_INFO_LOG_LENGTH, &log_length);
                log_length += 16;

                /*获得一段内存，并初始化为0*/
                sstd::string infos_;
                infos_.resize(log_length);

                char * info = infos_.data();
                glGetShaderInfoLog(e, log_length, nullptr, info);
                qDebug() << info;
            };
            printErrorDetail(varShader);
        }

        varProgram = glCreateProgram();
        glAttachShader(varProgram, varShader);
        glLinkProgram(varProgram);
        glDeleteShader(varShader);

    }
    const auto & varImage = varState.image;
    const auto varImageHeight = varImage.height();
    const auto varImageWidth = varImage.width();
    /*初始化ImageTexture*/
    {
        glCreateTextures(GL_TEXTURE_2D, 1, varTexture.pointer());
        glTextureStorage2D(varTexture, 1, GL_RGBA8, varImageWidth, varImageHeight);
        glTextureSubImage2D(varTexture,
            0/*level*/,
            0/*x*/, 0/*y*/,
            varImageWidth, varImageHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            varImage.bits());

    }
    /*开启OpenGL调试环境*/
    gl_debug_function_lock();
    /*进行绘制*/
    {
        glUseProgram(varProgram);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glBindImageTexture(0, varTexture, 0, false, 0, GL_READ_ONLY, GL_RGBA8UI);
        glDispatchCompute(varImageWidth, varImageHeight, 1);
    }
    /*等待绘制完成*/
    {
        glFlush();
    }
    /*取回数据*/
    {
        glGetTextureImage(
            varTexture, 
            0,
            GL_RGBA, GL_UNSIGNED_BYTE,
            varImage.byteCount(),
            varState.image.bits());
    }

}
catch (...) {
    qDebug() << "image format error : ";
}

sstd::RenderThread::~RenderThread() {
    mmm_Surface->deleteLater();
}

/** QQuickRenderControl Example  **/













