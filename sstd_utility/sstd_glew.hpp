#pragma once

#include <GL/glew.h>
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

#define enable_gl_debug_function(...)  $p$sstdPushDebugGroup()
#define disable_gl_debug_function(...) $p$sstdPopDebugGroup()


#else

#define enable_gl_debug_function(...)
#define disable_gl_debug_function(...)

#endif


















