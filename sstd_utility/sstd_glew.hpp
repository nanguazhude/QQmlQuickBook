#pragma once

#include <GL/glew.h>

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





















