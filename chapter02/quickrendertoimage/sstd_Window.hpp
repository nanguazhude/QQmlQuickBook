#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickview.h>
#include <QtGui/qimage.h>
#include <quick/sstd_QThread.hpp>

namespace sstd {

    class Window : public QQuickView {
        Q_OBJECT
    public:
        Window();
        ~Window();
    public:
        Q_SIGNAL void renderFinished(const QImage &);
        Q_SIGNAL void startRender(const QString &/*qml path*/);
    public:
        sstd::QuickThread * getRenderThread()const {
            return mmm_Thread;
        }
    private:
        Q_SLOT void ppp_start_render(const QString &/*qml path*/);
        Q_SLOT void ppp_1_start_render(const QVariant &/*qml path*/, const QVariant &);
        void ppp_RenderFinished(const QImage &);
        sstd::QuickThread * mmm_Thread{nullptr};
        using Super = QQuickView;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Window)
    };

}/*namespace sstd*/


