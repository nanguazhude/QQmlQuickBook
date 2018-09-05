#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include <QtCore/qthread.h>

class Window;
class ThreadObject : public QObject {
    Q_OBJECT
public:
    ThreadObject();
    ~ThreadObject();
public:
    static ThreadObject * getInThisThread();
    static Window * getMainWindow();
private:
    void $p$ConstructInThread();
private:
    Window * $m$Window = nullptr;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(ThreadObject)
};















