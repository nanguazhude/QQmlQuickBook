#pragma once

#include <sstd_memory.hpp>
#include <atomic>
#include <memory>
#include <QtGui/qwindow.h>
#include <QtCore/qthread.h>
#include <QtGui/qopenglcontext.h>

class ThreadObject;
class Window : public QWindow {
    Q_OBJECT
private:
    Window(QThread *);
public:
    QOpenGLContext * getOpenGLContex() const { return $m$OpenGLContex; }
public slots:
     
private:
    QOpenGLContext * $m$OpenGLContex = nullptr;
    friend class ThreadObject;
    void constructInThisThread();
    void destoryInThread();
    SSTD_MEMORY_QOBJECT_DEFINE(Window)
    SSTD_MEMORY_FRIND
};


