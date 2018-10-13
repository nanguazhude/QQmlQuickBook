#pragma once

#include <mutex>
#include <shared_mutex>
#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>
#include <QtGui/qopenglcontext.h>

class RootWindow : public QWindow {
    Q_OBJECT
public:
    RootWindow();
    ~RootWindow();

    class ClassState : public std::shared_mutex {
        ClassState(const ClassState &)=delete;
        ClassState(ClassState&&)=delete;
        ClassState&operator=(const ClassState &)=delete;
        ClassState&operator=(ClassState &&)=delete;
        friend RootWindow;
        using This = ClassState;
    protected:
        bool mmm_IsDesotry{ false };
        int mmm_IsRending{ false };
        void setDestory() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            mmm_IsDesotry = true;
        }
    public:
        ClassState() = default;
        bool isDestory() const {
            std::shared_lock varReadLock{ *const_cast<This*>(this) };
            return mmm_IsDesotry;
        }
        int renderCount() const {
            std::shared_lock varReadLock{ *const_cast<This*>(this) };
            return mmm_IsRending;
        }
        void addRenderCount() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            ++mmm_IsRending;
        }
        void subRenderCount() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            --mmm_IsRending;
        }
    private:
        SSTD_MEMORY_DEFINE(ClassState)
    };

    using MutexPointer = std::shared_ptr< ClassState >;
    QOpenGLContext * getContex() const {
        return mmm_Contex;
    }

    MutexPointer getMutex() const {
        return mmm_Mutex;
    }

private:
    QOpenGLContext * mmm_Contex{ nullptr };
    MutexPointer     mmm_Mutex;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(RootWindow)
};

















