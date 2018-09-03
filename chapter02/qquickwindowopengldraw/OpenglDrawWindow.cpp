#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <sstd_load_utf8_file.hpp>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include "OpenglDrawWindow.hpp"
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickwindow.h>

extern bool glewInitialize();

namespace {

    constexpr const inline auto getArraySize() { return std::uint16_t(64); }

    template<typename _T_>
    class BasicGLIndex {
    protected:
        class Data {
        public:
            GLuint const _m_index;
            Data(GLuint x) :_m_index(x) {}
            ~Data() {
                _T_::_p_destory(&_m_index);
            }
        private:
            SSTD_MEMORY_DEFINE(Data)
        };
        std::shared_ptr<Data> _m_data;
        inline GLuint _p_get_data() const {
            if (_m_data) {
                return _m_data->_m_index;
            }
            return 0;
        }
        inline bool _p_to_bool() const {
            if (_m_data) {
                return true;
            }
            else { return false; }
        }
        BasicGLIndex() {}
        BasicGLIndex(GLuint n) :_m_data(sstd::make_shared<Data>(n)) {
        }
    };

    class GLProgram : private BasicGLIndex<GLProgram> {
    private:
        using Super = BasicGLIndex<GLProgram>;
        template<typename T> friend class BasicGLIndex;
        static inline void _p_destory(const GLuint *a) {
            glDeleteProgram(*a);
        }
    public:
        GLProgram() = default;
        GLProgram&operator=(const GLProgram&) = default;
        GLProgram&operator=(GLProgram &&) = default;
        GLProgram(const GLProgram&) = default;
        GLProgram(GLProgram &&) = default;
        explicit GLProgram(GLuint a) :Super(a) {}
        explicit operator bool() const { return Super::_p_to_bool(); }
        operator GLuint() const { return Super::_p_get_data(); }
    private:
        SSTD_MEMORY_DEFINE(GLProgram)
    };

    class GLBuffer : private BasicGLIndex<GLBuffer> {
    private:
        using Super = BasicGLIndex<GLBuffer>;
        template<typename T> friend class BasicGLIndex;
        static inline void _p_destory(const GLuint *a) {
            glDeleteBuffers(1, a);
        }
    public:
        GLBuffer() = default;
        GLBuffer&operator=(const GLBuffer&) = default;
        GLBuffer&operator=(GLBuffer &&) = default;
        GLBuffer(const GLBuffer&) = default;
        GLBuffer(GLBuffer &&) = default;
        explicit GLBuffer(GLuint a) :Super(a) {}
        explicit operator bool() const { return Super::_p_to_bool(); }
        operator GLuint() const { return Super::_p_get_data(); }
    private:
        SSTD_MEMORY_DEFINE(GLBuffer)
    };

    class GLNamedVertexArrayObject : private BasicGLIndex<GLNamedVertexArrayObject> {
    private:
        using Super = BasicGLIndex<GLNamedVertexArrayObject>;
        template<typename T> friend class BasicGLIndex;
        static inline void _p_destory(const GLuint *a) {
            glDeleteVertexArrays(1, a);
        }
    public:
        GLNamedVertexArrayObject() = default;
        GLNamedVertexArrayObject&operator=(const GLNamedVertexArrayObject&) = default;
        GLNamedVertexArrayObject&operator=(GLNamedVertexArrayObject &&) = default;
        GLNamedVertexArrayObject(const GLNamedVertexArrayObject&) = default;
        GLNamedVertexArrayObject(GLNamedVertexArrayObject &&) = default;
        explicit GLNamedVertexArrayObject(GLuint a) :Super(a) {}
        explicit operator bool() const { return Super::_p_to_bool(); }
        operator GLuint() const { return Super::_p_get_data(); }
    private:
        SSTD_MEMORY_DEFINE(GLNamedVertexArrayObject)
    };

    inline static GLProgram getProgram() try {
        const QDir varAppDir{ qApp->applicationDirPath() };

        /*顶点着色器*/
        static auto vertex_shader_source = sstd::load_file_remove_utf8(
            varAppDir.absoluteFilePath(QStringLiteral("myqml/qquickwindowopengldraw/vertex.vert")));
        /*片段着色器*/
        static auto fragment_shader_source = sstd::load_file_remove_utf8(
            varAppDir.absoluteFilePath(QStringLiteral("myqml/qquickwindowopengldraw/fragment.frag")));

        /*错误输出函数*/
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

        auto printError = [](const auto & a) {
            qDebug() << a;
        };

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

        {//1
            varShader[0] = glCreateShader(GL_VERTEX_SHADER);
            if (0 == (varShader[0])) {
                printError("GL_VERTEX_SHADER not surported!");
                throw 1;
            }

            varShader[1] = glCreateShader(GL_FRAGMENT_SHADER);
            if (0 == (varShader[1])) {
                printError("GL_FRAGMENT_SHADER not surported!");
                throw 1;
            }
        }//1

        {//2
            const GLchar * sources[] = { vertex_shader_source.data(),fragment_shader_source.data() };
            GLint lengths[] = {
                (GLint)(vertex_shader_source.size()),
                (GLint)(fragment_shader_source.size())
            };

            glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
            glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

            glCompileShader(varShader[0]);
            glCompileShader(varShader[1]);

        }//2

        {//3
            GLint varTestVal;
            glGetShaderiv(varShader[0], GL_COMPILE_STATUS, &varTestVal);
            if (varTestVal == GL_FALSE) {
                printErrorDetail(varShader[0]);
                throw 1;
            }
            glGetShaderiv(varShader[1], GL_COMPILE_STATUS, &varTestVal);
            if (varTestVal == GL_FALSE) {
                printErrorDetail(varShader[1]);
                throw 1;
            }
        }//3

        GLuint varProgram = glCreateProgram();
        if (varProgram == 0) {
            printError("can not create program!");
            throw 1;
        }

        glAttachShader(varProgram, varShader[0]);
        glAttachShader(varProgram, varShader[1]);
        glLinkProgram(varProgram);

        {
            GLint testVal;
            glGetProgramiv(varProgram, GL_LINK_STATUS, &testVal);
            if (testVal == GL_FALSE)
            {
                printProgramInfo(varProgram);
                glDeleteProgram(varProgram);
                throw 1;
            }
        }

        return GLProgram{ varProgram };
    }
    catch (...) {
        return {};
    }

    class VertexArrayAndBuffer {
    public:
        GLBuffer $m$InstanceBuffer;
        GLNamedVertexArrayObject $m$VAO;
    };

    inline static VertexArrayAndBuffer getNamedVertexArrayObject() {
        VertexArrayAndBuffer varAns;

        GLuint varVAO = 0;
        glCreateVertexArrays(1, &varVAO);
        varAns.$m$VAO = GLNamedVertexArrayObject{varVAO};

        class RowData {
        public:
            /*
            x位置 ： 0
            y位置 ： 1
            旋转  ： 2
            z数值 ： 3
            */
            std::array<GLdouble, 4> $m$Data;
            RowData() {
                $m$Data[0] = (std::rand() & 255) / 256.0f;
                $m$Data[1] = (std::rand() & 255) / 256.0f;
                $m$Data[2] = (std::rand() & 255) / 256.0f;
                $m$Data[3] = 1;
            }
        };

        RowData varInitData[getArraySize()];
        {
            double varIndex = 1;
            for (auto & varI : varInitData) {
                varInitData->$m$Data[3] = std::fma((0.1 / 64), varIndex++, 0.5);
            }
        }

        GLuint varBuffer[1];

        glCreateBuffers(1, varBuffer);
        varAns.$m$InstanceBuffer = GLBuffer{ varBuffer[0] };

        /*上传数据*/
        glNamedBufferData(varBuffer[0], sizeof(varInitData), varInitData, GL_STATIC_DRAW);
         
        /*绑定index buffer*/
        glVertexArrayElementBuffer(varVAO, varBuffer[1]);
        /*将 point and color buffer 绑定到VAO*/
        glEnableVertexArrayAttrib(varVAO, 0)/*instance*/;
        /*指定如何从Buffer中解出包*/
        glVertexArrayVertexBuffer(varVAO, 0, varBuffer[0], 0, sizeof(RowData));
        /*指定如何从包中获得数据*/
        glVertexArrayAttribFormat(varVAO, 0, 4, GL_DOUBLE, false, 0);

        glVertexArrayAttribBinding(varVAO, 0, 0);

        return std::move(varAns);
    }         

}/*namespace*/

class OpenglDrawWindowItemRender::DrawData {
public:

    GLint $m$Width = 0;
    GLint $m$Height = 0;

    GLProgram $m$Program;
    GLNamedVertexArrayObject $m$VAO;
    GLBuffer $m$VAOBuffer;
    std::array<GLfloat, 4> $m$ClearColor;
    std::array<GLfloat, 1> $m$ClearDepth;

    DrawData() {
        glewInitialize();

        $m$Program = getProgram();
        {
            auto varTmp = getNamedVertexArrayObject();
            $m$VAO = std::move(varTmp.$m$VAO);
            $m$VAOBuffer = std::move(varTmp.$m$InstanceBuffer);
        }

        $m$ClearColor[0] = 0.3f;
        $m$ClearColor[1] = 0.4f;
        $m$ClearColor[2] = 0.5f;
        $m$ClearColor[3] = 1.0f;

        $m$ClearDepth[0] = 1.0f;
    }

    ~DrawData() {}

private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindowItemRender::DrawData)
};

void OpenglDrawWindowItemRender::paintGL() {
    if (_m_window == nullptr) { return; }
    initializeGL();

    sstd::StateStackBasic varGLState;
    varGLState.push_value(glIsEnabled(GL_DEPTH_TEST),
        [](auto && v) {if (v) { glEnable(GL_DEPTH_TEST); } else { glDisable(GL_DEPTH_TEST); }})/*保存OpenGL状态*/;
         
    {
        class Row { 
        public:
            double $m$PosX;
            double $m$PosY;
            double $m$Rot;
            double $m$Z;
        };
        using Type = std::array<Row, getArraySize()> ;

        auto varData = static_cast<Type *>( 
            glMapNamedBuffer(_m_draw_data->$m$VAOBuffer, GL_READ_WRITE) );
        for ( auto & varI : (*varData) ) {/**update x ,y , and rotate **/
            
        }
        glUnmapNamedBuffer(_m_draw_data->$m$VAOBuffer);
    }

    GLuint varFBOIndex = _m_window->renderTargetId();
    glViewport(0, 0, _m_draw_data->$m$Width, _m_draw_data->$m$Height);

    glEnable(GL_DEPTH_TEST);

    glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, _m_draw_data->$m$ClearColor.data() );
    glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, _m_draw_data->$m$ClearColor.data() );

    glUseProgram( _m_draw_data->$m$Program );
    glBindVertexArray(_m_draw_data->$m$VAO );
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, getArraySize());

    glBindVertexArray(0);
    glUseProgram(0);

}

OpenglDrawWindowItem::OpenglDrawWindowItem(QQuickItem *parent) :Super(parent) {
    this->setFlag(QQuickItem::ItemHasContents, true);
    connect(this, &QQuickItem::windowChanged, this, &OpenglDrawWindowItem::handleWindowChanged);
    this->setWidth(10);
    this->setHeight(10);
}

OpenglDrawWindowItemRender::OpenglDrawWindowItemRender() {}

void OpenglDrawWindowItem::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        window->setColor(QColor(100, 100, 100, 255));
        connect(window, &QQuickWindow::beforeSynchronizing, this, &OpenglDrawWindowItem::sync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &OpenglDrawWindowItem::cleanup, Qt::DirectConnection);
        window->setClearBeforeRendering(false);
    }
}

void OpenglDrawWindowItem::sync() {
    if (_m_render == nullptr) {
        _m_render = sstdNew<OpenglDrawWindowItemRender>();
        connect(
            window(), &QQuickWindow::beforeRendering,
            _m_render, &OpenglDrawWindowItemRender::paintGL,
            Qt::DirectConnection);
    }

    _m_render->resizeGL(window()->size() * window()->devicePixelRatio());
    /*******************************************************************/
    /*******************************************************************/
    _m_render->setWindow(window());
}

void OpenglDrawWindowItem::cleanup() {
    if (_m_render) {
        delete _m_render;
        _m_render = nullptr;
    }
}

void OpenglDrawWindowItemRender::initializeGL() {
    if (_m_draw_data) { return; }
    /*******************************************/
    _m_window->openglContext()->makeCurrent(_m_window);
    _m_draw_data = sstdNew<DrawData>();
    /*******************************************/

}

void OpenglDrawWindowItemRender::resizeGL(int w, int h) {
    if (_m_draw_data == nullptr) {
        glClearColor(0, 0, 0, 1);
        return;
    }
    _m_draw_data->$m$Height = h;
    _m_draw_data->$m$Width = w;
}

OpenglDrawWindowItemRender::~OpenglDrawWindowItemRender() {
    delete _m_draw_data;
}

/*****************************************************/
//
/*****************************************************/
