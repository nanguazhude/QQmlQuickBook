#pragma once

#include <sstd_memory.hpp>
#include <sstd_RenderPack.hpp>
#include <QWindow>
#include <QtQuick/qquickview.h>

namespace sstd {

    class Window : public QQuickView {
        Q_OBJECT
    public:
        Window();
        ~Window();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Window)
    };

}/*namespace sstd*/


