#pragma once

#include <mutex>
#include <thread>
#include <memory>

#define _disable_debug_call_once 1

#if defined(_disable_debug_call_once) 

#ifndef debug_call_once
#define debug_call_once(...)/*just do nothing*/((void)0)
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

 





