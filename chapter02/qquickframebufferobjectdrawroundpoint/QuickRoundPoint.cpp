/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>

#include <ConstructQSurface.hpp>
#include "QuickRoundPoint.hpp"


namespace sstd {

    QuickRoundPoint::QuickRoundPoint(QQuickItem * p) :Super(p) {
        mmm_DrawData = sstd::make_shared<DrawData>();
        connect(this, &QuickRoundPoint::pointColorChanged, this, &QuickRoundPoint::ppp_ColorChanged, Qt::QueuedConnection);
        connect(this, &QuickRoundPoint::pointSizeChanged, this, &QuickRoundPoint::ppp_PointSizeChanged, Qt::QueuedConnection);
        this->setFlag(QQuickItem::ItemHasContents, true);
    }

    namespace {

        class FBORenderer :
            public QQuickFramebufferObject::Renderer,
            public sstd::OpenGLFunctions {
        public:

            std::shared_ptr<QuickRoundPoint::DrawData> mmm_DrawData;
            FBORenderer(std::shared_ptr<QuickRoundPoint::DrawData> varDrawData)
                :mmm_DrawData(std::move(varDrawData)) {
                constructOpenGL();
            }

            void constructOpenGL() {
                /*init opengl contex*/
                /*init opengl functions*/
                this->initializeOpenGLFunctions();
                /*init opengl program*/
                {
                    class ShaderFree {
                        sstd::OpenGLFunctions * gL;
                    public:
                        std::array<GLuint, 2> data;
                        inline ShaderFree(sstd::OpenGLFunctions * g) :gL(g) {
                            data[0] = 0;
                            data[1] = 0;
                        }
                        inline ~ShaderFree() {
                            gL->glDeleteShader(data[1]);
                            gL->glDeleteShader(data[0]);
                        }
                    } varShaders{ this };

                    GLuint * varShader = &(varShaders.data[0]);
                    varShader[0] = glCreateShader(GL_VERTEX_SHADER);
                    varShader[1] = glCreateShader(GL_FRAGMENT_SHADER);

                    {
                        const std::string_view varSources[] = { u8R"(

#version 450 

layout( location = 0 ) in vec4 vertexCoord ;
layout( location = 1 ) in vec4 vertexColor ;
layout( location = 2 ) uniform float pointSize = 16 ;

out vec4 color;

void main() {
    gl_PointSize = pointSize  ;
    gl_Position = vertexCoord ; 
    color       = vertexColor ;
}

)"sv,u8R"(

#version 450 
in  vec4 color      ;
out vec4 finalColor ;

void main() {

    vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
    float ansA = 1 -  sqrt( dot(p,p) )  ;
    if( ansA > 0.3 ){ 
        ansA = 1;
     } else if( ansA > 0.25){
        ansA = 2*ansA + 0.4 ;
    }else if( ansA > 0.22  ){
       ansA = 6.667*ansA -0.7667 ;
    }
    else if( ansA > 0.2) {
       ansA = 35*ansA - 7 ;
     } 
     else{ ansA = 0; }
    finalColor = vec4( color.r , color.g , color.b , ansA ) ;

}

)"sv };
                        GLint lengths[] = {
                            (GLint)(varSources[0].size()),
                            (GLint)(varSources[1].size())
                        };
                        const GLchar * sources[] = { varSources[0].data(),varSources[1].data() };

                        glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
                        glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

                        glCompileShader(varShader[0]);
                        glCompileShader(varShader[1]);

                    }

                    mmm_Program = glCreateProgram();
                    glAttachShader(mmm_Program, varShader[0]);
                    glAttachShader(mmm_Program, varShader[1]);
                    glLinkProgram(mmm_Program);
                }

                glCreateVertexArrays(1, &mmm_VAO);
                glBindVertexArray(mmm_VAO);

                /*init opengl data*/
                glCreateBuffers(1, &mmm_Buffer);
                glNamedBufferData(mmm_Buffer,
                    sizeof(RowData),
                    mmm_GLDrawData.data.data(),
                    GL_DYNAMIC_DRAW);

                glEnableVertexAttribArray(0);
                glVertexArrayVertexBuffer(mmm_VAO, 0, mmm_Buffer, 0, sizeof(RowData));
                glVertexArrayAttribFormat(mmm_VAO, 0, 4, GL_FLOAT, false, 0);
                glVertexArrayAttribBinding(mmm_VAO, 0, 0);

                glEnableVertexAttribArray(1);
                glVertexArrayVertexBuffer(mmm_VAO, 1, mmm_Buffer, (sizeof(RowData) >> 1), sizeof(RowData));
                glVertexArrayAttribFormat(mmm_VAO, 1, 4, GL_FLOAT, false, 0);
                glVertexArrayAttribBinding(mmm_VAO, 1, 1);

            }

            void destrcutOpenGL() {
                glDeleteProgram(mmm_Program);
                glDeleteVertexArrays(1, &mmm_VAO);
                glDeleteBuffers(1, &mmm_Buffer);
            }

            ~FBORenderer() {
                destrcutOpenGL();
            }

            std::array<GLfloat, 4> mmm_ClearColor{ 0.0f,0.0f,0.0f,0.0f };
            std::array<GLfloat, 1> mmm_ClearDepth{ 1.0f };
            GLuint mmm_Program = 0;
            GLuint mmm_VAO = 0;
            GLuint mmm_Buffer = 0;
            class RowData {
            public:
                std::array<GLfloat, 8> data{ 0,0,0,1, 1,0,0,1 };
            }mmm_GLDrawData;

            void render() override {

                /*保存当前OpenGL状态，并在退出函数后恢复状态*/
                sstd::StateStackBasic varGLState;
                varGLState.push_value(glIsEnabled(GL_DEPTH_TEST),
                    [this](auto && v) {if (v) { glEnable(GL_DEPTH_TEST); } else { glDisable(GL_DEPTH_TEST); }});
                varGLState.push_value(glIsEnabled(GL_BLEND),
                    [this](auto && v) {if (v) { glEnable(GL_BLEND); } else { glDisable(GL_BLEND); }});
                varGLState.push([this]() { GLint v; glGetIntegerv(GL_BLEND_SRC_ALPHA, &v); return v; },
                    [this](auto && v) {glBlendFunc(GL_SRC_ALPHA, v); });

                auto varFrameBuffer = this->framebufferObject();

                const auto varFBOIndex = varFrameBuffer->handle();
                glBindFramebuffer(GL_FRAMEBUFFER, varFrameBuffer->handle());
                glViewport(0, 0, varFrameBuffer->size().width(), varFrameBuffer->size().height());

                /*清空颜色缓冲区和深度缓冲区*/
                glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, mmm_ClearColor.data());
                glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, mmm_ClearDepth.data());

                /*关闭深度测试，开启混合*/
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                /*开启程序控制点大小*/
                glEnable(GL_PROGRAM_POINT_SIZE);
                glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
                glEnable(GL_POINT_SPRITE);

                /*使用程序*/
                glUseProgram(mmm_Program);

                {/*设置点大小*/
                    GLfloat varPointSize = 1;
                    {
                        std::shared_lock varReadLock{ mmm_DrawData->mmm_ReadWriteLock };
                        varPointSize = static_cast<GLfloat>(mmm_DrawData->mmm_PointSize);
                    }
                    glPointSize(varPointSize > 1.0f ? varPointSize : 1.0f);
                    glUniform1f(2, varPointSize > 1.0f ? varPointSize : 1.0f);
                }

                /*更新顶点数据*/
                {/*更新点位置*/
                    mmm_GLDrawData.data[0] = 0;
                    mmm_GLDrawData.data[1] = 0;
                    mmm_GLDrawData.data[2] = 0;
                    mmm_GLDrawData.data[3] = 1;

                    {/*更新颜色*/
                        QColor varColor;
                        {
                            std::shared_lock varReadLock{ mmm_DrawData->mmm_ReadWriteLock };
                            varColor = mmm_DrawData->mmm_PointColor;
                        }
                        int r, g, b, a;
                        varColor.getRgb(&r, &g, &b, &a);
                        constexpr const static auto varK = 1.0f / 255.0f;
                        mmm_GLDrawData.data[0 + 4] = r * varK;
                        mmm_GLDrawData.data[1 + 4] = g * varK;
                        mmm_GLDrawData.data[2 + 4] = b * varK;
                        mmm_GLDrawData.data[3 + 4] = a * varK;
                    }

                    auto varData = static_cast<GLfloat *>(glMapNamedBuffer(mmm_Buffer, GL_WRITE_ONLY));
                    ::memcpy(varData, mmm_GLDrawData.data.data(), sizeof(mmm_GLDrawData));
                    glUnmapNamedBuffer(mmm_Buffer);

                }

                /*使用VAO*/
                glBindVertexArray(mmm_VAO);
                /*绘制数据*/
                glDrawArrays(GL_POINTS, 0, 1);
            }

            QOpenGLFramebufferObject * createFramebufferObject(const QSize &size) override {
                QOpenGLFramebufferObjectFormat format;
                format.setAttachment(QOpenGLFramebufferObject::NoAttachment);
                format.setInternalTextureFormat(GL_RGBA32F);
                format.setSamples(4);
                return sstdNew<QOpenGLFramebufferObject>(size, format);
            }

        public:

        private:
            SSTD_MEMORY_DEFINE(FBORenderer)
        };

    }/*namespace*/

    auto QuickRoundPoint::createRenderer() const ->Renderer * {
        return sstdNew<FBORenderer>(this->mmm_DrawData);
    }

    void QuickRoundPoint::ppp_ColorChanged() {
        this->update();
    }

    void QuickRoundPoint::ppp_PointSizeChanged() {
        ppp_UpdatePointSizeAnsPosition();
    }

    void QuickRoundPoint::ppp_UpdatePointSizeAnsPosition() {
        const auto varPointSize = getPointSize();
        this->setWidth(varPointSize);
        this->setHeight(varPointSize);
    }

    qreal QuickRoundPoint::getPointSize() const {
        std::shared_lock varReadLock{ mmm_DrawData->mmm_ReadWriteLock };
        return mmm_DrawData->mmm_PointSize;
    }

    void QuickRoundPoint::setPointSize(qreal a) {
        {
            std::unique_lock varWriteLock{ mmm_DrawData->mmm_ReadWriteLock };
            if (a < 0)a = 0;
            if (a == mmm_DrawData->mmm_PointSize)
                return;
            mmm_DrawData->mmm_PointSize = a;
        }
        pointSizeChanged();
    }

    QColor QuickRoundPoint::getPointColor() const {
        std::shared_lock varReadLock{ mmm_DrawData->mmm_ReadWriteLock };
        return mmm_DrawData->mmm_PointColor;
    }

    void QuickRoundPoint::setPointColor(const QColor & arg) {
        {
            std::unique_lock varWriteLock{ mmm_DrawData->mmm_ReadWriteLock };
            if (arg == mmm_DrawData->mmm_PointColor)
                return;
            mmm_DrawData->mmm_PointColor = arg;
        }
        pointColorChanged();
    }

    static inline void registerThis() {
        qmlRegisterType<QuickRoundPoint>("myqml.qquickframebufferobjectdrawroundpoint", 1, 0, "QuickRoundPoint");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*****/





























