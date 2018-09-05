#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include "Window.hpp"
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include "ThreadObject.hpp"

Window::Window(){
    this->setSurfaceType(QWindow::OpenGLSurface);
    this->create();
    $m$OpenGLContex = sstdNew<QOpenGLContext>(this);
    assert(qApp);
    if (QThread::currentThread()!=qApp->thread()) {
        auto varContx = ThreadObject::getMainWindow()->getOpenGLContex();
        assert(varContx);
        $m$OpenGLContex->setShareContext(varContx);
    }
    $m$OpenGLContex->create();
    $m$OpenGLContex->makeCurrent(this);
    glewInitialize();
}


