#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qthread.h>

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
        QOffscreenSurface * mmm_Surface=nullptr;
    public:
        RenderThread();
        ~RenderThread();

        void start(const QString &arg);

        auto * getSurface() const { return mmm_Surface; }
    public:
        void run() override ;
    public:
        Q_SIGNAL void renderFinished(const QImage &);
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(RenderThread)
    };


}/**/









