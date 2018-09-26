#pragma once

#include <mutex>
#include <thread>
#include <memory>

#if defined(NDEBUG)

#ifndef debug_call_once
#define debug_call_once(...)/*just do nothing*/
#endif

#else

template<typename T>
void __private_call_once(T && f) {
    static std::once_flag varFlag;
    std::call_once(varFlag, std::forward<T>(f));
}

#ifndef debug_call_once
#define debug_call_once __private_call_once
#endif

#endif







