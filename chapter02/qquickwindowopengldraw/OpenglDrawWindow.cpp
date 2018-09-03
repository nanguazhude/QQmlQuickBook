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

    class GLTexture : private BasicGLIndex<GLTexture> {
    private:
        using Super = BasicGLIndex<GLTexture>;
        template<typename T> friend class BasicGLIndex;
        static inline void _p_destory(const GLuint *a) {
            glDeleteTextures(1, a);
        }
    public:
        GLTexture() = default;
        GLTexture&operator=(const GLTexture&) = default;
        GLTexture&operator=(GLTexture &&) = default;
        GLTexture(const GLTexture&) = default;
        GLTexture(GLTexture &&) = default;
        explicit GLTexture(GLuint a) :Super(a) {}
        explicit operator bool() const { return Super::_p_to_bool(); }
        operator GLuint() const { return Super::_p_get_data(); }
    private:
        SSTD_MEMORY_DEFINE(GLTexture)
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
        varAns.$m$VAO = GLNamedVertexArrayObject{ varVAO };

        return varAns;

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
                varI.$m$Data[3] = std::fma((0.1 / 64), varIndex++, 0.5);
            }
        }

        GLuint varBuffer[1];

        glCreateBuffers(1, varBuffer);
        varAns.$m$InstanceBuffer = GLBuffer{ varBuffer[0] };

        /*上传数据*/
        glNamedBufferStorage(varBuffer[0],
            sizeof(varInitData), 
            varInitData, 
            GL_MAP_WRITE_BIT| GL_MAP_READ_BIT| GL_DYNAMIC_STORAGE_BIT);
                 
        /*将 point and color buffer 绑定到VAO*/
        glEnableVertexArrayAttrib(varVAO, 0)/*instance*/;
        /*指定如何从Buffer中解出包*/
        glVertexArrayVertexBuffer(varVAO, 0, varBuffer[0], 0, sizeof(RowData));
        /*指定如何从包中获得数据*/
        glVertexArrayAttribFormat(varVAO, 0, 4, GL_DOUBLE, false, 0);

        glVertexArrayAttribBinding(varVAO, 0, 0);

        glVertexArrayBindingDivisor(varVAO, 0, 1);

        return std::move(varAns);
    }

    GLTexture getTexture() {

        const static QString varFileNames[] = {
            QStringLiteral("myqml/qquickwindowopengldraw/01.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/02.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/03.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/04.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/05.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/06.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/07.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/08.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/09.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/10.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/11.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/12.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/13.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/14.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/15.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/16.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/17.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/18.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/19.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/20.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/21.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/22.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/23.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/24.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/25.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/26.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/27.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/28.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/29.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/30.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/31.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/32.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/33.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/34.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/35.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/36.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/37.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/38.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/39.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/40.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/41.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/42.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/43.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/44.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/45.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/46.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/47.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/48.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/49.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/50.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/51.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/52.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/53.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/54.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/55.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/56.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/57.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/58.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/59.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/60.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/61.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/62.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/63.png"),
            QStringLiteral("myqml/qquickwindowopengldraw/00.png"),
        };

        {
            int varWidth = 0;
            int varHeight = 0;
            std::array<QImage, std::size(varFileNames)> varImages;
            {
                const QDir varAppDir{ qApp->applicationDirPath() };
                auto * varOut = varImages.data();
                for (const auto & varF : varFileNames) {
                    const auto varFileName = varAppDir.absoluteFilePath(varF);
                    auto varImage = QImage{ varFileName };
                    varWidth = varImage.width();
                    varHeight = varImage.height();
                    varImage = varImage.convertToFormat(QImage::Format_RGBA8888);
                    *varOut++ = std::move(varImage);
                    /*这仅仅是一个示例代码，数据大小和格式是固定的*/
                    assert(varHeight == 256);
                    assert(varWidth == 256);
                }
            };

            /*这仅仅是一个示例代码，数据大小和格式是固定的*/
            assert(varImages.size() == 64);
            assert(varImages[0].format() == QImage::Format_RGBA8888);

            /*创建Texture Array*/
            GLuint texture = 0;
            glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
            glTextureStorage3D(texture, 1, GL_RGBA8,
                varWidth, varHeight,
                static_cast<int>(varImages.size()/*depth*/));

            /*上传数据*/
            for (int varI = 0; varI < varImages.size(); ++varI) {
                glTextureSubImage3D(texture, 0/*level​*/, 0/*x*/, 0/*y*/, varI/*z*/,
                    varWidth, varHeight, 1,
                    GL_RGBA, GL_UNSIGNED_BYTE, varImages[varI].bits());
            }

            return GLTexture{ texture };

        }

        return {};
    }

}/*namespace*/

class OpenglDrawWindowItemRender::DrawData {
public:

    GLint $m$Width = 0;
    GLint $m$Height = 0;

    GLProgram $m$Program;
    GLNamedVertexArrayObject $m$VAO;
    GLBuffer $m$VAOBuffer;
    GLTexture $m$Texture;
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

        //$m$Texture = getTexture();
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
        using Type = std::array<Row, getArraySize()>;

        //auto varData = static_cast<Type *>(
        //    glMapNamedBuffer(_m_draw_data->$m$VAOBuffer, GL_READ_WRITE));
        //for (auto & varI : (*varData)) {/**update x ,y , and rotate **/
        //    //qDebug() << varI.$m$Z;
        //}
        //glUnmapNamedBuffer(_m_draw_data->$m$VAOBuffer);
    }

    GLuint varFBOIndex = _m_window->renderTargetId();
    glViewport(0, 0, _m_draw_data->$m$Width, _m_draw_data->$m$Height);

    //glEnable(GL_DEPTH_TEST);

    glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, _m_draw_data->$m$ClearColor.data());
    glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, _m_draw_data->$m$ClearDepth.data());

    glUseProgram(_m_draw_data->$m$Program);
    glBindVertexArray(_m_draw_data->$m$VAO);
    //glActiveTexture(GL_TEXTURE0 + 0);
    //glBindTexture(GL_TEXTURE_2D_ARRAY, _m_draw_data->$m$Texture);
    //glBindTextureUnit(0, _m_draw_data->$m$Texture);
    //glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, getArraySize());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

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

void OpenglDrawWindowItem::setUpdateValue(const double & arg) {
    if (arg == _m_update_value) { return; }
    _m_update_value = arg;
    updateValueChanged();
    if (window()) { window()->update(); }
}

/*****************************************************/
//https://www.khronos.org/opengl/wiki/Texture_Storage
/*****************************************************/
