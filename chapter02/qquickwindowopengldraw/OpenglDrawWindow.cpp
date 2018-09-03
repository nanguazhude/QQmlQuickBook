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

class OpenglDrawWindowItemRender::DrawData {
public:

    GLint $m$Width  = 0;
    GLint $m$Height = 0;

    DrawData() {
        glewInitialize();
    }

    ~DrawData() {}

private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindowItemRender::DrawData)
};

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

}/*namespace*/


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

void OpenglDrawWindowItemRender::paintGL() {
    if (_m_window == nullptr) { return; }
    initializeGL();

    class ResetGLStateLock {
        QQuickWindow * _m;
    public:
        ResetGLStateLock(QQuickWindow *l) :_m(l) {}
        ~ResetGLStateLock() { if constexpr (true) { _m->resetOpenGLState(); } }
    } varGLStateLock{ _m_window };

    GLuint varFBOIndex = _m_window->renderTargetId();


    glViewport(0, 0, _m_draw_data->$m$Width, _m_draw_data->$m$Height);


    GLfloat xxx[]{ 0.3,0.4,0.5,1 };
    glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, xxx);
    glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, xxx);



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
