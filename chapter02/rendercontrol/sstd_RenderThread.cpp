#include <sstd_glew.hpp>
#include "sstd_RenderThread.hpp"
#include <QtCore/qcoreapplication.h>
#include <ConstructQSurface.hpp>
#include <QtGui/qevent.h>

namespace {

    inline auto getEventIndex() {
        const static auto varAns = static_cast<QEvent::Type>(QEvent::registerEventType());
        return varAns;
    }

    class CallEvent : public QEvent {
        sstd::vector< std::packaged_task<void(void)> > mmm_calls;
        std::shared_ptr< sstd::vector< std::future<void> > > mmm_ans;
    public:
        CallEvent(sstd::vector< std::packaged_task<void(void)> > arg) :QEvent(getEventIndex()) {
            mmm_ans = sstd::make_shared<sstd::vector< std::future<void> >>();
            mmm_calls = std::move(arg);
            mmm_ans->reserve(arg.size());
            for (auto & varI : mmm_calls) {
                mmm_ans->push_back( varI.get_future() );
            }
        }

        std::shared_ptr< const sstd::vector< std::future<void> > > getFuture() const {
            return mmm_ans ;
        }

        void call()  {
            for (auto & varI : mmm_calls) {
                varI();
            }
        }  
    private:
        SSTD_MEMORY_DEFINE(CallEvent)
    };
}/**/

namespace sstd {

    RenderThread::RenderThread(std::shared_ptr<sstd::RenderPack> arg) : mmm_RenderPack(std::move(arg)) {
        this->moveToThread(qApp->thread());
        connect(this, &QThread::finished, this, &RenderThread::on_this_thread_finished, Qt::QueuedConnection);
        connect(qApp, &QCoreApplication::aboutToQuit, this, &QThread::quit, Qt::QueuedConnection);
    }

    RenderThread::~RenderThread() {

    }

    void RenderThread::quitRender() {
        std::call_once(this->mmm_CallOnce, [this]() {
            mmm_RenderPack->targetWindowState->subRenderCount();
        });
    }

    void RenderThread::on_this_thread_finished() {
        this->deleteLater();
        quitRender();
    }

    void RenderThread::run() {
        private_sstd::ThreadRunObject varObj;
        mmm_CallObject = &varObj;
        /******************************************/
        mmm_RenderPack->sourceContex = sstd::make_unique<QOpenGLContext>();
        mmm_RenderPack->sourceContex->setFormat(sstd::getDefaultOpenGLFormat());
        mmm_RenderPack->sourceContex->create();
        mmm_RenderPack->sourceContex->setShareContext(mmm_RenderPack->targetWindowContex);
        mmm_RenderPack->sourceContex->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());
        glewInitialize();
        /******************************************/
        mmm_RenderPack->targetWindowState->addRenderCount();
        /******************************************/
        this->exec();
    }

    std::shared_ptr< const sstd::vector< std::future<void> > > RenderThread::_callInThisThread(sstd::vector< std::packaged_task<void(void)> > arg) {
        auto varCall = mmm_CallObject.data();
        if (varCall == nullptr) {
            return {};
        }

        auto varEvent = sstdNew<CallEvent>( std::move( arg ) );
        auto varAns = varEvent->getFuture();
        if (varCall) {
            QCoreApplication::postEvent(varCall, varEvent);
        }
        return std::move(varAns);
    }

    namespace private_sstd {
        bool ThreadRunObject::event(QEvent * e) {
            if (e->type() == getEventIndex()) {
                static_cast<CallEvent *>(e)->call();
                return true;
            }
            return QObject::event(e);
        }
    }/*private_sstd*/

} /* namespace sstd */


