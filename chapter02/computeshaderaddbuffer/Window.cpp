#include <sstd_glew.hpp>
#include <sstd_load_utf8_file.hpp>
#include "Window.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <chrono>

using namespace std::chrono_literals;

Window::Window() {}

class Window::DrawData : public QObject {
public:
    GLuint $m$InputImage = 0;
    GLuint $m$OutputImage = 0;
    GLuint $m$Program = 0;
    GLint $m$ImageWidth = 0;
    GLint $m$ImageHeight = 0;
    QImage $m$ImageInput;

    std::chrono::high_resolution_clock::time_point $m$LastDraw;
    QTimer * $m$Timer = nullptr;
    void deleteThisObject() {
        $m$Timer->stop();
        glDeleteTextures(1, &$m$InputImage);
        glDeleteTextures(1, &$m$OutputImage);
        glDeleteProgram($m$Program);
        delete this;
    }

    DrawData() {
        $m$Timer = sstdNew<QTimer>(this);
    }

    void setProgram() {
        auto varShader = glCreateShader(GL_COMPUTE_SHADER);
        const auto varShaderSource =
            sstd::load_file_remove_utf8(QStringLiteral("myqml/computeshaderfilterimage/main.cps"));
        {
            GLint varSL = static_cast<GLint>(varShaderSource.size());
            const char *varS[]{ varShaderSource.c_str() };
            glShaderSource(varShader, 1, varS, &varSL);
        }
        glCompileShader(varShader);
        $m$Program = glCreateProgram();
        glAttachShader($m$Program, varShader);
        glLinkProgram($m$Program);

        glDeleteShader(varShader);
    }

    void setTexture() {

        glCreateTextures(GL_TEXTURE_2D, 1, &$m$InputImage);
        glCreateTextures(GL_TEXTURE_2D, 1, &$m$OutputImage);

        const QImage varImage = []() {
            return QImage(QStringLiteral("myqml/computeshaderfilterimage/test.png"))
                .convertToFormat(QImage::Format_RGBA8888);
        }();

        /*分配内存*/
        glTextureStorage2D($m$InputImage, 0, GL_RGBA8UI, varImage.width(), varImage.height());
        /*上传数据*/
        glTextureSubImage2D($m$InputImage, 0,
            0, 0,
            varImage.width(), varImage.height(),
            GL_BGRA,
            GL_UNSIGNED_BYTE,
            varImage.bits());

        /*分配内存*/
        glTextureStorage2D($m$OutputImage, 0, GL_RGBA8UI, varImage.width(), varImage.height());

        /*初始化长宽*/
        $m$ImageHeight = varImage.height();
        $m$ImageWidth = varImage.width();
        $m$ImageInput = varImage;

    }

};

void Window::initializeGL() {
    /*************************************************************************/
    //标准开头
    if ($m$DrawData) { return; }
    this->makeCurrent();
    glewInitialize();

    gl_debug_function_lock();

    $m$DrawData = sstdNew<DrawData>();
    connect(context(), &QOpenGLContext::aboutToBeDestroyed,
        $m$DrawData, &DrawData::deleteThisObject, Qt::DirectConnection);
    connect($m$DrawData->$m$Timer, &QTimer::timeout,
        this, [this]() {update(); });
    $m$DrawData->$m$Timer->start(10);
    /*************************************************************************/
   
    $m$DrawData->setTexture();
    $m$DrawData->setProgram();
    

}

void Window::paintGL() {

    if (nullptr == $m$DrawData) {
        return;
    }

    {/*间隔超过1s才执行*/
        auto varCurrentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::abs(varCurrentTime - $m$DrawData->$m$LastDraw) > 1s) {
            $m$DrawData->$m$LastDraw = varCurrentTime;
        }
        else {
            return;
        }
    }

    /*开启OpenGL调试环境*/
    gl_debug_function_lock();

    /*为了照顾数学不好的孩子，这里只让显卡做2以内的加法*/
    GLfloat varInput[4];
    GLfloat varOutput[4];

    varInput[0] = std::rand() & 1;
    varInput[1] = std::rand() & 1;
    varInput[2] = std::rand() & 1;
    varInput[3] = std::rand() & 1;
    
    glUseProgram($m$DrawData->$m$Program);
    
    glDispatchCompute(1, 1, 1);

    /*等待显卡完成*/
    glFinish();

    {/*从显卡获得数据*/
         
    }

    

}

void Window::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

/***
https://blog.csdn.net/z444_579/article/details/54138504
https://www.cnblogs.com/vertexshader/articles/3022981.html
***/




