#include "sstd_Render.hpp"
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qopenglframebufferobject.h>
#include <QtQuick/qquickwindow.h>
#include <QtQml/QtQml>
#include "sstd_RenderControl.hpp"


namespace sstd {

    /*构造渲染环境...*/
    Render::Render(std::shared_ptr<QOffscreenSurface> arg) {
        setRenderSurface(std::move(arg));
        ppp_ConstructAll();
    }

    /*清理渲染环境...*/
    Render::~Render() {
        ppp_CleanAll();
    }

    /*构造渲染环境...*/
    void Render::ppp_ConstructAll() {

        if (true == mmm_isInit) {
            return;
        }
        mmm_isInit = true;
        if (true == mmm_isDestory) {
            return;
        }

        /*创建OpenGL环境*/
        this->setRenderContex(sstdNew<QOpenGLContext>());
        this->getRenderContex()->setFormat(sstd::getDefaultOpenGLFormat());
        this->getRenderContex()->create();
        this->getRenderContex()->makeCurrent(this->getRenderSurface());

        /*创建渲染窗口*/
        this->setRenderControl(sstdNew<sstd::RenderControl>());
        this->setRenderSource(sstdNew<QQuickWindow>(this->getRenderControl()));
        this->setRenderSourceEngine(sstdNew<QQmlEngine>());

    }

    /*清理渲染数据...*/
    void Render::ppp_CleanAll() {
        auto varSurface = std::move(mmm_RenderSurface);
        if (true == mmm_isDestory) {
            return;
        }
        mmm_isDestory = true;
        if (false == mmm_isInit) {
            return;
        }
        class DestoryLock {
            Render  * const thisd;
        public:
            DestoryLock(Render * r) :thisd(r) {
                thisd->getRenderContex()->makeCurrent(thisd->getRenderSurface());
            }
            ~DestoryLock() {
                thisd->renderThreadQuit();
                thisd->getRenderContex()->doneCurrent();
                delete thisd->getRenderContex();
            }
        } varLock{ this };
        if (mmm_Target) {
            delete mmm_Target;
            mmm_Target = nullptr;
        }
        if (mmm_RenderControl) {
            delete mmm_RenderControl;
        }
        if (mmm_SourceQuickWindow) {
            delete mmm_SourceQuickWindow;
        }
        if (mmm_SourceEngine) {
            delete mmm_SourceEngine;
        }
    }

    RenderControl * Render::getRenderControl() const {
        return mmm_RenderControl;
    }

    void Render::setRenderControl(RenderControl *arg) {
        assert(nullptr == mmm_RenderControl);
        mmm_RenderControl = arg;
    }

    QOpenGLContext * Render::getRenderContex() const {
        return mmm_RenderContext;
    }

    void Render::setRenderContex(QOpenGLContext * arg) {
        assert(nullptr == mmm_RenderContext);
        mmm_RenderContext = arg;
    }

    QQuickWindow * Render::getRenderSource() const {
        return mmm_SourceQuickWindow;
    }

    void Render::setRenderSource(QQuickWindow * arg) {
        assert(nullptr == mmm_SourceQuickWindow);
        mmm_SourceQuickWindow = arg;
    }

    QOffscreenSurface * Render::getRenderSurface() const {
        return mmm_RenderSurface.get();
    }

    void Render::setRenderSurface(std::shared_ptr<QOffscreenSurface> && arg) {
        assert(false == bool(mmm_RenderSurface));
        mmm_RenderSurface = std::move(arg);
    }

    QQmlEngine * Render::getRenderSourceEngine()const {
        return mmm_SourceEngine;
    }

    void Render::setRenderSourceEngine(QQmlEngine * arg) {
        assert(nullptr == mmm_SourceEngine);
        mmm_SourceEngine = arg;
    }

}/*namespace sstd*/















