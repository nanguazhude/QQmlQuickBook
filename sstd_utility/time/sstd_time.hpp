#pragma once

#include "../sstd_core_library/global_sstd_core_library.hpp"
#include <memory>
#include <cstdint>
#include <cstddef>
#include <functional>

namespace sstd {

    class _1_SSTD_CORE_EXPORT TimerThread {
    protected:
        TimerThread();
    public:
        virtual ~TimerThread() = default;
        using type = std::uint64_t;
        /*返回一个时间戳，
        返回值应当是递增的，
        当超过std::uint64_t之后重置为0，
        至少保证10ms变化一次值*/
        virtual type getCurrentTime() const = 0;
        /*时间戳发生变化时运行此函数一次，
        然后此函数被清除,
        此函数抛出的异常将被忽略,
        函数执行顺序不被保证,
        函数执行线程不保证,
        函数插入时间点和函数执行时间点间隔不保证,
        主函数退出后函数是否被运行不被保证*/
        virtual void postFunction(std::function<void(void)>) = 0;
        /*返回队列中还存有多少函数*/
        virtual std::size_t functionsAboutToRun() const = 0;
        /*当前线程对对象具有所有权，
        当其他线程对此对象无所有权后，
        延时删除对象
        设计用于构造std::promise,packaged_task*/
        template<typename T,typename ... Args>
        std::shared_ptr<T> threadOwnCreate(Args && ...args) const {
            auto varAns = sstd::make_shared<T>(std::forward<Args>(args)...);
            const_cast<TimerThread*>(this)->addUniqueDelete(varAns);
            return std::move(varAns);
        }
    protected:
        /*为shared_ptr在当前线程做一个备份，当其他线程取消引用时删除*/
        virtual void addUniqueDelete(std::shared_ptr<const void>) = 0 ;
    };

    _1_SSTD_CORE_EXPORT std::shared_ptr<TimerThread> getTimeStamp();

}/*namespace sstd*/
















