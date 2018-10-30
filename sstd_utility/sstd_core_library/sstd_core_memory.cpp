/*重写这个模块，调整程序内存分配策略...*/
#include "../sstd_memory.hpp"

namespace sstd{

    namespace unique {
        VirtualBasic::~VirtualBasic() {
        }
    }/*namespace unique */

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




