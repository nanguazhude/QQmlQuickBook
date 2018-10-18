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
        std::promise<void> mmm_CallState;
        std::function<void(void)> mmm_Call;
    public:
        CallEvent(std::function<void(void)> && arg) :QEvent(getEventIndex()), mmm_Call(std::move(arg)) {
        }

        std::future<void> getFuture() {
            return mmm_CallState.get_future();
        }

        void call()  {
            try {
                if (mmm_Call) {
                mmm_Call();
            }
            } catch (...) {
                /*TODO : */
            }
            mmm_CallState.set_value();
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

    std::future<void> RenderThread::callInThisThread(std::function<void(void)> arg) {
        auto varCall = mmm_CallObject.data();
        auto varEvent = sstdNew<CallEvent>(std::move(arg));
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


