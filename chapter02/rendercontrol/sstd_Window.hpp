#pragma once

#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>

namespace sstd {

    class Window : public QWindow {
        Q_OBJECT
    public:
        Window();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Window)
    };

} /*namespace sstd*/








