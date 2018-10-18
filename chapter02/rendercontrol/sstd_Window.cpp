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

        this->setMinimumHeight(128);
        this->setMinimumWidth(128);

        /*标记为OpenGL渲染环境*/
        this->setSurfaceType(QSurface::OpenGLSurface);
        this->setFormat(sstd::getDefaultOpenGLFormat());

        mmm_RenderThread = sstdNew<sstd::QuickThread>();

    }

    Window::~Window() {
                 

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
            varPack->sourceViewControl->prepareThread(mmm_RenderThread);
            mmm_RenderThread->runInThisThread([varPack]() {
                /*create opengl contex in this thread ...*/
                varPack->sourceContex = sstd::make_unique<QOpenGLContext>();
                varPack->sourceContex->setFormat(sstd::getDefaultOpenGLFormat());
                varPack->sourceContex->create();
                varPack->sourceContex->setShareContext(varPack->targetWindowContex);
                varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
                glewInitialize();
            })->data()->wait();
            /*add signal and slot*/
            auto varRenderControl = varPack->sourceViewControl.get();
            connect(varRenderControl, &QQuickRenderControl::renderRequested,
                this, &Window::ppp_RenderRequested, Qt::QueuedConnection);
            connect(varRenderControl, &QQuickRenderControl::sceneChanged,
                this, &Window::ppp_SceneChanged, Qt::QueuedConnection);
            varPack->renderThread = mmm_RenderThread;
        }

        goto start_pos;
    }

    void Window::ppp_RenderRequested() {
        assert(QThread::currentThread() == thread());
        ppp_Init();
        mmm_RenderPack->renderThread->runInThisThread(
            [renderPack = mmm_RenderPack]() {
            auto varRenderControl = renderPack->sourceViewControl.get();
            varRenderControl->render();
            /**/});
    }

    void Window::ppp_SceneChanged() {
        assert(QThread::currentThread() == thread());
        ppp_Init();
        auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
        /*polish items in main thread ... */
        varRenderControl->polishItems();
        /*call sync and wait for finished ... */
        auto varFutures = mmm_RenderPack->renderThread->runInThisThread(
            [renderPack = mmm_RenderPack]() {
            renderPack->sourceContex->makeCurrent(renderPack->sourceOffscreenSurface.get());
            /*check and set fbo ... */

            /*ask a sync ....*/
            auto varRenderControl = renderPack->sourceViewControl.get();
            varRenderControl->sync();
        }, [renderPack = mmm_RenderPack]() {
            /*call render do not need wait*/
            auto varRenderControl = renderPack->sourceViewControl.get();
            varRenderControl->render();
        });
        if (false == bool(varFutures)) {
            return;
        }
        (*varFutures)[0].wait();
    }

} /*namespace sstd*/












