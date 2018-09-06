#include <sstd_glew.hpp>
#include "Window.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <chrono>

using namespace std::chrono_literals;

Window::Window() {}

class Window::DrawData : public QObject {
public:
    GLuint $m$InputBuffer = 0;
    GLuint $m$OutputBuffer = 0;
    GLuint $m$Program = 0;
    std::chrono::high_resolution_clock::time_point $m$LastDraw;
    QTimer * $m$Timer = nullptr;
    void deleteThisObject() {
        $m$Timer->stop();
        glDeleteBuffers(1, &$m$InputBuffer);
        glDeleteBuffers(1, &$m$OutputBuffer);
        glDeleteProgram($m$Program);
        delete this;
    }

    DrawData() {
        $m$Timer = sstdNew<QTimer>(this);
    }

};

void Window::initializeGL() {
    /*************************************************************************/
    //标准开头
    if ($m$DrawData) { return; }
    this->makeCurrent();
    glewInitialize();

    enable_gl_debug_function();

    $m$DrawData = sstdNew<DrawData>();
    connect(context(), &QOpenGLContext::aboutToBeDestroyed,
        $m$DrawData, &DrawData::deleteThisObject, Qt::DirectConnection);
    connect($m$DrawData->$m$Timer, &QTimer::timeout,
        this, [this]() {update(); });
    $m$DrawData->$m$Timer->start(10);
    /*************************************************************************/
    const static constexpr GLfloat varInitData[]{ 1,2,3,4 };
    //在显卡上创建输入缓冲区
    glCreateBuffers(1, &($m$DrawData->$m$InputBuffer));
    glNamedBufferStorage($m$DrawData->$m$InputBuffer,
        sizeof(varInitData),
        varInitData,
        GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    //在显卡上创建输出缓冲区
    glCreateBuffers(1, &($m$DrawData->$m$OutputBuffer));
    glNamedBufferStorage($m$DrawData->$m$OutputBuffer,
        sizeof(varInitData),
        varInitData,
        GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    //编译连接glsl源码，生成程序
    const GLchar * varShaderSource[] = { u8R"___(
/*为了照顾数学不好的孩子，这里只让显卡将输入加一*/
#version 450
layout (local_size_x = 1 , local_size_y = 1 , local_size_z = 1) in;

layout( binding = 0, std430 ) readonly buffer InputData { vec4 inputData[] ; };
layout( binding = 1, std430 ) writeonly buffer OutputData{ vec4 outputData[]; };

void main(void){
    outputData[ gl_LocalInvocationID.x ] = inputData[gl_LocalInvocationID.x] + vec4(1) ; 
}

)___" };

    auto varShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(varShader, 1, varShaderSource, nullptr);
    glCompileShader(varShader);
    $m$DrawData->$m$Program = glCreateProgram();
    glAttachShader($m$DrawData->$m$Program, varShader);
    glLinkProgram($m$DrawData->$m$Program);

    glDeleteShader(varShader);

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
    enable_gl_debug_function();

    /*为了照顾数学不好的孩子，这里只让显卡做2以内的加法*/
    GLfloat varInput[4];
    GLfloat varOutput[4];
    
    varInput[0] = std::rand() & 1;
    varInput[1] = std::rand() & 1;
    varInput[2] = std::rand() & 1;
    varInput[3] = std::rand() & 1;

    {/*向显卡上传数据*/
        const auto varInputGL = static_cast<GLfloat*>(
            glMapNamedBuffer($m$DrawData->$m$InputBuffer, GL_WRITE_ONLY));
        std::memcpy(varInputGL, varInput, sizeof(varInput));
        glUnmapNamedBuffer($m$DrawData->$m$InputBuffer);
    }

    glUseProgram($m$DrawData->$m$Program);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, $m$DrawData->$m$InputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, $m$DrawData->$m$OutputBuffer);
    glDispatchCompute(1, 1, 1);

    /*等待显卡完成*/
    glFinish();

    {/*从显卡获得数据*/
        const auto varOutputGL = static_cast<GLfloat*>(
            glMapNamedBuffer($m$DrawData->$m$OutputBuffer, GL_READ_ONLY));
        std::memcpy(varOutput, varOutputGL, sizeof(varOutput));
        glUnmapNamedBuffer($m$DrawData->$m$OutputBuffer);
    }

    qDebug()
        << endl
        << "----------------------------------------"
        << endl
        << "source   :"
        << varInput[0] << QChar(',')
        << varInput[1] << QChar(',')
        << varInput[2] << QChar(',')
        << varInput[3] << endl
        << "add 1 is :" << endl
        << "          "
        << varOutput[0] << QChar(',')
        << varOutput[1] << QChar(',')
        << varOutput[2] << QChar(',')
        << varOutput[3];

}

void Window::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

/***
https://blog.csdn.net/z444_579/article/details/54138504
***/




