/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include "cuberenderer.h"
#include <QOpenGLContext>
//#include <QOpenGLFunctions>
//#include <QOpenGLShaderProgram>
//#include <QOpenGLVertexArrayObject>
//#include <QOpenGLBuffer>
//#include <QOpenGLVertexArrayObject>
#include <QOffscreenSurface>
#include <QWindow>
#include <array>

class WindowRenderPack {
public:
    GLuint targetVAO{ 0 };
    GLuint targetVAOBuffer{ 0 };
    GLuint targetVAOIndexBuffer{ 0 };
    GLuint targetProgram{ 0 };
    int width = 123;
    int height = 123;
};

CubeRenderer::CubeRenderer(QOffscreenSurface *offscreenSurface)
    : m_offscreenSurface(offscreenSurface),
      m_context(nullptr),
      m_program(nullptr),
      m_vbo(nullptr),
      m_vao(nullptr),
      m_matrixLoc(0)
{
    this->test = new WindowRenderPack;
}

CubeRenderer::~CubeRenderer()
{
    // Use a temporary offscreen surface to do the cleanup.
    // There may not be a native window surface available anymore at this stage.
    m_context->makeCurrent(m_offscreenSurface);

    delete m_program;
    delete m_vbo;
    delete m_vao;

    m_context->doneCurrent();
    delete m_context;
}

void CubeRenderer::init(QWindow *w, QOpenGLContext *share)
{
    m_context = new QOpenGLContext;
    m_context->setShareContext(share);
    m_context->setFormat(w->requestedFormat());
    m_context->create();
    if (!m_context->makeCurrent(w))
        return;

    glewInitialize();

    //QOpenGLFunctions *f = m_context->functions();
    glClearColor(0.0f, 0.1f, 0.25f, 1.0f);
    glViewport(0, 0, w->width() * w->devicePixelRatio(), w->height() * w->devicePixelRatio());

     
}

void CubeRenderer::resize(int w, int h){
    test->width = w;
    test->height = h;
}

void CubeRenderer::setupVertexAttribs()
{
  
}



inline static GLuint buildVFShader(std::string_view varVShaderSource, std::string_view varFShaderSource) {

    class ShaderFree {
    public:
        std::array<GLuint, 2> data;
        inline ShaderFree() {
            data[0] = 0;
            data[1] = 0;
        }
        inline ~ShaderFree() {
            glDeleteShader(data[1]);
            glDeleteShader(data[0]);
        }
    } varShaders{};

    GLuint * varShader = &(varShaders.data[0]);
    varShader[0] = glCreateShader(GL_VERTEX_SHADER);
    varShader[1] = glCreateShader(GL_FRAGMENT_SHADER);

    {
        const GLchar * sources[] = { varVShaderSource.data(),varFShaderSource.data() };
        GLint lengths[] = {
            (GLint)(varVShaderSource.size()),
            (GLint)(varFShaderSource.size())
        };

        glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
        glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

        glCompileShader(varShader[0]);
        glCompileShader(varShader[1]);

    }

    GLuint varProgram = glCreateProgram();
    glAttachShader(varProgram, varShader[0]);
    glAttachShader(varProgram, varShader[1]);
    glLinkProgram(varProgram);

    if constexpr (true) {

        auto printProgramInfo = [](GLuint e) {
            /*获得错误大小*/
            GLint log_length = 0;
            glGetProgramiv(e, GL_INFO_LOG_LENGTH, &log_length);
            log_length += 16;

            /*获得一段内存，并初始化为0*/
            sstd::string infos_;
            infos_.resize(log_length);

            /*获得错误并输出*/
            char * info = infos_.data();
            glGetProgramInfoLog(e, 1024, nullptr, info);
            qDebug() << info;
        };

        printProgramInfo(varProgram);
    }

    return varProgram;

}

void initProgram(WindowRenderPack * varPack )  {
    
    if (varPack->targetProgram) {
        return;
    }

    varPack->targetProgram = buildVFShader(
        u8R"(
/*简单顶点着色器，用于渲染一个图片*/
#version 450

layout( location = 0 ) in vec4 argPosition  ;
layout( location = 1 ) in vec4 argTexturePos;
out vec4 ioTexturePos ;

void main(){
    ioTexturePos = argTexturePos ;
    gl_Position = argPosition    ;
}

)"sv,
u8R"(

/*简单片段着色器,用于渲染一个图像*/
#version 450

in  vec4 ioTexturePos                           ;
out vec4 outColor                               ;
layout (binding=1) uniform sampler2D argTexture ;

void main(){
    outColor = texture2D( argTexture , ioTexturePos.xy ) ;
    //outColor = vec4(1,0,1,1) ;
}

)"sv);

}

void initVAO(WindowRenderPack * varPack)  {
    
    if (varPack->targetVAO) {
        return;
    }

    glCreateVertexArrays(1, &(varPack->targetVAO));
    glBindVertexArray(varPack->targetVAO);
    glCreateBuffers(1, &(varPack->targetVAOBuffer));
    glCreateBuffers(1, &(varPack->targetVAOIndexBuffer));
    class DataRow {
    public:
        GLfloat x, y, z, w;
        GLfloat s, t, p, q;
    };
    constexpr const static std::array<DataRow, 4> varVAOData{ DataRow {-1,1,0,1,/**/0,1,0,1},
    DataRow {-1,-1,0,1,/**/0,0,0,1},
    DataRow {1,-1,0,1,/**/1,0,0,1},
    DataRow {1,1,0,1,/**/1,1,0,1}
    };
    constexpr const static std::array<std::uint16_t, 6> varVAOIndex{
        3,2,1,
        3,1,0
    };
    glNamedBufferData(varPack->targetVAOBuffer, sizeof(varVAOData), varVAOData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexArrayVertexBuffer(varPack->targetVAO, 0, varPack->targetVAOBuffer, 0, sizeof(DataRow));
    glVertexArrayAttribFormat(varPack->targetVAO, 0, 4, GL_FLOAT, false, 0);
    glVertexArrayAttribBinding(varPack->targetVAO, 0, 0);

    glEnableVertexAttribArray(1);
    glVertexArrayVertexBuffer(varPack->targetVAO, 1, varPack->targetVAOBuffer, (sizeof(DataRow) >> 1), sizeof(DataRow));
    glVertexArrayAttribFormat(varPack->targetVAO, 1, 4, GL_FLOAT, false, 0);
    glVertexArrayAttribBinding(varPack->targetVAO, 1, 1);

    glNamedBufferData(varPack->targetVAOIndexBuffer, sizeof(varVAOIndex), varVAOIndex.data(), GL_STATIC_DRAW);
    glVertexArrayElementBuffer(varPack->targetVAO, varPack->targetVAOIndexBuffer);
}

void CubeRenderer::render(QWindow *w, QOpenGLContext *share, uint texture)
{
    if (!m_context)
        init(w, share);

    if (!m_context->makeCurrent(w))
        return;

    auto varPack = test;

    initProgram(varPack);
    initVAO(varPack);

    glViewport(0,0,varPack->width,varPack->height);

    glUseProgram(varPack->targetProgram);
    glBindVertexArray(varPack->targetVAO);
    glActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindTextureUnit(1, texture);
    //qDebug() << "texture : " << varPack->sourceFrameBufferObject->texture();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    glFinish();

    m_context->swapBuffers(w);
}
