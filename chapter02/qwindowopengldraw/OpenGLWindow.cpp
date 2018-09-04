#include <sstd_glew.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <sstd_load_utf8_file.hpp>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include "OpenGLWindow.hpp"

namespace {
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
            varAppDir.absoluteFilePath(QStringLiteral("myqml/qwindowopengldraw/vertex.vert")));
        /*片段着色器*/
        static auto fragment_shader_source = sstd::load_file_remove_utf8(
            varAppDir.absoluteFilePath(QStringLiteral("myqml/qwindowopengldraw/fragment.frag")));

        /*错误输出函数*/
        auto printProgramInfo = [](GLuint e) {
            /*获得错误大小*/
            GLint log_length = 0;
            glGetProgramiv(e, GL_INFO_LOG_LENGTH, &log_length);
            log_length += 16;

            /*获得一段内存，并初始化为0*/
            std::basic_string<char,
                std::char_traits<char>,
                sstd::allocator<char>> infos_;
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
            std::basic_string<char,
                std::char_traits<char>,
                sstd::allocator<char>> infos_;
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
        } varShaders{  };

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

    inline constexpr double operator""_spos(const long double a) {
        return a / 2.6;
    }

    class VertexArrayAndBuffer {
    public:
        GLNamedVertexArrayObject _m_vao;
        GLBuffer _m_point_color_buffer;
        GLBuffer _m_index_buffer;
    };

    inline static VertexArrayAndBuffer getNamedVertexArrayObject() {

        class PointAndColor {
        public:
            GLdouble x; GLdouble y;
            GLdouble z; GLdouble w;
            GLdouble r; GLdouble g;
            GLdouble b; GLdouble a;
        };

        constexpr const static PointAndColor varRawData[8]{
            /*0*/{-1.0_spos,-1.0_spos,+1.0_spos,1.0,/*0*/0.5,0.8,0.1,1.0},
            /*1*/{-1.0_spos,+1.0_spos,+1.0_spos,1.0,/*1*/0.6,0.7,0.2,1.0},
            /*2*/{+1.0_spos,+1.0_spos,+1.0_spos,1.0,/*2*/0.7,0.6,0.3,1.0},
            /*3*/{+1.0_spos,-1.0_spos,+1.0_spos,1.0,/*3*/0.8,0.5,0.4,1.0},
            /*4*/{-1.0_spos,-1.0_spos,-1.0_spos,1.0,/*4*/0.5,0.8,0.4,1.0},
            /*5*/{-1.0_spos,+1.0_spos,-1.0_spos,1.0,/*5*/0.6,0.7,0.3,1.0},
            /*6*/{+1.0_spos,+1.0_spos,-1.0_spos,1.0,/*6*/0.7,0.6,0.2,1.0},
            /*7*/{+1.0_spos,-1.0_spos,-1.0_spos,1.0,/*7*/0.8,0.5,0.1,1.0}
        };

        constexpr static std::uint16_t pointsIndex[] = {
            0,1,3,
            3,1,2,

            4,5,7,
            7,5,6,

            1,5,2,
            2,5,6,

            0,4,3,
            3,4,7,

            1,0,5,
            5,0,4,

            2,6,3,
            3,6,7,
        };

        GLuint varVAO = 0;
        glCreateVertexArrays(1, &varVAO);
        GLNamedVertexArrayObject varAns{ varVAO };

        if (varVAO == 0) {
            qDebug() << "can not get vao";
            return {};
        }

        GLuint varBuffer[2];

        glCreateBuffers(2, varBuffer);

        GLBuffer varPointBuffer{ varBuffer[0] };
        GLBuffer varIndexBuffer{ varBuffer[1] };

        if (varBuffer[0] == 0) {
            qDebug() << "gen buffer error!";
            return {};
        }
        if (varBuffer[1] == 0) {
            qDebug() << "gen buffer error!";
            return {};
        }

        /*上传数据*/
        glNamedBufferData(varBuffer[0], sizeof(varRawData), varRawData, GL_STATIC_DRAW);
        glNamedBufferData(varBuffer[1], sizeof(pointsIndex), pointsIndex, GL_STATIC_DRAW);

        /*绑定index buffer*/
        glVertexArrayElementBuffer(varVAO, varBuffer[1]);

        /*将 point and color buffer 绑定到VAO*/
        glEnableVertexArrayAttrib(varVAO, 0)/*position*/;
        glEnableVertexArrayAttrib(varVAO, 1)/*color*/;

        glVertexArrayVertexBuffer(varVAO, 0, varBuffer[0], 0, sizeof(PointAndColor));
        glVertexArrayVertexBuffer(varVAO, 1, varBuffer[0], 0, sizeof(PointAndColor));

        glVertexArrayAttribFormat(varVAO, 0, 4, GL_DOUBLE, false, 0);
        glVertexArrayAttribFormat(varVAO, 1, 4, GL_DOUBLE, false, sizeof(PointAndColor) / 2);

        glVertexArrayAttribBinding(varVAO, 0, 0);
        glVertexArrayAttribBinding(varVAO, 1, 1);

        return { std::move(varAns),std::move(varPointBuffer),std::move(varIndexBuffer) };
    }


}/*namespace*/

class OpenGLWindow::DrawData {
public:
    std::size_t _draw_stamp = 0;
    std::size_t _last_draw_stamp = std::size_t(-1);
    std::array<GLfloat, 4> _m_clean_color{ 1.0f,0.3f,1.0f,1.0f };
    std::array<GLfloat, 1> _m_clean_depth{ 1.0f };
    void update_draw_data() {
        if (_draw_stamp == _last_draw_stamp) {
            return;
        }
        _last_draw_stamp = _draw_stamp;

        /*udpate clean color*/
        _m_clean_color[0] = (std::rand() & 127)*(1.0f / 256);
        _m_clean_color[1] = (std::rand() & 255)*(1.0f / 256);
        _m_clean_color[2] = (std::rand() & 127)*(1.0f / 256);
        _m_clean_color[3] = 1.0f;

        switch (_draw_stamp % 3) {
        case 0:_m_mvp = glm::rotate(_m_mvp, (std::rand() & 255) / 256.0f, glm::vec3(0, 0, 1)); break;
        case 1:_m_mvp = glm::rotate(_m_mvp, (std::rand() & 255) / 256.0f, glm::vec3(0, 1, 0)); break;
        case 2:_m_mvp = glm::rotate(_m_mvp, (std::rand() & 255) / 256.0f, glm::vec3(1, 0, 1)); break;
        }
    }

    void construct() {
        _m_program = getProgram();
        {
            const auto varBufferAndVAO = getNamedVertexArrayObject();
            _m_named_vertex_array_object = varBufferAndVAO._m_vao;
            _m_point_color_buffer = varBufferAndVAO._m_point_color_buffer;
            _m_index_buffer = varBufferAndVAO._m_index_buffer;
        }
    }

    DrawData() {
        _m_mvp = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }

    GLProgram _m_program;
    GLNamedVertexArrayObject _m_named_vertex_array_object;
    GLBuffer _m_point_color_buffer;
    GLBuffer _m_index_buffer;
    glm::mat4 _m_mvp;
private:
    SSTD_MEMORY_DEFINE(OpenGLWindow::DrawData)
};

OpenGLWindow::OpenGLWindow(QOpenGLWindow::UpdateBehavior updateBehavior, QWindow *parent)
    :Super(updateBehavior, parent) {
    this->setMinimumWidth(64);
    this->setMinimumHeight(64);
    this->resize(512, 512);
    this->startTimer(500 + (std::rand() & 511));
}

void OpenGLWindow::initializeGL() {
    if (_m_draw_data) {
        return;
    }

    this->makeCurrent();
    glewInitialize();
    _m_draw_data = new DrawData;
    _m_draw_data->construct();

    auto varDestory = _m_draw_data;
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, 
        context(), [varDestory]() {delete varDestory; },Qt::DirectConnection);
}

void OpenGLWindow::paintGL() {
    if (nullptr == _m_draw_data) { return; }

    sstd::StateStackBasic varGLState;
    varGLState.push_value(glIsEnabled(GL_DEPTH_TEST) , 
        [](auto && v) {if (v) { glEnable(GL_DEPTH_TEST); } else { glDisable(GL_DEPTH_TEST); }})/*保存OpenGL状态*/;
      
    _m_draw_data->update_draw_data();

    const auto varFBOIndex = this->defaultFramebufferObject();

    glEnable(GL_DEPTH_TEST);
    
    glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, _m_draw_data->_m_clean_color.data());
    glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, _m_draw_data->_m_clean_depth.data());

    glUseProgram(_m_draw_data->_m_program);
    glBindVertexArray(_m_draw_data->_m_named_vertex_array_object);
    glUniformMatrix4fv(2, 1, false, &(_m_draw_data->_m_mvp[0][0]));
    glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_SHORT, nullptr);    

    glBindVertexArray(0);
    glUseProgram(0);
}

void OpenGLWindow::resizeGL(int w, int h) {
    if (nullptr == _m_draw_data) { return; }
    glViewport(0, 0, w, h);
}

bool OpenGLWindow::event(QEvent *ev) {
    if (ev->type() == QEvent::Close) {
        _m_draw_data = nullptr;
        this->deleteLater();
    }
    else if (ev->type() == QEvent::Timer) {
        if (_m_draw_data) {
            ++(_m_draw_data->_draw_stamp);
            this->update();
        }
    }
    return Super::event(ev);
}

OpenGLWindow::~OpenGLWindow() { 
    _m_draw_data = nullptr; 
}












