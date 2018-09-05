#include <mutex>
#include <atomic>
#include <shared_mutex>
#include "Window.hpp"
#include "ThreadObject.hpp"
#include <QtCore/qtimer.h>
#include <QtCore/qpointer.h>
#include <QtCore/qcoreapplication.h>

namespace {

    class Global {
    public:
        virtual ~Global() = default;
        class Item {
        public:
            QThread *    $m$Thread;
            ThreadObject $m$Object;
            Item(QThread * arg) :$m$Thread(arg) {}
            friend bool operator<(const Item & l, const QThread * r) {
                return l.$m$Thread < r;
            }
            friend bool operator<(const QThread * l, const Item & r) {
                return l < r.$m$Thread;
            }
            friend bool operator<(const Item & l, const Item &r) {
                return l.$m$Thread < r.$m$Thread;
            }
        };
        sstd::set<Item> $m$Data;
        QPointer< Window > $m$MainWindow;

        std::shared_mutex $m$Mutex;

        ThreadObject * insert_item(QThread * arg) {
            {/*lock for read*/
                std::shared_lock varLock{ $m$Mutex };
                auto varPos = $m$Data.find(arg);
                if (varPos != $m$Data.end()) {
                    return const_cast<ThreadObject *>(&(varPos->$m$Object));
                }
            }
            {/*lock for write*/
                std::unique_lock varLock{ $m$Mutex };
                auto varPos = $m$Data.find(arg);
                if (varPos == $m$Data.end()) {
                    return const_cast<ThreadObject *>(&($m$Data.emplace(arg).first->$m$Object));
                }
                return const_cast<ThreadObject *>(&(varPos->$m$Object));
            }
        }

        void erase(QThread * arg) {
            /*lock for write*/
            std::unique_lock varLock{ $m$Mutex };
            $m$Data.erase(arg);
        }

    };

    Global * getGlobalData() {
        static auto varAns = sstdNew<Global>();
        return varAns;
    }

}/*global data*/

ThreadObject::ThreadObject() {
    $p$ConstructInThread();
}

void ThreadObject::$p$ConstructInThread() {
    /**/
    assert(qApp);
    const auto varQAppThread = qApp->thread();
    assert(varQAppThread);
    auto varCurrentThread = QThread::currentThread();
    assert(varCurrentThread);
    /*Window 只能在主线程创建*/
    if (varCurrentThread == varQAppThread) {
        $m$Window = sstdNew<Window>(varCurrentThread);
        getGlobalData()->$m$MainWindow = $m$Window;
        extern void constructMain();
        constructMain();
    }
    else {/*如果不在主线程，那么等待主线程创建*/
        std::promise<Window *> varWindow;
        auto varFuture = varWindow.get_future();;
        QTimer::singleShot(0, qApp, [p = &varWindow, this, varCurrentThread]() {
            p->set_value(sstdNew< Window >(varCurrentThread));
        });
        $m$Window = varFuture.get();
    }
    $m$Window->constructInThisThread();
    connect(varCurrentThread, &QThread::finished, this, [varCurrentThread]() {
        getGlobalData()->erase(varCurrentThread);
    }, Qt::DirectConnection);
    {
        auto varWindow = $m$Window;
        connect(varCurrentThread, &QThread::finished,
            $m$Window, [varWindow]() {
            varWindow->destoryInThread();
            varWindow->deleteLater();
        }, Qt::DirectConnection);
    }
}

ThreadObject::~ThreadObject() {}

ThreadObject * ThreadObject::getInThisThread() {
    /*判断当前线程中有无此对象...*/
    /*无此对象构造一个*/
    /*有则返回*/
    assert(QThread::currentThread());
    return getGlobalData()->insert_item(QThread::currentThread());
}

Window * ThreadObject::getMainWindow() {
    return getGlobalData()->$m$MainWindow.data();
}



