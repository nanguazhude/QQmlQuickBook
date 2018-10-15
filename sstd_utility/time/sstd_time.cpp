#include <sstd_memory.hpp>
#include "sstd_time.hpp"
#include <ctime>
#include <memory>
#include <atomic>
#include <thread>
#include <shared_mutex>

namespace sstd {

    TimerThread::TimerThread() {
    }

}/*namespace sstd*/

namespace sstd::private_thread {

    std::atomic<bool> & isMainQuit() {
        static std::atomic<bool> varAns{ false };
        return varAns;
    }

    class PrivateTimerThread :
        public std::enable_shared_from_this<PrivateTimerThread>,
        public TimerThread {
        std::atomic<type> mmm_Value{ (type)(std::time(nullptr)) };
        std::atomic<bool> & mmm_IsQuit{ isMainQuit() };
        mutable std::shared_mutex mmm_Mutex_Functions;
        sstd::list<std::function<void(void)>> mmm_Functions;
        std::atomic<int> mmm_RunThreadCount{ 0 };
    public:

        class Runner final {
            std::shared_ptr<PrivateTimerThread> mmm_Super;
            sstd::list<std::function<void(void)>> mmm_Functions;
        public:

            Runner(PrivateTimerThread * r) {
                r->addRunner();
                mmm_Super = r->shared_from_this();
            }

            ~Runner() {
                mmm_Super->removeRunner();
            }

            inline void run() {
                auto & argFunctions = mmm_Functions;
                while (argFunctions.empty() == false) {
                    try {
                        auto varFunction = std::move(argFunctions.front());
                        argFunctions.pop_front();
                        varFunction();
                    } catch (...) {
                        /*忽略所有异常*/
                    }
                }
            }

            template<typename U>
            inline void setFunctions(U&&arg) {
                mmm_Functions = std::forward<U>(arg);
            }

        private:
            SSTD_MEMORY_DEFINE(Runner)
        };

        void addRunner() {
            ++mmm_RunThreadCount;
        }

        void removeRunner() {
            --mmm_RunThreadCount;
        }

        void quit() {
            mmm_IsQuit.store(true);
        }

        type getCurrentTime() const override {
            return mmm_Value.load();
        }

        void postFunction(std::function<void(void)> arg) override {
            std::unique_lock varWriteLock{ mmm_Mutex_Functions };
            mmm_Functions.push_back(std::move(arg));
        }

        void run() {
            while (mmm_IsQuit.load() == false) {

                std::this_thread::sleep_for(500us);
                ++mmm_Value;

                /*确保只额外启动一个执行线程,如果还有执行线程在运行，那么就等下次执行...*/
                if (mmm_RunThreadCount.load() > 0) {
                    continue;
                }

                /*测试执行是否为空...*/
                {
                    std::shared_lock varReadLock{ mmm_Mutex_Functions };
                    if (mmm_Functions.empty()) {
                        continue;
                    }
                }

                /*构造一个执行器*/
                auto varRunner = sstd::make_shared< Runner >(this);

                /*将执行函数拷贝出来...*/
                {
                    std::unique_lock varWriteLock{ mmm_Mutex_Functions };
                    varRunner->setFunctions(std::move(mmm_Functions));
                }

                /*运行执行函数...*/
                std::thread([varRunner]() mutable { varRunner->run(); }).detach();

            }/*while*/
        }

        PrivateTimerThread() {
        }

        ~PrivateTimerThread() {
            mmm_IsQuit.store(true);
        }

        std::size_t functionsAboutToRun() const override {
            std::shared_lock varReadLock{ mmm_Mutex_Functions };
            return mmm_Functions.size();
        }

    private:
        SSTD_MEMORY_DEFINE(PrivateTimerThread)
    };

    std::shared_ptr<PrivateTimerThread> instance() {
        static const std::shared_ptr<PrivateTimerThread> varAns = []() {
            auto varAns = sstd::make_shared<PrivateTimerThread>();
            std::thread([varAns]() {varAns->run(); }).detach();
            return std::move(varAns);
        }();
        return varAns;
    }

    class StartUpThis {
    public:
        std::shared_ptr<PrivateTimerThread> mmm_Data;
        /*主线程开始运行时开始计时*/
        StartUpThis() {
            mmm_Data = instance();
        }
        /*程序退出时停止计时*/
        ~StartUpThis() {
            mmm_Data->quit();
        }
    };

    static StartUpThis globalThis;

}/*namespace sstd*/

namespace sstd {
    std::shared_ptr<TimerThread> getTimeStamp() {
        return sstd::private_thread::instance();
    }
}/*namespace sstd*/

