#pragma once

#include <mutex>
#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>
#include <QtGui/qopenglcontext.h>

class RootWindow : public QWindow {
    Q_OBJECT
public:
    RootWindow();
    ~RootWindow();

    class ClassState : public std::recursive_mutex {
    public:
        bool isDesotry{false};
        int isRending{false};
    };

    using MutexPointer = std::shared_ptr< ClassState >;
    QOpenGLContext * getContex() const { return mmm_Contex; }
    MutexPointer getMutex() const { return mmm_Mutex; }

private:
    QOpenGLContext * mmm_Contex{ nullptr };
    MutexPointer     mmm_Mutex;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(RootWindow)
};

















