#pragma once

#include "sstd_RenderControl.hpp"
#include <ConstructQSurface.hpp>
#include <QtGui/qoffscreensurface.h>
#include <QtQml/QtQml>

namespace sstd {
    class Render : public QObject {
        Q_OBJECT
    public:
        Render(std::shared_ptr<QOffscreenSurface>)/*此函数将在任意线程构造*/;
    public:
        sstd::RenderControl * getRenderControl() const;
        QOpenGLContext * getRenderContex() const;
        QQuickWindow * getRenderSource() const;
        QOffscreenSurface * getRenderSurface() const;
        QQmlEngine * getRenderSourceEngine()const;
        ~Render();
        Q_SIGNAL void renderThreadQuit();
    private:
        void ppp_CleanAll();
        void ppp_ConstructAll();
    private:
        void setRenderSource(QQuickWindow *);
        void setRenderControl(sstd::RenderControl *);
        void setRenderContex(QOpenGLContext *);
        void setRenderSurface(std::shared_ptr<QOffscreenSurface> &&arg);
        void setRenderSourceEngine(QQmlEngine *);
    private:
        /*渲染状态*/
        bool mmm_isInit{ false };
        bool mmm_isDestory{ false };
        /*渲染源*/
        sstd::RenderControl * mmm_RenderControl{ nullptr };
        QQuickWindow * mmm_SourceQuickWindow{ nullptr };
        QQmlEngine * mmm_SourceEngine{nullptr};
        /*渲染目标*/
        QOpenGLFramebufferObject * mmm_Target{ nullptr };
        /*渲染环境*/
        QOpenGLContext * mmm_RenderContext{ nullptr }/*OpengGL渲染环境，此变量所在线程应当与this（Render）相同*/;
        std::shared_ptr<QOffscreenSurface>mmm_RenderSurface/*此变量是为了构造mmm_RenderContext，此变量必须在主线程构造*/;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Render)
    };
} // namespace sstd




