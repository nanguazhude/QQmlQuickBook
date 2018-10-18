#include "sstd_QThread.hpp"
#include <QtGui/qevent.h>
#include <QtCore/qcoreapplication.h>

namespace {

    using PackType = std::packaged_task<void(void)>;

    inline auto getEventIndex() {
        const static auto varAns = static_cast<QEvent::Type>(QEvent::registerEventType());
        return varAns;
    }

    class RunThisEvent : public QEvent {
        sstd::vector< PackType > mmm_Functions;
        std::shared_ptr< sstd::vector< std::future<void> > > mmm_Futures;
    public:

        RunThisEvent(sstd::vector< PackType > && arg) :
            QEvent(getEventIndex()),
            mmm_Functions(std::move(arg)) {
            mmm_Futures = sstd::make_shared<sstd::vector< std::future<void> >>();
            mmm_Futures->reserve(mmm_Functions.size());
            for (auto & varI : mmm_Functions) {
                mmm_Futures->push_back(varI.get_future());
            }
        }

        std::shared_ptr< const sstd::vector< std::future<void> > > getFutures()const {
            return mmm_Futures;
        }

        void run() {
            /*call functions in order*/
            for (auto & varI : mmm_Functions) {
                varI();
            }
        }

    public:
        SSTD_MEMORY_DEFINE(RunThisEvent)
    };

}/*namespace*/

namespace sstd {

    namespace private_quick_thread_sstd {

        bool RunEventObject::event(QEvent * e) {
            if (mmm_logical_quit->load()) {
                /*skip event when logical quit*/
                return true;
            }
            if (e->type() == getEventIndex()) {
                static_cast<RunThisEvent *>(e)->run();
                return true;
            }
            return QObject::event(e);
        }

        RunEventObject::RunEventObject(std::shared_ptr<std::atomic_bool> arg) :mmm_logical_quit(std::move(arg)) {
        }

    }/*namespace private_quick_thread_sstd*/

    void  QuickThread::run() {
        private_quick_thread_sstd::RunEventObject varObj{ mmm_logical_quit };
        {/*set the value ... */
            std::unique_lock varWriteLock{ mmm_Mutex };
            mmm_CallObject = &varObj;
        }

        try {
            this->exec();
        } catch (...) {
            /*ignore all exception*/
        }

        {/*wait all used ... */
            std::unique_lock varWriteLock{ mmm_Mutex };
            mmm_CallObject = nullptr;
        }

    }

    QuickThread::QuickThread() {
        mmm_logical_quit = sstd::make_shared< std::atomic_bool >(false);
        this->moveToThread(qApp->thread())/*move to main thread*/;
        connect(this, &QThread::finished,
            this, &QuickThread::ppp_on_finished,
            Qt::QueuedConnection)/*delete itself when finished*/;
        this->start();
        for (;;) {
            /* wait for init finished ... */
            std::shared_lock varReadLock{ mmm_Mutex };
            if (mmm_CallObject) {
                return;
            }
        }
    }

    void QuickThread::ppp_on_finished() {
        this->deleteLater();
    }

    QuickThread::~QuickThread() {

    }

    std::shared_ptr< const sstd::vector< std::future<void> > > QuickThread::ppp_Call(sstd::vector<std::packaged_task<void(void)>> && arg) {
        std::shared_lock varReadLock{ mmm_Mutex };
        if (mmm_CallObject) {
            auto varEvnet = sstdNew<RunThisEvent>(std::move(arg));
            auto varAns = varEvnet->getFutures();
            QCoreApplication::postEvent(mmm_CallObject, varEvnet);
            return std::move(varAns);
        }
        return{};
    }

}/*namespace sstd*/


