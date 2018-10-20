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
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickwindow.h>
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qoffscreensurface.h>
#include <quick/sstd_QThread.hpp>

namespace  sstd {

    class RenderPack : public std::enable_shared_from_this<RenderPack> {
    protected:
        RenderPack()/*此类应当被继承*/;
    public:
        std::atomic<double> targetWindowDevicePixelRatio{ 1 };
        std::atomic<int> targetWindowWidth{ 1 };
        std::atomic<int> targetWindowHeight{ 1 };
        QWindow * targetWindow /**/ = nullptr;
        QOpenGLContext * globalWindowContex/**/ = nullptr;
        std::unique_ptr<QOpenGLContext> targetContex/**/;
        std::unique_ptr<QQuickWindow> sourceView /**/;
        std::unique_ptr<QOpenGLContext> sourceContex /**/;
        std::unique_ptr<QOffscreenSurface> sourceOffscreenSurface/**/;
        std::unique_ptr<QQuickRenderControl> sourceViewControl/**/;
        std::unique_ptr<QQmlEngine> sourceQQmlEngine;
        sstd::QuickThread * renderThread{ nullptr };
        QQuickItem * sourceRootItem{nullptr};
        virtual ~RenderPack();
    };


}/*namesapce sstd*/













