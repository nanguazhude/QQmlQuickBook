#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include "Window.hpp"
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include "ThreadObject.hpp"

/*在主线程运行*/
Window::Window(){
    this->setSurfaceType(QWindow::OpenGLSurface);
    this->create();
    if (QThread::currentThread() == qApp->thread()) {
        constructInThisThread();
    }
}

/*在创建线程运行*/
void Window::constructInThisThread() {
    if ($m$OpenGLContex)return;
    $m$OpenGLContex = sstdNew<QOpenGLContext>(this);
    assert(qApp);
    if (QThread::currentThread() != qApp->thread()) {
        auto varContx = ThreadObject::getMainWindow()->getOpenGLContex();
        assert(varContx);
        $m$OpenGLContex->setShareContext(varContx);
    }
    $m$OpenGLContex->create();
    $m$OpenGLContex->makeCurrent(this);
    glewInitialize();
}

/*在创建线程运行*/
void Window::destoryInThread() {
    delete $m$OpenGLContex;
    $m$OpenGLContex = nullptr;
}

