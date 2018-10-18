#include <sstd_glew.hpp>
#include "sstd_Window.hpp"
#include <ConstructQSurface.hpp>
#include <QtQuick/qquickview.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQml/qqmlengine.h>
#include "sstd_RenderThread.hpp"

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

        ppp_QuitRender();

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
                this->ppp_Init();
                return true;
            default:
                return QWindow::event(event);
        }
    }

    void Window::exposeEvent(QExposeEvent *event) {
        this->ppp_Init();
        return;
        (void)event;
    }

    void Window::ppp_Init() {

        if (mmm_Mutex->isDestory()) {
            return;
        }

        if (mmm_Contex == nullptr) {
            mmm_Contex = sstdNew<QOpenGLContext>();
            mmm_Contex->setFormat(sstd::getDefaultOpenGLFormat());
            mmm_Contex->create();
        }

        /*make current in this thread*/
        mmm_Contex->makeCurrent(this);
        glewInitialize();
        /*************************************************/
        auto varRender = makeRender();
        /*************************************************/

    }

    std::shared_ptr<sstd::RenderPack> Window::makeRender() {
    start_pos:
        /*if render is make ....*/
        if (mmm_RenderPack) {
            /*万一用于运行时重装显卡驱动 ... */
            mmm_RenderPack->targetWindowDevicePixelRatio = this->devicePixelRatio();

            return mmm_RenderPack;
        }
        /*create the render ... */
        {
            /*this function should be run in main thread*/
            auto varPack = sstd::make_shared<sstd::RenderPack>();
            mmm_RenderPack = varPack;
            /*copy this data */
            varPack->targetWindow = this;
            varPack->targetWindowState = this->mmm_Mutex;
            varPack->targetWindowContex = this->mmm_Contex;
            /*create offscreen surface*/
            varPack->sourceOffscreenSurface = sstd::make_unique<QOffscreenSurface>();
            varPack->sourceOffscreenSurface->setFormat(sstd::getDefaultOpenGLFormat());
            varPack->sourceOffscreenSurface->create();
            /*make render control*/
            varPack->sourceViewControl = sstd::make_unique<QQuickRenderControl>();
            /*create engine*/
            varPack->sourceQQmlEngine = sstd::make_unique<QQmlEngine>();
            varPack->sourceView = sstd::make_unique<QQuickWindow>(varPack->sourceViewControl.get());
            if (!varPack->sourceQQmlEngine->incubationController()) {
                varPack->sourceQQmlEngine->setIncubationController(varPack->sourceView->incubationController());
            }
            /*create data in another thread*/
            auto varRenderThread = sstdNew<sstd::RenderThread>(varPack);
            connect(this, &Window::ppp_QuitRender, varRenderThread, &sstd::RenderThread::quitRender, Qt::DirectConnection);
            varPack->sourceViewControl->prepareThread(varRenderThread);
            varRenderThread->start();
            while (mmm_Mutex->renderCount() == 0) {
                std::this_thread::sleep_for(100ns);
            }
            /*add signal and slot*/
            auto varRenderControl = varPack->sourceViewControl.get();
            connect(varRenderControl, &QQuickRenderControl::renderRequested,
                this, &Window::ppp_RenderRequested, Qt::QueuedConnection);
            connect(varRenderControl, &QQuickRenderControl::sceneChanged,
                this, &Window::ppp_SceneChanged, Qt::QueuedConnection);
            varPack->renderThread = varRenderThread;
        }

        goto start_pos;
    }

    void Window::ppp_RenderRequested() {
        assert(QThread::currentThread() == thread());
        ppp_Init();
        mmm_RenderPack->renderThread->callInThisThread([renderPack = mmm_RenderPack]() {
            auto varRenderControl = renderPack->sourceViewControl.get();
            varRenderControl->render();
            /**/
        });
    }

    void Window::ppp_SceneChanged() {
        assert(QThread::currentThread() == thread());
        ppp_Init();
        auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
        /*polish items in main thread ... */
        varRenderControl->polishItems();
        /*call sync and wait for finished ... */
        mmm_RenderPack->renderThread->callInThisThread([renderPack = mmm_RenderPack]() {
            renderPack->sourceContex->makeCurrent(renderPack->sourceOffscreenSurface.get());
            /*check and set fbo ... */

            /*ask a sync ....*/
            auto varRenderControl = renderPack->sourceViewControl.get();
            varRenderControl->sync();
        }).wait();
        /*ask a render ... */
        ppp_RenderRequested();
    }

} /*namespace sstd*/












