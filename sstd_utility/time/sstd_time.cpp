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

    class PrivateTimerThread : public TimerThread {
        std::atomic<type> mmm_Value{ (type)(std::time(nullptr)) };
        std::atomic<bool> mmm_IsQuit{ false };
        std::shared_mutex mmm_Mutex_Functions;
        sstd::list<std::function<void(void)>> mmm_Functions;
    public:

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
                std::this_thread::sleep_for(100us);
                ++mmm_Value;
                /*测试执行是否为空...*/
                {
                    std::shared_lock varReadLock{ mmm_Mutex_Functions };
                    if (mmm_Functions.empty()) {
                        continue;
                    }
                }
                /*将执行函数拷贝出来...*/
                std::remove_cv_t<std::remove_reference_t<decltype(mmm_Functions)>> varFunctions;
                {
                    std::unique_lock varWriteLock{ mmm_Mutex_Functions };
                    varFunctions = std::move(mmm_Functions);
                }
                /*运行执行函数...*/
                std::thread([argFunctions = std::move(varFunctions)]() mutable {
                    while (argFunctions.empty() == false) {
                        try {
                            auto varFunction = std::move(argFunctions.front());
                            argFunctions.pop_front();
                            varFunction();
                        } catch (...) {
                            /*忽略所有异常*/
                        }
                    }
                }).detach();
            }
        }

        PrivateTimerThread() {
        }

        ~PrivateTimerThread() {
            mmm_IsQuit.store(true);
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

