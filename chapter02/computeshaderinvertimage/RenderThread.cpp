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
    FINAL_CLASS_TYPE_ASSIGN(AtomicCountType, sstd::NumberWrapType<GLuint>);

    using OpenGLResourceData = std::tuple<ImageTextureType, ProgramType, AtomicCountType>;
    class OpenGLResource : public OpenGLResourceData {
    public:
        OpenGLResource() :OpenGLResourceData(0, 0, 0) {}
        ~OpenGLResource() {
            glDeleteProgram(std::get<ProgramType>(*this).value());
            glDeleteTextures(1, std::get<ImageTextureType>(*this).pointer());
            glDeleteBuffers(1, std::get<AtomicCountType>(*this).pointer());
        }
    };

    OpenGLResource varOpenGLResource;
    auto & varTexture = std::get<ImageTextureType>(varOpenGLResource);
    auto & varProgram = std::get<ProgramType>(varOpenGLResource);
    auto & varAtomicCount = std::get<AtomicCountType>(varOpenGLResource);

    /*初始化OpenGL Program*/
    {
        auto varShaderSource = u8R"___(
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding = 0,rgba8ui) uniform uimage2D argImageInputOutput;
layout(binding = 1,offset = 0 ) uniform atomic_uint argRenderCount ;

void main(void) {
    ivec2 varPos = ivec2( gl_WorkGroupID.xy ) ;
    uvec4 varColor = imageLoad(argImageInputOutput,varPos);
    varColor.r = 255 - varColor.r;
    varColor.g = 255 - varColor.g;
    varColor.b = 255 - varColor.b;
    imageStore(argImageInputOutput,varPos,varColor);
    atomicCounterIncrement(argRenderCount);
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
    {
        glCreateBuffers(1, varAtomicCount.pointer());
        {
            const std::array<GLuint, 4> varTmpInitData{ 0,0,0,0 };
            glNamedBufferStorage(varAtomicCount,
                sizeof(varTmpInitData),
                varTmpInitData.data(),
                GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
        }
    }
    /*开启OpenGL调试环境*/
    gl_debug_function_lock();
    /*更新数据*/
    {
        *reinterpret_cast<GLuint *>(glMapNamedBuffer(varAtomicCount, GL_WRITE_ONLY)) = 0;
        glUnmapNamedBuffer(varAtomicCount);
    }
    /*进行绘制*/
    {
        glUseProgram(varProgram);
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
        glBindImageTexture(0, varTexture, 0, false, 0, GL_READ_ONLY, GL_RGBA8UI);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, varAtomicCount);
        glDispatchCompute(varImageWidth, varImageHeight, 1);
    }
    /*等待绘制完成*/
    if constexpr (true) {
        for (;;) {
            auto varCount = reinterpret_cast<volatile GLuint *>(glMapNamedBuffer(varAtomicCount, GL_READ_ONLY));
            const auto varAllPixCount = static_cast<GLuint>(varImageHeight*varImageWidth);
            if ((*varCount) != varAllPixCount) {
                glUnmapNamedBuffer(varAtomicCount);
                qDebug() << "...";
                std::this_thread::sleep_for(10ms);
            }
            else {
                glUnmapNamedBuffer(varAtomicCount);
                break;
            }
        }
    }
    else {
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













