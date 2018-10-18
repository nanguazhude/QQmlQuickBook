#include "sstd_Window.hpp"
#include <ConstructQSurface.hpp>
#include <QtQuick/qquickview.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQml/qqmlengine.h>

namespace sstd {

    Window::Window() {

        mmm_Mutex = sstd::make_shared<WindowState>();

        this->setMinimumHeight(128);
        this->setMinimumWidth(128);

        /*标记为OpenGL渲染环境*/
        this->setSurfaceType(QSurface::OpenGLSurface);
        this->setFormat(sstd::getDefaultOpenGLFormat());

    }

    Window::~Window() {

        auto varMutex = mmm_Mutex;
        varMutex->setDestory();

        /*rending need this data , so wait for rending finished*/
        for (;;) {
            if (varMutex->renderCount() > 0) {
                std::this_thread::sleep_for(10ns);
            } else {
                break;
            }
        }

        /*destory the contex*/
        if (this->getContex()) {
            this->getContex()->deleteLater();
        }

    }

    bool Window::event(QEvent *event) {
        switch (event->type()) {
            case QEvent::UpdateRequest:
                this->ppp_InitAndRepaint();
                return true;
            default:
                return QWindow::event(event);
        }
    }

    void Window::exposeEvent(QExposeEvent *event) {
        this->ppp_InitAndRepaint();
        return;
        (void)event;
    }

    void Window::ppp_InitAndRepaint() {
        if (mmm_Contex == nullptr) {
            mmm_Contex = sstdNew<QOpenGLContext>();
            mmm_Contex->setFormat(sstd::getDefaultOpenGLFormat());
            mmm_Contex->create();
        }
        /*************************************************/
        auto varRender = makeRender();
        /*************************************************/
    }

    std::shared_ptr<sstd::RenderPack> Window::makeRender() {
        if (mmm_RenderPack) {
            return mmm_RenderPack;
        }
        /*this function should be run in main thread*/
        auto varPack = sstd::make_shared<sstd::RenderPack>();
        mmm_RenderPack = varPack;
        /*copy this data */
        varPack->targetWindow = this;
        varPack->targetWindowState = this->mmm_Mutex;
        varPack->targetWindowContex = this->mmm_Contex;
        /*create offscreen surface*/
        varPack->sourceOffscreenSurface = sstd::make_shared<QOffscreenSurface>();
        varPack->sourceOffscreenSurface->setFormat(sstd::getDefaultOpenGLFormat());
        varPack->sourceOffscreenSurface->create();
        /*make render control*/
        varPack->sourceViewControl = sstd::make_shared<QQuickRenderControl>();
        /*create engine*/
        varPack->sourceQQmlEngine = sstd::make_shared<QQmlEngine>();
        varPack->sourceView = sstd::make_shared<QQuickWindow>(varPack->sourceViewControl.get());
        if (!varPack->sourceQQmlEngine->incubationController()) {
            varPack->sourceQQmlEngine->setIncubationController(varPack->sourceView->incubationController());
        }

        return std::move(varPack);
    }

} /*namespace sstd*/












