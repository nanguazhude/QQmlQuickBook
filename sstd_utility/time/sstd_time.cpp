#if defined(NO_SSTD_TIME_CPP)

#else

#include <sstd_memory.hpp>
#include "sstd_time.hpp"
#include <ctime>
#include <memory>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <condition_variable>
#include <optional>

namespace sstd {

    TimerThread::TimerThread() {
    }

}/*namespace sstd*/

namespace {
    class SimpleGC {
    public:

        using GCSet = std::set<
            std::shared_ptr<const void>,
            std::owner_less<void>,
            sstd::allocator<std::shared_ptr<const void>>>;
        using GCSetConstPos = GCSet::const_iterator;

        class GCObject {
        public:
            const std::shared_ptr<const void> * mmm_Data;
            GCSetConstPos mmm_Pos;
            std::optional< std::chrono::steady_clock::time_point > mmm_MarkDeleteTime;
        public:

            GCObject(const std::shared_ptr<const void> * a, GCSetConstPos b) :
                mmm_Data(a),
                mmm_Pos(std::move(b)) {
            }

            GCObject(const GCObject &) = default;
            GCObject(GCObject &&) = default;
            GCObject&operator=(const GCObject &) = default;
            GCObject&operator=(GCObject &&) = default;

        private:
            SSTD_MEMORY_DEFINE(GCObject)
        };
        using GCList = sstd::list<GCObject>;

        std::shared_mutex mmm_Mutex_Objecs;
        GCList mmm_Objecs;
        std::shared_mutex mmm_Mutex_ObjectsSet;
        GCSet mmm_ObjectsSet;
        constexpr const static std::size_t mmm_GCStepSize{ 1024 };

        void insert(std::shared_ptr<const void > arg) {

            if (false == bool(arg)) {
                return;
            }

            bool varIsInsert{ false };
            GCSetConstPos varConstPos;

            {
                std::unique_lock varWriteLock{ mmm_Mutex_ObjectsSet };
                std::tie(varConstPos, varIsInsert) = mmm_ObjectsSet.insert(std::move(arg));
            }

            /*the data have in it ... */
            if (false == varIsInsert) {
                return;
            }

            {
                /*push it to front ... */
                std::unique_lock varWriteLock{ mmm_Mutex_Objecs };
                mmm_Objecs.emplace_front(&(*varConstPos), std::move(varConstPos));
            }

            return;
        }

        void gcStep() {

            std::size_t varAllElementSize{ 0 };
            {
                std::shared_lock varReadLock{ mmm_Mutex_Objecs };
                varAllElementSize = mmm_Objecs.size();
            }

            if (varAllElementSize < 1) {
                return;
            }

            GCList varTmpData;
            GCList varAboutToDelete;
            GCList varAboutToDeleteNextTime;

            /*get object to gc ... */
            if (varAllElementSize < mmm_GCStepSize) {
                std::unique_lock varWriteLock{ mmm_Mutex_Objecs };
                varTmpData.splice(varTmpData.begin(), std::move(mmm_Objecs));
            } else {
                std::unique_lock varWriteLock{ mmm_Mutex_Objecs };
                auto varMoveFistPos = mmm_Objecs.begin();
                auto varMoveLastPos = std::next(varMoveFistPos, mmm_GCStepSize);
                varTmpData.splice(varTmpData.begin(),
                    mmm_Objecs,
                    varMoveFistPos,
                    varMoveLastPos);
            }

            const auto varNow = std::chrono::steady_clock::now();
            while (false == varTmpData.empty()) {
                auto varI = varTmpData.begin();
                if (varI->mmm_MarkDeleteTime) {
                    if (std::chrono::abs(varNow - *(varI->mmm_MarkDeleteTime)) > 10ms) {
                        /*about to delete ...*/
                        varAboutToDelete.splice(varAboutToDelete.end(), varTmpData, varI);
                        continue;
                    }
                } else if (varI->mmm_Data->use_count() < 2) {
                    /*mark it to delete ...*/
                    varI->mmm_MarkDeleteTime = varNow;
                }
                /*try delete next time ...*/
                varAboutToDeleteNextTime.splice(varAboutToDeleteNextTime.end(), varTmpData, varI);
            }

            /*add try delete next time ...*/
            if (varAboutToDeleteNextTime.empty() == false) {
                std::unique_lock varWriteLock{ mmm_Mutex_Objecs };
                mmm_Objecs.splice(mmm_Objecs.end(), std::move(varAboutToDeleteNextTime));
            }

            /*delete now ...*/
            if (varAboutToDelete.empty() == false) {
                std::unique_lock varWriteLock{ mmm_Mutex_ObjectsSet };
                for (const auto & varI : varAboutToDelete) {
                    /*really delete ... */
                    mmm_ObjectsSet.erase(varI.mmm_Pos);
                }
            }

        }

    private:
        SSTD_MEMORY_DEFINE(SimpleGC)
    };
}/*namespace*/

namespace sstd::private_thread {

    std::atomic<bool> & isMainQuit() {
        static std::atomic<bool> varAns{ false };
        return varAns;
    }

    std::atomic<TimerThread::type> & getTimer() {
        static std::atomic<TimerThread::type> varAns{ (255)&((TimerThread::type)(std::time(nullptr))) };
        return varAns;
    }

    class PrivateTimerThread final :
        public std::enable_shared_from_this<PrivateTimerThread>,
        public TimerThread {
        std::atomic<type> & mmm_Value{ getTimer() };
        std::atomic<bool> & mmm_IsQuit{ isMainQuit() };
        mutable std::shared_mutex mmm_Mutex_Functions;
        sstd::list<std::function<void(void)>> mmm_Functions;
        std::atomic<int> mmm_RunThreadCount{ 0 };
        mutable std::condition_variable mmm_Wait;
        SimpleGC mmm_GC;
    public:

        void addUniqueDelete(std::shared_ptr<const void> arg) override {
            /*insert data ...*/
            mmm_GC.insert(std::move(arg));
            mmm_Wait.notify_all();
        }

        void tryUniqueData() {
            /*eval a gc ...*/
            mmm_GC.gcStep();
        }

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
            mmm_Wait.notify_all();
            return mmm_Value.load();
        }

        void postFunction(std::function<void(void)> arg) override {

            if (false == bool(arg)) {
                return;
            }

            {
                std::unique_lock varWriteLock{ mmm_Mutex_Functions };
                mmm_Functions.push_back(std::move(arg));
            }

            if (mmm_IsQuit.load() == false) {
                mmm_Wait.notify_all();
            }

        }

        void callFunctions() {
            /*确保只额外启动一个执行线程,如果还有执行线程在运行，那么就等下次执行...*/
            if (mmm_RunThreadCount.load() > 0) {
                return;
            }

            /*测试执行是否为空...*/
            {
                std::shared_lock varReadLock{ mmm_Mutex_Functions };
                if (mmm_Functions.empty()) {
                    return;
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
        }

        void run() {
            while (mmm_IsQuit.load() == false) {
                std::mutex varMutex;
                std::unique_lock varLocker{ varMutex };

                auto varLockBegin = std::chrono::steady_clock::now();
                int varWakeTimeCount = 0;

                /*
                当等待超时则退出等待
                当被唤醒100次之后，则退出等待
                当被唤醒时，检查如果有函数需要运行，则退出等待；
                如果等待时间超过10ms，则退出等待
                */
                constexpr const static auto varWaitTime = 10ms;
                static_assert(varWaitTime > 2ms);
                mmm_Wait.wait_for(varLocker, varWaitTime,/*continue*/
                    [this, varLockBegin, &varWakeTimeCount]() mutable ->bool {
                    ++varWakeTimeCount;
                    if (varWakeTimeCount > 100) {
                        return true;
                    }

                    if ((functionsAboutToRun() != 0) && (mmm_RunThreadCount.load() == 0)) {
                        return true;
                    }

                    const auto varCurrentTime = std::chrono::steady_clock::now();
                    return (std::chrono::abs(varCurrentTime - varLockBegin) >= (varWaitTime - 1ms));
                })/*这里可能有虚假唤醒，无关紧要...*/;

                ++mmm_Value;
                /*增加执行函数...*/
                {
                    std::unique_lock varWriteLock{ mmm_Mutex_Functions };
                    mmm_Functions.push_back([thisp = this->shared_from_this()]() {
                        thisp->tryUniqueData();
                    });
                }

                callFunctions();
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
        /*构造时开始计时*/
        StartUpThis() {
            mmm_Data = instance();
        }
        /*程序退出时停止计时*/
        ~StartUpThis() {
            mmm_Data->quit();
        }
    };

}/*namespace sstd*/

namespace sstd {
    std::shared_ptr<TimerThread> getTimeStamp() {
        /*调用此函数开启计数器*/
        static sstd::private_thread::StartUpThis globalThis;
        return sstd::private_thread::instance();
    }
}/*namespace sstd*/

#endif

