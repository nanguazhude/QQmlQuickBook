#pragma once 

#include <sstd_memory.hpp>
#include "sstd_RenderPack.hpp"
#include "sstd_RenderPack.hpp"
#include <QtCore/qthread.h>
#include <QtCore/qpointer.h>
#include <mutex>
#include <future>
#include <functional>

namespace sstd{
     
    namespace private_sstd{

        class ThreadRunObject : public QObject {
        public:
            bool event(QEvent *) override;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(ThreadRunObject)
        };
    }/*private_sstd*/

    class RenderThread : public QThread {
        Q_OBJECT
    private:
        std::once_flag mmm_CallOnce{};
        std::shared_ptr<sstd::RenderPack> mmm_RenderPack;
        QPointer<private_sstd::ThreadRunObject> mmm_CallObject{nullptr};
    public:
        RenderThread( std::shared_ptr<sstd::RenderPack> );
        ~RenderThread();
    public:
        std::future<void> callInThisThread(std::function<void(void)>);
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
















