#pragma once

#include <mutex>
#include <shared_mutex>
#include <sstd_memory.hpp>

namespace sstd {

    class Window;
    class WindowState final : public std::shared_mutex {
        SSTD_DELETE_COPY_ASSIGN(WindowState);
    private:
        friend Window;
        using This = WindowState;
    protected:
        bool mmm_IsDesotry{ false };
        int mmm_RendingCount{ false };
        void setDestory() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            mmm_IsDesotry = true;
        }
    public:
        WindowState() = default;
        bool isDestory() const volatile {
            std::shared_lock varReadLock{ *const_cast<This*>(this) };
            return mmm_IsDesotry;
        }
        int renderCount() const volatile {
            std::shared_lock varReadLock{ *const_cast<This*>(this) };
            return mmm_RendingCount;
        }
        void addRenderCount() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            ++mmm_RendingCount;
        }
        void subRenderCount() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            --mmm_RendingCount;
        }
    private:
        SSTD_MEMORY_DEFINE(WindowState)
    };

}/*sstd*/



