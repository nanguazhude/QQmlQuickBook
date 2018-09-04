#pragma once

#include <GL/glew.h>
#include <cassert>
extern bool glewInitialize();

namespace sstd{

inline static constexpr unsigned char $p$sstdGlewDebugGroupIndex(){ 
    return static_cast<unsigned char>(35);
}

inline static constexpr unsigned char $p$sstdGlewDebugGroupStringLength(){
    return static_cast<unsigned char>(16);
}

inline static constexpr const char * $p$sstdGlewDebugGroupString(){
    return "sstdGlewDebugGroupString";
}

inline static void $p$sstdPushDebugGroup(){
    glPushDebugGroup(
        GL_DEBUG_SOURCE_APPLICATION,
        $p$sstdGlewDebugGroupIndex(),
        $p$sstdGlewDebugGroupStringLength(),
        $p$sstdGlewDebugGroupString());
}

inline static void $p$sstdPopDebugGroup(){
    glPopDebugGroup();
}

}/****************************/

#if defined(ENABLE_GL_DEBUG)

#define enable_gl_debug_function(...)  sstd::$p$sstdPushDebugGroup()
#define disable_gl_debug_function(...) sstd::$p$sstdPopDebugGroup()

namespace sstd {
    class ___$p$GLDL {
    public:
        ___$p$GLDL() {
            enable_gl_debug_function();
        }
        ~___$p$GLDL() {
            disable_gl_debug_function();
        }
    };
}/*********/

#define gl_debug_function_lock(...) sstd::___$p$GLDL ___$p$locker{}

#else

#define enable_gl_debug_function(...)   assert(true)
#define disable_gl_debug_function(...)  assert(true)
#define gl_debug_function_lock(...)     assert(true)

#endif


















