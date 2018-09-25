#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include <QtQuick/QQuickWindow>

namespace sstd {

    class WindowOpenGLDrawControl : public QObject {
        Q_OBJECT
    public:
        WindowOpenGLDrawControl(QQuickWindow *);
        static void setToWindow(QQuickWindow *);
    public:
        Q_SLOT void beforeRendering();
    private:
        using Super = QObject;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(WindowOpenGLDrawControl)
    };

}/*namespace sstd*/










