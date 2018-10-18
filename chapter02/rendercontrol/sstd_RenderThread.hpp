#pragma once 

#include <sstd_memory.hpp>
#include "sstd_RenderPack.hpp"
#include "sstd_RenderPack.hpp"
#include <QtCore/qthread.h>
#include <mutex>

namespace sstd{
     
    class RenderThread : public QThread {
        Q_OBJECT
    private:
        std::once_flag mmm_CallOnce{};
        std::shared_ptr<sstd::RenderPack> mmm_RenderPack;
    public:
        RenderThread( std::shared_ptr<sstd::RenderPack> );
        ~RenderThread();
    public:
        Q_SLOT void quitRender();
    protected:
        void run() override;
    protected:
        Q_SLOT void on_this_thread_finished();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(RenderThread)
    };

}
















