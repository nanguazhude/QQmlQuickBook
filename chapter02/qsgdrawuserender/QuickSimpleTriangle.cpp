/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include "QuickSimpleTriangle.hpp"

namespace sstd {

    QSGRenderNode::RenderingFlags QuickSimpleTriangleNode::flags() const {

        return QSGRenderNode::BoundedRectRendering      |
            QSGRenderNode::DepthAwareRendering          |
            QSGRenderNode::OpaqueRendering              ;

    }

    /*
        glDepthMask(false)
        glDisable(GL_DEPTH_TEST)
        glStencilFunc(GL_EQUAL, state.stencilValue, 0xff) depending on clip
        glScissor(state.scissorRect.x(), state.scissorRect.y(), state.scissorRect.width(), state.scissorRect.height()) depending on clip
        glEnable(GL_BLEND)
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)
        glDisable(GL_CULL_FACE)
    */
    QSGRenderNode::StateFlags QuickSimpleTriangleNode::changedStates() const {
        return nullptr;
    }

    QRectF QuickSimpleTriangleNode::rect() const {
        return Super::rect();
    }

    void QuickSimpleTriangleNode::render(const QSGRenderNode::RenderState *state) {
        
        if ( this->isInitialized()==false ) {
            this->initializeOpenGLFunctions();
        }

        const auto varVP = u8R"(/*顶点着色器*/
#version 450

)"sv;
        const auto varFP = u8R"(/*片段着色器*/
#version 450

)"sv;

        if (mmm_Program == 0) { ppp_InitProgram(varVP,varFP); }
        if (mmm_Buffer == 0) { ppp_InitBuffer(); }
        if (mmm_VAO == 0) { ppp_InitVAO(); }
        


    }

    void QuickSimpleTriangleNode::ppp_InitProgram(std::string_view v, std::string_view f) {

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

        GLint lengths[] = {
                           (GLint)(v.size()),
                           (GLint)(f.size())
        };
        const GLchar * sources[] = { v.data(),f.data() };

        glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
        glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

        glCompileShader(varShader[0]);
        glCompileShader(varShader[1]);

        mmm_Program = glCreateProgram();
        glAttachShader(mmm_Program, varShader[0]);
        glAttachShader(mmm_Program, varShader[1]);
        glLinkProgram(mmm_Program);

    }

    class RowData {
    public:
        /*x,y,z,w,r,g,b,a*/
        std::array<GLfloat, 8> data;
    };
    void QuickSimpleTriangleNode::ppp_InitBuffer() {

        RowData varDrawData[3];

        glCreateBuffers(1, &mmm_Buffer);
        glNamedBufferData(mmm_Buffer,
            sizeof(RowData),
            varDrawData[0].data.data(),
            GL_STATIC_DRAW);
        
    }

    void QuickSimpleTriangleNode::ppp_InitVAO() {

        glCreateVertexArrays(1, &mmm_VAO);
        glBindVertexArray(mmm_VAO);

        glEnableVertexAttribArray(0);
        glVertexArrayVertexBuffer(mmm_VAO, 0, mmm_Buffer, 0, sizeof(RowData));
        glVertexArrayAttribFormat(mmm_VAO, 0, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(mmm_VAO, 0, 0);

        glEnableVertexAttribArray(1);
        glVertexArrayVertexBuffer(mmm_VAO, 1, mmm_Buffer, (sizeof(RowData) >> 1), sizeof(RowData));
        glVertexArrayAttribFormat(mmm_VAO, 1, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(mmm_VAO, 1, 1);

    }

    void QuickSimpleTriangleNode::ppp_Clean() {
        if (false == this->isInitialized()) { return; }
        glDeleteBuffers(1, &mmm_Buffer);
        glDeleteProgram(mmm_Program);
        glDeleteVertexArrays(1, &mmm_VAO);
        mmm_Buffer = 0;
        mmm_Program = 0;
        mmm_VAO = 0;
    }

    QuickSimpleTriangleNode::~QuickSimpleTriangleNode() {
        ppp_Clean();
    }

    void QuickSimpleTriangleNode::releaseResources() {
        ppp_Clean();
    }

    static inline void registerThis() {
        qmlRegisterType<QuickSimpleTriangle>("myqml.qsgdrawuserender", 1, 0, "QuickSimpleTriangle");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*namespace sstd*/





















