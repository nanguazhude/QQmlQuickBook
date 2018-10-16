#include <sstd_glew.hpp>
#include "Window.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <chrono>

using namespace std::chrono_literals;

Window::Window() {}

class Window::DrawData : public QObject {
public:
    GLuint mmm_InputBuffer = 0;
    GLuint mmm_OutputBuffer = 0;
    GLuint mmm_Program = 0;
    std::chrono::high_resolution_clock::time_point mmm_LastDraw;
    QTimer * mmm_Timer = nullptr;
    void deleteThisObject() {
        mmm_Timer->stop();
        glDeleteBuffers(1, &mmm_InputBuffer);
        glDeleteBuffers(1, &mmm_OutputBuffer);
        glDeleteProgram(mmm_Program);
        delete this;
    }

    DrawData() {
        mmm_Timer = sstdNew<QTimer>(this);
    }

};

void Window::initializeGL() {
    /*************************************************************************/
    //标准开头
    if (mmm_DrawData) { return; }
    this->makeCurrent();
    glewInitialize();

    /*开启OpengGL调试环境*/
    gl_debug_function_lock();

    mmm_DrawData = sstdNew<DrawData>();
    connect(context(), &QOpenGLContext::aboutToBeDestroyed,
        mmm_DrawData, &DrawData::deleteThisObject, Qt::DirectConnection);
    connect(mmm_DrawData->mmm_Timer, &QTimer::timeout,
        this, [this]() {update(); });
    mmm_DrawData->mmm_Timer->start(10);
    /*************************************************************************/
    const static constexpr GLfloat varInitData[]{ 1,2,3,4 };
    /*在显卡上创建输入缓冲区，提示将数据存在显存里*/
    glCreateBuffers(1, &(mmm_DrawData->mmm_InputBuffer));
    glNamedBufferStorage(mmm_DrawData->mmm_InputBuffer,
        sizeof(varInitData),
        varInitData,
        GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    /*在显卡上创建输出缓冲区，提示将数据存在内存里*/
    glCreateBuffers(1, &(mmm_DrawData->mmm_OutputBuffer));
    glNamedBufferStorage(mmm_DrawData->mmm_OutputBuffer,
        sizeof(varInitData),
        varInitData,
        GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

    /*编译连接glsl源码，生成程序*/
    const GLchar * varShaderSource[] = { u8R"___(
#version 450
layout (local_size_x = 1 , local_size_y = 1 , local_size_z = 1) in;

layout( binding = 0, std430 ) readonly  buffer InputData { vec4 inputData[] ; };
layout( binding = 1, std430 ) writeonly buffer OutputData{ vec4 outputData[]; };

void main(void){
    outputData[ gl_LocalInvocationID.x ] = inputData[gl_LocalInvocationID.x] + vec4(1) ;
}

)___" };

    auto varShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(varShader, 1, varShaderSource, nullptr);
    glCompileShader(varShader);
    mmm_DrawData->mmm_Program = glCreateProgram();
    glAttachShader(mmm_DrawData->mmm_Program, varShader);
    glLinkProgram(mmm_DrawData->mmm_Program);
    glDeleteShader(varShader);

}

void Window::paintGL() {

    if (nullptr == mmm_DrawData) {
        return;
    }

    {/*间隔超过1s才执行*/
        auto varCurrentTime = std::chrono::high_resolution_clock::now();
        if (std::chrono::abs(varCurrentTime - mmm_DrawData->mmm_LastDraw) > 1s) {
            mmm_DrawData->mmm_LastDraw = varCurrentTime;
        }
        else {
            return;
        }
    }

    /*开启OpenGL调试环境*/
    gl_debug_function_lock();

    /*初始化输入数据和输出数据*/
    GLfloat varInput[4];
    GLfloat varOutput[4];

    varInput[0] = std::rand() & 1;
    varInput[1] = std::rand() & 1;
    varInput[2] = std::rand() & 1;
    varInput[3] = std::rand() & 1;

    {/*向显卡上传数据*/
        const auto varInputGL = static_cast<GLfloat*>(
            glMapNamedBuffer(mmm_DrawData->mmm_InputBuffer, GL_WRITE_ONLY));
        std::memcpy(varInputGL, varInput, sizeof(varInput));
        glUnmapNamedBuffer(mmm_DrawData->mmm_InputBuffer);
    }

    glUseProgram(mmm_DrawData->mmm_Program);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mmm_DrawData->mmm_InputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mmm_DrawData->mmm_OutputBuffer);
    glDispatchCompute(1, 1, 1);

    /*等待显卡完成*/
    glFinish();

    {/*从显卡获得数据*/
        const auto varOutputGL = static_cast<GLfloat*>(
            glMapNamedBuffer(mmm_DrawData->mmm_OutputBuffer, GL_READ_ONLY));
        std::memcpy(varOutput, varOutputGL, sizeof(varOutput));
        glUnmapNamedBuffer(mmm_DrawData->mmm_OutputBuffer);
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
https://www.cnblogs.com/vertexshader/articles/3022981.html
***/




