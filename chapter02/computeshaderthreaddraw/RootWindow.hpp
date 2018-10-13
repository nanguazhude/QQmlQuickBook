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
        ClassState(const ClassState &) = delete;
        ClassState(ClassState&&) = delete;
        ClassState&operator=(const ClassState &) = delete;
        ClassState&operator=(ClassState &&) = delete;
        friend RootWindow;
        using This = ClassState;
    protected:
        bool mmm_IsDesotry{ false };
        int mmm_RendingCount{ false };
        void setDestory() {
            std::unique_lock varWriteLock{ *const_cast<This*>(this) };
            mmm_IsDesotry = true;
        }
    public:
        ClassState() = default;
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
        SSTD_MEMORY_DEFINE(ClassState)
    };

    using MutexPointer = std::shared_ptr< ClassState >;
    QOpenGLContext * getContex() const {
        return mmm_Contex;
    }

    MutexPointer getMutex() const {
        return mmm_Mutex;
    }
protected:
    void initialize();
    void openglDraw();
    bool event(QEvent *event)override;
    void exposeEvent(QExposeEvent *event) override;
private:
    QOpenGLContext * mmm_Contex{ nullptr };
    MutexPointer     mmm_Mutex;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(RootWindow)
};

















