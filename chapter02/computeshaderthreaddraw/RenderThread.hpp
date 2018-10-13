#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qthread.h>
#include <RootWindow.hpp>

class QImage;
class QWindow;
class QOffscreenSurface;

namespace sstd{

    /*this class should be created in main thread or gui thread*/
    class RenderThread : public QThread {
        Q_OBJECT
    private:
        using Super = QThread;
        QString mmm_ImageFileName;
        QWindow * const mmm_DrawWindow{nullptr};
        RootWindow::MutexPointer mmm_Mutex;
    public:
        RenderThread(RootWindow *);
        ~RenderThread();

        void start(const QString &arg);
        auto * getDrawWindow() const { return mmm_DrawWindow; }
    public:
        void run() override ;
    public:
        Q_SIGNAL void renderFinished(const QImage &);
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(RenderThread)
    };


}/**/









