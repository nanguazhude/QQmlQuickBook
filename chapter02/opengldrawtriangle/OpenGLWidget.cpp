#include "OpenGLWidget.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>

#include <string_view>
using namespace std::string_view_literals;

namespace {
    class GLProgram {
        class Data {
        public:
            sstd::OpenGLFunctions * const glF = nullptr;
            GLuint _m_data = 0;
            ~Data() {
                if (glF == nullptr) { return; }
                glF->glDeleteProgram(_m_data);
            }
            Data(sstd::OpenGLFunctions *a) :glF(a) {}
        private:
            SSTD_MEMORY_DEFINE(Data)
        };
        std::shared_ptr<Data> _m_data;
        GLProgram(sstd::OpenGLFunctions * f) :
            _m_data(sstd::make_shared<Data>(f)) {}
    public:
        GLProgram() = default;
        GLProgram(const GLProgram&) = default;
        GLProgram(GLProgram&&) = default;
        GLProgram&operator=(const GLProgram&) = default;
        GLProgram&operator=(GLProgram&&) = default;

        GLProgram(sstd::OpenGLFunctions * f, GLuint p) :GLProgram(f) { _m_data->_m_data = p; }
        GLuint operator*() const { if (_m_data)return _m_data->_m_data; return 0; }
        explicit operator bool() const { return bool(_m_data) && (_m_data->_m_data); }
        operator GLuint() const { return operator*(); }

    private:
        SSTD_MEMORY_DEFINE(GLProgram)
    };

    class GLNamedVertexArrayObject {
        class Data {
        public:
            sstd::OpenGLFunctions * const glF = nullptr;
            GLuint _m_data = 0;
            ~Data() {
                if (glF == nullptr) { return; }
                glF->glDeleteVertexArrays(1, &_m_data);
            }
            Data(sstd::OpenGLFunctions *a) :glF(a) {}
        private:
            SSTD_MEMORY_DEFINE(Data)
        };
        std::shared_ptr<Data> _m_data;
        GLNamedVertexArrayObject(sstd::OpenGLFunctions * f) :_m_data(sstd::make_shared<Data>(f)) {}
    public:
        GLNamedVertexArrayObject() = default;
        GLNamedVertexArrayObject(const GLNamedVertexArrayObject&) = default;
        GLNamedVertexArrayObject(GLNamedVertexArrayObject&&) = default;
        GLNamedVertexArrayObject&operator=(const GLNamedVertexArrayObject&) = default;
        GLNamedVertexArrayObject&operator=(GLNamedVertexArrayObject&&) = default;

        GLNamedVertexArrayObject(sstd::OpenGLFunctions * f, GLuint p) :
            GLNamedVertexArrayObject(f) { _m_data->_m_data = p; }
        GLuint operator*() const { if (_m_data)return _m_data->_m_data; return 0; }
        explicit operator bool() const { return bool(_m_data) && (_m_data->_m_data); }
        operator GLuint() const { return operator*(); }
    private:
        SSTD_MEMORY_DEFINE(GLNamedVertexArrayObject)
    };

    inline static GLNamedVertexArrayObject getNamedVertexArrayObject(sstd::OpenGLFunctions * argGLF) {
        GLuint vao = 0;
        argGLF->glCreateVertexArrays(1, &vao);
        return { argGLF,vao };
    }

    inline static GLProgram getProgram(sstd::OpenGLFunctions * argGLF) try {
        const QDir varAppDir{ qApp->applicationDirPath() };
        sstd::OpenGLFunctions * glF = argGLF;
        /*顶点着色器*/
        static auto vertex_shader_source = sstd::load_file_remove_utf8(
            varAppDir.absoluteFilePath(QStringLiteral("myqml/opengldrawtriangle/vertex.vert")));
        /*片段着色器*/
        static auto fragment_shader_source = sstd::load_file_remove_utf8(
            varAppDir.absoluteFilePath(QStringLiteral("myqml/opengldrawtriangle/fragment.frag")));

        /*错误输出函数*/
        auto printProgramInfo = [glF](GLuint e) {
            /*获得错误大小*/
            GLint log_length = 0;
            glF->glGetProgramiv(e, GL_INFO_LOG_LENGTH, &log_length);
            log_length += 16;

            /*获得一段内存，并初始化为0*/
            std::basic_string<char,
                std::char_traits<char>,
                sstd::allocator<char>> infos_;
            infos_.resize(log_length);

            /*获得错误并输出*/
            char * info = infos_.data();
            glF->glGetProgramInfoLog(e, 1024, nullptr, info);
            qDebug() << info;
        };

        auto printError = [](const auto & a) {
            qDebug() << a;
        };

        auto printErrorDetail = [glF](GLuint e) {
            GLint log_length;
            glF->glGetShaderiv(e, GL_INFO_LOG_LENGTH, &log_length);
            log_length += 16;

            /*获得一段内存，并初始化为0*/
            std::basic_string<char,
                std::char_traits<char>,
                sstd::allocator<char>> infos_;
            infos_.resize(log_length);

            char * info = infos_.data();
            glF->glGetShaderInfoLog(e, log_length, nullptr, info);
            qDebug() << info;
        };

        class ShaderFree {
            sstd::OpenGLFunctions * glF;
        public:
            ShaderFree(sstd::OpenGLFunctions * a) : glF(a) {}
            std::array<GLuint, 2> data;
            ShaderFree() {
                data[0] = 0;
                data[1] = 0;
            }
            ~ShaderFree() {
                glF->glDeleteShader(data[1]);
                glF->glDeleteShader(data[0]);
            }
        } varShaders{ glF };

        GLuint * varShader = &(varShaders.data[0]);

        {//1
            varShader[0] = glF->glCreateShader(GL_VERTEX_SHADER);
            if (0 == (varShader[0])) {
                printError("GL_VERTEX_SHADER not surported!");
                throw 1;
            }

            varShader[1] = glF->glCreateShader(GL_FRAGMENT_SHADER);
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

            glF->glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
            glF->glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

            glF->glCompileShader(varShader[0]);
            glF->glCompileShader(varShader[1]);

        }//2

        {//3
            GLint varTestVal;
            glF->glGetShaderiv(varShader[0], GL_COMPILE_STATUS, &varTestVal);
            if (varTestVal == GL_FALSE) {
                printErrorDetail(varShader[0]);
                throw 1;
            }
            glF->glGetShaderiv(varShader[1], GL_COMPILE_STATUS, &varTestVal);
            if (varTestVal == GL_FALSE) {
                printErrorDetail(varShader[1]);
                throw 1;
            }
        }//3

        GLuint varProgram = glF->glCreateProgram();
        if (varProgram == 0) {
            printError("can not create program!");
            throw 1;
        }

        glF->glAttachShader(varProgram, varShader[0]);
        glF->glAttachShader(varProgram, varShader[1]);
        glF->glLinkProgram(varProgram);

        {
            GLint testVal;
            glF->glGetProgramiv(varProgram, GL_LINK_STATUS, &testVal);
            if (testVal == GL_FALSE)
            {
                printProgramInfo(varProgram);
                glF->glDeleteProgram(varProgram);
                throw 1;
            }
        }

        return { glF,varProgram };
    }
    catch (...) {
        return {};
    }

}/*namespace*/

class OpenGLWidget::DrawData {
public:
    GLProgram _m_program;
    GLNamedVertexArrayObject _m_vao;
    std::array< GLfloat, 4> _m_clear_color;
private:
    SSTD_MEMORY_DEFINE(OpenGLWidget::DrawData)
};

void OpenGLWidget::initializeGL() {

    if (_m_draw_data) { return; }
    
    makeCurrent();

    {
        /*初始化OpenGL设备失败，更换你的硬件*/
        const bool varIsInit = initializeOpenGLFunctions();
        Q_ASSERT_X(varIsInit, __FILE__, "opengl init error");
        _m_draw_data = sstdNew<OpenGLWidget::DrawData>();
    }

    {
        _m_draw_data->_m_program = getProgram(this);
        Q_ASSERT_X(_m_draw_data->_m_program, __FILE__, "unkonw error!");
    }

    {
        _m_draw_data->_m_vao = getNamedVertexArrayObject(this);
        Q_ASSERT_X(_m_draw_data->_m_vao, __FILE__, "unkonw error!");
    }

    auto varDestory = _m_draw_data;
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, context(), [varDestory]() {delete varDestory; },Qt::DirectConnection);

}

void OpenGLWidget::resizeGL(int w, int h) {
    if (nullptr == _m_draw_data) { return; }

    glViewport(0, 0, w, h);
    _m_draw_data->_m_clear_color[0] = (std::rand() & 127) / 255.0f;
    _m_draw_data->_m_clear_color[1] = 0.4f;
    _m_draw_data->_m_clear_color[2] = 0.5f;
    _m_draw_data->_m_clear_color[3] = 1.0f;
}

void OpenGLWidget::paintGL() {
    if (nullptr == _m_draw_data) { return; }

    GLuint varFBOIndex = this->defaultFramebufferObject();
    GLuint varProgram = _m_draw_data->_m_program;
    GLuint varVAO = _m_draw_data->_m_vao;

    glClearNamedFramebufferfv(varFBOIndex,
        GL_COLOR,
        0/*draw buffer*/,
        _m_draw_data->_m_clear_color.data());

    glUseProgram(varProgram);
    glBindVertexArray(varVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

OpenGLWidget::~OpenGLWidget() { 
    _m_draw_data = nullptr;
}

OpenGLWidget::OpenGLWidget(QWidget *parent, Qt::WindowFlags f) : Super(parent, f) {
    this->setMinimumWidth(64);
    this->setMinimumHeight(64);
    this->resize(512, 512);
}

