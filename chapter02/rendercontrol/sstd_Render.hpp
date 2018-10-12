#pragma once

#include "sstd_RenderPack.hpp"
#include "sstd_RenderControl.hpp"
#include <ConstructQSurface.hpp>
#include <QtGui/qoffscreensurface.h>
#include <QtQml/QtQml>
#include <shared_mutex>

namespace sstd {
    class Render : public QObject {
        Q_OBJECT
    public:
        Render(std::shared_ptr<RenderPack>)/*此函数将在任意线程构造*/;
    public:
        sstd::RenderControl * getRenderControl() const;
        QOpenGLContext * getRenderContex() const;
        QQuickWindow * getRenderSource() const;
        QOffscreenSurface * getRenderSurface() const;
        QQmlEngine * getRenderSourceEngine()const;
        RenderPack * getRenderPack() const;
        ~Render();
        Q_SIGNAL void renderThreadQuit();
    private:
        template<typename T>
        class Atomic {
            T mmm_Data;
            mutable std::shared_mutex mmm_Mutex;
        public:
            template<typename U>
            Atomic(U && a):mmm_Data(std::forward<U>(a)) {
            }
            Atomic() :mmm_Data{} {
            }
            Atomic(const Atomic &)=delete;
            Atomic(Atomic&&)=delete;
            Atomic&operator=(Atomic&)=delete;
            Atomic&operator=(Atomic&&)=delete;
            const T & getData() const {
                std::shared_lock varReadLock{mmm_Mutex};
                return mmm_Data;
            }
            template<typename U>
            bool setData( U && a ) {
               using InputType = std::remove_cv_t< std::remove_reference_t<U> >;
               if constexpr ( std::is_same_v<InputType, std::remove_cv_t<T> > ) {
                   {
                       std::shared_lock varReadLock{ mmm_Mutex };
                       if (a == mmm_Data) {
                           return false;
                       }
                   }
                   std::unique_lock varWriteLock{ mmm_Mutex };
                   mmm_Data = std::forward<U>(a);
                   return true;
               } else {
                   std::remove_cv_t<T> varTmpData{ std::forward<U>(a) };
                   {
                       std::shared_lock varReadLock{ mmm_Mutex };
                       if (varTmpData == mmm_Data) {
                           return false;
                       }
                   }
                   std::unique_lock varWriteLock{ mmm_Mutex };
                   mmm_Data = std::move(varTmpData);
                   return true;
               }
            }
        };

        Atomic<QSize> mmm_WindowSize;
        std::int_fast32_t mmm_CountWindowSizeEvent{0};
        Q_SLOT void ppp_WindowSizeChanged();
    public:
        QSize getWindowSize() const {
            return mmm_WindowSize.getData();
        }
        void setWindowSize(const QSize & arg) {
            if (mmm_WindowSize.setData(arg)) {
                windowSizeChanged();
            }
        }
        Q_SIGNAL void windowSizeChanged();
    public:
        Q_SLOT void resizeEventFunction(const QSize &);
        Q_SLOT void renderEventFunction();
        Q_SIGNAL void renderChanged(const QImage &);
    private:
        QImage mmm_RenderResult;
    protected:
        bool event(QEvent *)override;
        void ppp_Event(QEvent *);
    private:
        void ppp_RenderAgain();
    private:
        void ppp_CleanAll();
        void ppp_ConstructAll();
    private:
        void setRenderSource(QQuickWindow *);
        void setRenderControl(sstd::RenderControl *);
        void setRenderContex(QOpenGLContext *);
        void setRenderSurface(QOffscreenSurface *);
        void setRenderSourceEngine(QQmlEngine *);
        void setRenderPack(std::shared_ptr<RenderPack>&&);
    private:
        /*渲染状态*/
        bool mmm_isInit{ false };
        bool mmm_isDestory{ false };
        /*渲染源*/
        sstd::RenderControl * mmm_RenderControl{ nullptr };
        QQuickWindow * mmm_SourceQuickWindow{ nullptr };
        QQmlEngine * mmm_SourceEngine{nullptr};
        /*渲染目标*/
        QOpenGLFramebufferObject * mmm_Target{ nullptr };
        /*渲染环境*/
        QOpenGLContext * mmm_RenderContext{ nullptr }/*OpengGL渲染环境，此变量所在线程应当与this（Render）相同*/;
        QOffscreenSurface * mmm_RenderSurface{nullptr}/*此变量是为了构造mmm_RenderContext，此变量必须在主线程构造*/;
        std::shared_ptr<RenderPack>mmm_RenderPack;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Render)
    };
} // namespace sstd




