#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qthread.h>
#include <QtCore/qpointer.h>
#include <QtGui/qoffscreensurface.h>

namespace sstd {
    namespace private_sstd {
        /*此类调用start后自删除*/
        class RenderThreadBasic : public QThread {
            Q_OBJECT
        public:
            RenderThreadBasic()/*此函数必须在主线程调用*/;
            ~RenderThreadBasic();
        protected:
            std::shared_ptr< QOffscreenSurface > getOffscreenSurface() const {
                return mmm_Surface;
            }
        private:
            std::shared_ptr< QOffscreenSurface > mmm_Surface /*在构造函数里面构造一个Surface*/;
        private:
            SSTD_MEMORY_QOBJECT_DEFINE(RenderThreadBasic)
        };
    }/*namespace private_sstd*/
} /*namespace sstd*/


namespace sstd {

    /*
     * T : 必须接受构造函数std::shared_ptr< QOffscreenSurface >
     * T : 必须有信号renderThreadQuit
     * T : 必须是QQbject
    */
    template<typename T >
    class RenderThread : public private_sstd::RenderThreadBasic {
        std::atomic<T * > mmm_DataAtomic{ nullptr };
        QPointer<T> mmm_DataWatcher;
        std::function<void(T*)> mmm_Construct;
    public:

        void run() override {
            try {
                auto varConstructFunction = std::move(mmm_Construct);
                auto thisRender = sstdNew<T>(this->getOffscreenSurface());
                connect(thisRender, &T::renderThreadQuit,
                    this, &QThread::quit, Qt::QueuedConnection);
                connect(thisRender, &QObject::destroyed,
                    this, &QThread::quit, Qt::QueuedConnection);
                if (varConstructFunction) {
                    varConstructFunction(thisRender);
                }
                mmm_DataWatcher = thisRender;
                mmm_DataAtomic.store(thisRender);
                this->exec();
            } catch (...) {
            }
        }

        RenderThread(std::function<void(T*)> arg = {}) :mmm_Construct(std::move(arg)) {
            this->start();
        }

        T * getRender() const volatile {
            while (mmm_DataAtomic.load() == nullptr) {
                std::this_thread::sleep_for(10ns);
            }
            return const_cast<const QPointer<T> &>(mmm_DataWatcher).data();
        }

    };

}/*namespace sstd*/

















