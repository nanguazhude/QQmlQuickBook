/*重写这个模块，调整程序内存分配策略...*/
#include "../sstd_memory.hpp"

namespace sstd{

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

}/*namespace sstd*/








