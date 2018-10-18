#pragma once

#include <memory>
#include "sstd_WindowState.hpp"
class QWindow;
class QQuickWindow;
class QOffscreenSurface;
class QOpenGLContext;
class QQuickRenderControl;
class QQmlEngine;

namespace  sstd{

    class RenderPack : public std::enable_shared_from_this<RenderPack> {
    public:
        virtual ~RenderPack() /*这里不搞极限设计*/ ;

        QWindow * targetWindow /**/ =nullptr;
        QOpenGLContext * targetWindowContex/**/ =nullptr;
        std::shared_ptr<sstd::WindowState> targetWindowState/**/;
        std::shared_ptr<QQuickWindow> sourceView /**/ ;
        std::shared_ptr<QQuickRenderControl> sourceViewControl/**/;
        std::shared_ptr<QOffscreenSurface> sourceOffscreenSurface/**/;
        std::shared_ptr<QQmlEngine> sourceQQmlEngine;
    };


}/*namesapce sstd*/













