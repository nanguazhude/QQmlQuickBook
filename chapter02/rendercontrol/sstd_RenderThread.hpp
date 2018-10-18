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
        template<typename ... Args>
        std::shared_ptr< const sstd::vector< std::future<void> > > callInThisThread(Args && ... args) {
            sstd::vector< std::packaged_task<void(void)> > varAns;
            varAns.reserve(sizeof...(Args));
            (varAns.emplace_back(std::forward<Args>(args)),...);
            return _callInThisThread(std::move(varAns));
        }
    private:
        std::shared_ptr< const sstd::vector< std::future<void> > > _callInThisThread(sstd::vector< std::packaged_task<void(void)> >);
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
















