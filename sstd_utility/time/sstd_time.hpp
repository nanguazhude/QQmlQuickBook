#pragma once

#include <memory>
#include <cstdint>
#include <cstddef>
#include <functional>

namespace sstd{

    class TimerThread {
    protected:
        TimerThread();
    public:
        virtual ~TimerThread() = default ;
        using type = std::uint64_t;
        /*返回一个时间戳，每次时间戳变化间隔应当小于1ms*/
        virtual type getCurrentTime() const = 0;
        /*时间戳发生变化时运行此函数一次，然后此函数被清除,此函数抛出的异常将被忽略,函数执行顺序不被保证*/
        virtual void postFunction(std::function<void(void)>) =0 ;
    };

    std::shared_ptr<TimerThread> getTimeStamp();

}/*namespace sstd*/
















