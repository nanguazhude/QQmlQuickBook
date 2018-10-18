#pragma once

#include <memory>
#include "sstd_WindowState.hpp"
class QWindow;
class QQuickWindow;
class QOffscreenSurface;
class QOpenGLContext;
class QQuickRenderControl;
class QQmlEngine;
#include <QtQml/qqmlengine.h>
#include <QtQuick/qquickwindow.h>
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qoffscreensurface.h>

namespace  sstd{

    class RenderPack : public std::enable_shared_from_this<RenderPack> {
    public:
        virtual ~RenderPack() /*这里不搞极限设计*/ ;
        std::atomic<double> targetWindowDevicePixelRatio{ 1 };
        QWindow * targetWindow /**/ =nullptr;
        QOpenGLContext * targetWindowContex/**/ =nullptr;
        std::shared_ptr<sstd::WindowState> targetWindowState/**/;
        std::unique_ptr<QQuickWindow> sourceView /**/ ;
        std::unique_ptr<QQuickRenderControl> sourceViewControl/**/;
        std::unique_ptr<QOffscreenSurface> sourceOffscreenSurface/**/;
        std::unique_ptr<QQmlEngine> sourceQQmlEngine;
    };


}/*namesapce sstd*/













