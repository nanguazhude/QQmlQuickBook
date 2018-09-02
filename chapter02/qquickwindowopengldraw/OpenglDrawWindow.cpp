#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "OpenglDrawWindow.hpp"
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickwindow.h>

extern bool glewInitialize();


class OpenglDrawWindowItemRender::DrawData{
public:
    DrawData() {
        glewInitialize();
    }

    ~DrawData() {}

private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindowItemRender::DrawData)
};

OpenglDrawWindowItem::OpenglDrawWindowItem(QQuickItem *parent) :Super(parent) {
    this->setFlag(QQuickItem::ItemHasContents, true);
    connect(this, &QQuickItem::windowChanged, this, &OpenglDrawWindowItem::handleWindowChanged);
}

OpenglDrawWindowItemRender::OpenglDrawWindowItemRender() {}

void OpenglDrawWindowItem::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &OpenglDrawWindowItem::sync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &OpenglDrawWindowItem::cleanup, Qt::DirectConnection);
        window->setClearBeforeRendering(false);
    }
}

void OpenglDrawWindowItem::sync() {
    if ( _m_render == nullptr ) {
        _m_render = sstdNew<OpenglDrawWindowItemRender>();
        connect(
            window(),&QQuickWindow::beforeRendering,
            _m_render,&OpenglDrawWindowItemRender::paintGL,
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
    _m_draw_data = sstdNew< DrawData >();
    /*******************************************/
}

void OpenglDrawWindowItemRender::paintGL() {
    if (_m_window == nullptr) { return; }
    initializeGL();

    class ResetGLStateLock {
        QQuickWindow * _m;
    public:
        ResetGLStateLock(QQuickWindow *l):_m(l) {}
        ~ResetGLStateLock() { _m->resetOpenGLState();}
    } varGLStateLock{_m_window};
        

    GLuint varFBOIndex = _m_window->renderTargetId();
    
    GLfloat xxx[]{1,1,1,1};
    glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, xxx);
    glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, xxx);

}

void OpenglDrawWindowItemRender::resizeGL(int w, int h){
    glViewport(0,0,w,h);
}

OpenglDrawWindowItemRender::~OpenglDrawWindowItemRender() { delete _m_draw_data; }

