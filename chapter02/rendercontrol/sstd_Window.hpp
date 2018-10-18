#pragma once

#include <shared_mutex>
#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>
#include "sstd_RenderPack.hpp"
#include "sstd_Window.hpp"

namespace sstd {

    class Window : public QWindow {
        Q_OBJECT
    public:
        Window();
        ~Window();

        QOpenGLContext * getContex() const {
            return mmm_Contex;
        }

    private:
        QOpenGLContext * mmm_Contex{ nullptr };
        std::shared_ptr<WindowState> mmm_Mutex;
        std::shared_ptr<RenderPack> mmm_RenderPack;
        void ppp_InitAndRepaint();
        Q_SIGNAL void ppp_QuitRender();
    protected:
        virtual std::shared_ptr<sstd::RenderPack> makeRender();
    protected:
        bool event(QEvent *event) override;
        void exposeEvent(QExposeEvent *event) override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Window)
    };

} /*namespace sstd*/








