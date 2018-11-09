/*重写这个模块，调整程序内存分配策略...*/
#include "../sstd_memory.hpp"

#if __has_include(<jemalloc.h>)
#define JEMALLOC_INCLUDE 1
#else
#define JEMALLOC_INCLUDE 0
#endif

#if JEMALLOC_INCLUDE
#include <jemalloc.h>
#else

#endif

namespace sstd{

    namespace unique {
        VirtualBasic::~VirtualBasic() {
        }
    }/*namespace unique */

#if JEMALLOC_INCLUDE


    void* SSTDMemory::operator new (std::size_t count) {
        return ::je_malloc(count);
    }

    void SSTDMemory::operator delete(void * ptr) {
        return ::je_free(ptr);
    }

    void* SSTDMemory::operator new[](std::size_t count) {
        return ::je_malloc(count);
    }

    void SSTDMemory::operator delete[](void * ptr) {
        return ::je_free(ptr);
    }

    void* SSTDMemory::operator new(std::size_t count, std::align_val_t al) {
        return ::je_aligned_alloc(static_cast<std::size_t>(al),count);
    }

    void SSTDMemory::operator delete(void* ptr, std::align_val_t al) {
        return ::je_free(ptr); (void)al;
    }

    void* SSTDMemory::operator new[](std::size_t count, std::align_val_t al) {
        return ::je_aligned_alloc(static_cast<std::size_t>(al), count);
    }

    void SSTDMemory::operator delete[](void* ptr, std::align_val_t al) {
        return ::je_free(ptr); (void)al;
    }

#else

    void* SSTDMemory::operator new (std::size_t count) {
        return ::operator new(count);
    }

    void SSTDMemory::operator delete(void * ptr) {
        return ::operator delete(ptr);
    }

    void* SSTDMemory::operator new[](std::size_t count) {
        return ::operator new(count);
    }

    void SSTDMemory::operator delete[](void * ptr) {
        return ::operator delete(ptr);
    }

    void* SSTDMemory::operator new(std::size_t count, std::align_val_t al) {
        return ::operator new(count, al);
    }

    void SSTDMemory::operator delete(void* ptr, std::align_val_t al) {
        return ::operator delete(ptr, al);
    }

    void* SSTDMemory::operator new[](std::size_t count, std::align_val_t al) {
        return ::operator new(count, al);
    }

    void SSTDMemory::operator delete[](void* ptr, std::align_val_t al) {
        return ::operator delete(ptr, al);
    }
#endif

}/*namespace sstd*/



namespace sstd::memory_lock {

    namespace _0_private {
        VirtualBasic::~VirtualBasic() {
        }
        VirtualBasic::VirtualBasic() {
        }
    }/*_0_private*/

    VirtualClassBasic::VirtualClassBasic() : mmm_data( sstdNew<DataType>() ) {
    }

    VirtualClassBasic::~VirtualClassBasic() {
        delete mmm_data;
    }

}/*namespace sstd::memory_lock*/




