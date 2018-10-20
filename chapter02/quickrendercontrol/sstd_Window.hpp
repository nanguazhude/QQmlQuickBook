#pragma once

#include <sstd_memory.hpp>
#include <sstd_RenderPack.hpp>
#include <QWindow>

namespace sstd {

    class Window : public QWindow {
        Q_OBJECT

    public:
        Window();
        ~Window();

    protected:
        void exposeEvent(QExposeEvent *e) override;
        void resizeEvent(QResizeEvent *e) override;
        void mousePressEvent(QMouseEvent *e) override;
        void mouseReleaseEvent(QMouseEvent *e) override;
        bool event(QEvent *e) override;

    private :
        Q_SLOT void requestUpdate();
        Q_SLOT void justRender();
        Q_SLOT void polishSyncAndRender();
        Q_SLOT void polishSyncAndRenderResize();
    private:
        template<bool needPolish,bool needSync,bool isResize>
        void ppp_PolishSyncAndRender();
    private:
        void startQuick(const QUrl &filename);
        void updateSizes();

        
        bool mmm_QuickInitialized;
        bool mmm_psrRequested;

        std::shared_ptr<sstd::RenderPack> mmm_RenderPack;

    };

}/*namespace sstd*/

 
