#include <sstd_glew.hpp>
#include <sstd_load_utf8_file.hpp>
#include "Window.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtCore/qeventloop.h>
#include "ImageView.hpp"
#include <QtCore/qurl.h>
#include <chrono>

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
    QString getPath(const QString & arg) {
        return getLocalFileFullPath(arg).toLocalFile();
    }
}

using namespace std::chrono_literals;

Window::~Window() {
}

Window::Window() {
    mmm_ImageView = sstdNew< ImageView >();
    connect(mmm_ImageView, &QObject::destroyed, this, &QObject::deleteLater);
    connect(this, &QObject::destroyed, mmm_ImageView, &QObject::deleteLater);
    mmm_WatcherImageView = mmm_ImageView;
    mmm_ImageView->show();
    const auto varPos = mmm_ImageView->geometry().bottomRight();
    this->setPosition(varPos);
}

class Window::DrawData : public QObject {
public:
    GLuint mmm_InputImage = 0;
    GLuint mmm_OutputImage = 0;
    GLuint mmm_Program = 0;
    GLint  mmm_ImageWidth = 0;
    GLint  mmm_ImageHeight = 0;
    QImage mmm_OriginImageInput;
    QImage mmm_LastImageInput;
    std::array<GLuint, 2> mmm_InputOutputOrder{ 0,1 };

    std::chrono::high_resolution_clock::time_point mmm_LastDraw;
    QTimer * mmm_Timer = nullptr;
    void deleteThisObject() {
        mmm_Timer->stop();
        glDeleteTextures(1, &mmm_InputImage);
        glDeleteTextures(1, &mmm_OutputImage);
        glDeleteProgram(mmm_Program);
        delete this;
    }

    DrawData() {
        mmm_Timer = sstdNew<QTimer>(this);
    }

    void setProgram() {
        auto varShader = glCreateShader(GL_COMPUTE_SHADER);
        const auto varShaderSource =
            sstd::load_file_remove_utf8(
            sstd::getPath(QStringLiteral("myqml/computeshaderfilterimage/main.cps")));

        {
            GLint varSL = static_cast<GLint>(varShaderSource.size());
            const char *varS[]{ varShaderSource.c_str() };
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

        mmm_Program = glCreateProgram();
        glAttachShader(mmm_Program, varShader);
        glLinkProgram(mmm_Program);
        glDeleteShader(varShader);
    }

    void setTexture() {

        /*获得图片*/
        mmm_OriginImageInput = []() {
            return QImage(sstd::getPath(QStringLiteral("myqml/computeshaderfilterimage/test.png")))
                .convertToFormat(QImage::Format_RGBA8888);
        }();
        const auto & varImage = mmm_OriginImageInput;
        mmm_LastImageInput = varImage;

        /*初始化长宽*/
        mmm_ImageHeight = varImage.height();
        mmm_ImageWidth = varImage.width();

        /*创建Texture*/
        glCreateTextures(GL_TEXTURE_2D, 1, &mmm_InputImage);

        /*分配内存*/
        glTextureStorage2D(mmm_InputImage, 1, GL_RGBA8, mmm_ImageWidth, mmm_ImageHeight);
        /*上传数据*/
        glTextureSubImage2D(mmm_InputImage,
            0/*level*/,
            0/*x*/, 0/*y*/,
            mmm_ImageWidth, mmm_ImageHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            varImage.bits());

        glCreateTextures(GL_TEXTURE_2D, 1, &mmm_OutputImage);
        /*分配内存*/
        glTextureStorage2D(mmm_OutputImage, 1, GL_RGBA8, varImage.width(), varImage.height());

    }

};

void Window::initializeGL() {
    /*************************************************************************/
    //标准开头
    if (mmm_DrawData) {
        return;
    }
    this->makeCurrent();
    glewInitialize();

    gl_debug_function_lock();

    mmm_DrawData = sstdNew<DrawData>();
    connect(context(), &QOpenGLContext::aboutToBeDestroyed,
        mmm_DrawData, &DrawData::deleteThisObject, Qt::DirectConnection);
    connect(mmm_DrawData->mmm_Timer, &QTimer::timeout,
        this, [this]() {update(); });
    mmm_DrawData->mmm_Timer->start(10);
    /*************************************************************************/

    mmm_DrawData->setTexture();
    mmm_DrawData->setProgram();

}

/******************************************/

/***
class SharedDataInAWorkGroup {

};

class InPut {
public:
    std::array<std::uint32_t, 3> gl_NumWorkGroups;
    std::array<std::uint32_t, 3> gl_WorkGroupSize;
    std::array<std::uint32_t, 3> gl_WorkGroupID;
    std::array<std::uint32_t, 3> gl_LocalInvocationID;
    std::array<std::uint32_t, 3> gl_GlobalInvocationID;
    std::uint32_t gl_LocalInvocationIndex;
    SharedDataInAWorkGroup * shread;
};

void dispatchCompute(
    const std::uint32_t num_groups_x,
    const std::uint32_t num_groups_y,
    const std::uint32_t num_groups_z,
    const std::uint32_t local_size_x,
    const std::uint32_t local_size_y,
    const std::uint32_t local_size_z,
    std::function<void(const InPut *)> glslProgramMain) {

    auto fma = [](auto x, auto y, auto z) {return x * y + z; };

    for (std::uint32_t gx = 0; gx < num_groups_x; ++gx) {
        for (std::uint32_t gy = 0; gy < num_groups_y; ++gy) {
            for (std::uint32_t gz = 0; gz < num_groups_z; ++gz) {
                SharedDataInAWorkGroup varSharedData;
                for (std::uint32_t lx = 0; lx < local_size_x; ++lx) {
                    for (std::uint32_t ly = 0; ly < local_size_y; ++ly) {
                        for (std::uint32_t lz = 0; lz < local_size_z; ++lz) {
                            InPut varInput;
                            varInput.gl_NumWorkGroups = { num_groups_x ,num_groups_y ,num_groups_z };
                            varInput.gl_WorkGroupSize = { local_size_x ,local_size_y,local_size_z };
                            varInput.gl_WorkGroupID = { gx,gy,gz };
                            varInput.gl_LocalInvocationID = { lx,ly,lz };
                            varInput.gl_GlobalInvocationID = {
                                fma(local_size_x,gx,lx),
                                fma(local_size_y,gy,ly),
                                fma(local_size_z,gz,lz),
                            };
                            varInput.gl_LocalInvocationIndex = fma(local_size_x,
                                fma(lz, local_size_y, ly), lx);
                            varInput.shread = &varSharedData;
                            glslProgramMain(&varInput);
                        }
                    }
                }
            }
        }
    }
}
***/

/******************************************/

void Window::paintGL() {

    if (nullptr == mmm_DrawData) {
        return;
    }

    if (bool(mmm_WatcherImageView) == false) {
        return;
    }

    glDisable(GL_RASTERIZER_DISCARD);

    {/*间隔超过600ms才执行*/
        auto varCurrentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::abs(varCurrentTime - mmm_DrawData->mmm_LastDraw) > 600ms) {
            mmm_DrawData->mmm_LastDraw = varCurrentTime;
        } else {
            return;
        }
    }

    /*开启OpenGL调试环境*/
    gl_debug_function_lock();

    const auto varStartWaitTime = std::chrono::high_resolution_clock::now();
    auto varEndWaitTime = std::chrono::high_resolution_clock::now();

    if constexpr (false) {
        const auto & varImage = mmm_DrawData->mmm_LastImageInput;
        /*上传数据*/
        glTextureSubImage2D(mmm_DrawData->mmm_InputImage,
            0/*level*/,
            0/*x*/, 0/*y*/,
            mmm_DrawData->mmm_ImageWidth, mmm_DrawData->mmm_ImageHeight,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            varImage.bits());
    }

    glUseProgram(mmm_DrawData->mmm_Program);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glBindImageTexture(mmm_DrawData->mmm_InputOutputOrder[0], mmm_DrawData->mmm_InputImage, 0, false, 0, GL_READ_ONLY, GL_RGBA8UI);
    glBindImageTexture(mmm_DrawData->mmm_InputOutputOrder[1], mmm_DrawData->mmm_OutputImage, 0, false, 0, GL_WRITE_ONLY, GL_RGBA8UI);
    glDispatchCompute(mmm_DrawData->mmm_ImageWidth, mmm_DrawData->mmm_ImageHeight, 1);

    /*等待显卡完成*/
    if constexpr (true) {
        auto varSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glClientWaitSync(varSync, GL_SYNC_FLUSH_COMMANDS_BIT, 100);
        GLint varResult[]{
            GL_UNSIGNALED ,
            GL_UNSIGNALED ,
            GL_UNSIGNALED ,
            GL_UNSIGNALED };
        GLsizei varResultLength = 0;
        do {
            if constexpr (true) {
                std::this_thread::sleep_for(100ns);
                glGetSynciv(varSync, GL_SYNC_STATUS, 4, &varResultLength, varResult);
            } else {
                varResult[0] = glClientWaitSync(varSync, GL_SYNC_FLUSH_COMMANDS_BIT, 100);
            }
            varEndWaitTime = std::chrono::high_resolution_clock::now();
        } while ((varResult[0] != GL_SIGNALED) &&
            (std::chrono::abs(varEndWaitTime - varStartWaitTime) < 1s));
        glDeleteSync(varSync);
    } else {
        glFinish();
    }

    {/*从显卡获得数据*/
        auto & varImageOutput = mmm_DrawData->mmm_LastImageInput;
        glGetTextureImage(
            mmm_DrawData->mmm_InputOutputOrder[0] ?
            mmm_DrawData->mmm_InputImage :
            mmm_DrawData->mmm_OutputImage, 0,
            GL_RGBA, GL_UNSIGNED_BYTE,
            varImageOutput.byteCount(),
            varImageOutput.bits());

        varEndWaitTime = std::chrono::high_resolution_clock::now();
        qDebug() << "draw time less than : " << std::chrono::duration_cast<
            std::chrono::duration<double, std::milli>>(std::chrono::abs(
            varEndWaitTime - varStartWaitTime)).count() << "ms";

        mmm_ImageView->showImage(mmm_DrawData->mmm_OriginImageInput, varImageOutput);
    }
    /*交换输入输出*/
    std::swap(mmm_DrawData->mmm_InputOutputOrder[0], mmm_DrawData->mmm_InputOutputOrder[1]);
}

void Window::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

bool Window::event(QEvent *argEvent) {

    if (argEvent->type() == QEvent::Close) {
        this->deleteLater();
        argEvent->accept();
        return true;
    }

    return Super::event(argEvent);
}

/***
https://blog.csdn.net/z444_579/article/details/54138504
https://www.cnblogs.com/vertexshader/articles/3022981.html
***/

/***
如果release比debug慢，可能是开启了垂直同步
***/



