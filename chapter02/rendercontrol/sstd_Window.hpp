﻿#pragma once

#include <shared_mutex>
#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>

namespace sstd {

    class Window : public QWindow {
        Q_OBJECT
    public:
        Window();
        ~Window();

        class ClassState final : public std::shared_mutex {
            SSTD_DELETE_COPY_ASSIGN(ClassState);
        private:
            friend Window;
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

        QOpenGLContext * getContex() const {
            return mmm_Contex;
        }

    private:
        QOpenGLContext * mmm_Contex{ nullptr };
        std::shared_ptr<ClassState> mmm_Mutex;
        void ppp_InitAndRepaint();
    protected:
        bool event(QEvent *event) override;
        void exposeEvent(QExposeEvent *event) override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Window)
    };

} /*namespace sstd*/








