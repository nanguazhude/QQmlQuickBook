#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include "Window.hpp"
#include <QtCore/qcoreapplication.h>

Window::Window(){
    this->setSurfaceType(QWindow::OpenGLSurface);
    this->create();
    $m$OpenGLContex = sstdNew<QOpenGLContext>(this);
    $m$OpenGLContex->create();
    $m$OpenGLContex->makeCurrent(this);
    glewInitialize();
}


