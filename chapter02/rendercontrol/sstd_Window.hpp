#pragma once

#include <mutex>
#include <future>
#include <shared_mutex>
#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>
#include <quick/sstd_QThread.hpp>
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
        sstd::QuickThread * mmm_RenderThread{ nullptr };
        QOpenGLContext * mmm_Contex{ nullptr };
        std::shared_ptr<RenderPack> mmm_RenderPack;
        void ppp_Init();
        Q_SLOT void ppp_RenderRequested();
        Q_SLOT void ppp_SceneChanged();
        Q_SLOT void ppp_UpdateSize();
    protected:
        virtual std::shared_ptr<sstd::RenderPack> makeRender();
        std::once_flag mmm_LoadQml{};
        virtual void loadQml();
    protected:
        bool event(QEvent *event) override;
        void exposeEvent(QExposeEvent *event) override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Window)
    };

} /*namespace sstd*/








