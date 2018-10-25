#pragma once

#include "../sstd_memory.hpp"
#include "global_sstd_core_library.hpp"

namespace sstd {

    class _0_PrivateScene2D;
    class _1_SSTD_CORE_EXPORT Scene2D {
    public:
        virtual ~Scene2D();
        Scene2D();
    public:
        std::shared_ptr<_0_PrivateScene2D> thisp;
    public:
        SSTD_MEMORY_DEFINE(Scene2D)
    };

}/*namespace sstd*/













