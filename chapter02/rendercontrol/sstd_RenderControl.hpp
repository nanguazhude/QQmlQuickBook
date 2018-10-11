#pragma once

#include "sstd_RenderPack.hpp"
#include <sstd_memory.hpp>
#include <QtQuick/qquickrendercontrol.h>

namespace sstd {
    class RenderControl : public QQuickRenderControl {
        Q_OBJECT
    public:
        RenderControl();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(RenderControl)
    };
} // namespace sstd















