#include <sstd_glew.hpp>
#include "sstd_Window.hpp"
#include <ConstructQSurface.hpp>
#include <QtQuick/qquickview.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQml/qqmlengine.h>

namespace {

    using RPT = std::shared_ptr<sstd::RenderPack>;

    class UpdataSourceContex {
        RPT mmm_RenderPack;
    public:

        UpdataSourceContex(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() {
            mmm_RenderPack->sourceContex->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());
        }

    };

    class PolishSourceWindow {
        RPT mmm_RenderPack;
    public:
        PolishSourceWindow(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() {
            auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
            varRenderControl->polishItems();
        }

    };

    class SyncSourceQSGDAta {
        RPT mmm_RenderPack;
    public:
        SyncSourceQSGDAta(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() {
            auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
            varRenderControl->sync();
        }

    };

    class RenderSourceWindow {
        RPT mmm_RenderPack;
    public:
        RenderSourceWindow(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() {
            auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
            varRenderControl->render();
            glFinish();
        }

    };

    class DrawOnTarget {
        RPT mmm_RenderPack;
    public:
        DrawOnTarget(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() {
             
        }

    };


}/*namesapce*/

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

        if (mmm_RenderPack) {/*destory data in thread ...*/
            auto varFutures = mmm_RenderThread->runInThisThread([varPack = mmm_RenderPack]() {
                varPack->renderThread->setLogicalQuit(true);
                /*the thread will delete it self */
                varPack->renderThread->quit();
                /*delete contex in this thread ...*/
                varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
                glFinish();
                varPack->sourceContex->doneCurrent();
                varPack->sourceContex.reset();
            });
            varFutures->data()->wait();
            mmm_RenderPack->sourceView.reset();
            mmm_RenderPack->sourceQQmlEngine.reset();
            mmm_RenderPack->sourceViewControl.reset();
            mmm_RenderPack->sourceOffscreenSurface.reset();
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
            /*考虑到运行时重装显卡驱动等情况，这个值还是每次更新 ... */
            mmm_RenderPack->targetWindowDevicePixelRatio = this->devicePixelRatio();
            /*更新窗口大小*/
            mmm_RenderPack->targetWindowWidth = this->width();
            mmm_RenderPack->targetWindowHeight = this->height();
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
            {
                auto varFutures = mmm_RenderThread->runInThisThread([varPack]() {
                    /*create opengl contex in this thread ...*/
                    varPack->sourceContex = sstd::make_unique<QOpenGLContext>();
                    varPack->sourceContex->setFormat(sstd::getDefaultOpenGLFormat());
                    varPack->sourceContex->create();
                    varPack->sourceContex->setShareContext(varPack->targetWindowContex);
                    varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
                    glewInitialize();
                });
                /*wait for init finished ... */
                varFutures->data()->wait();
            }
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

        /*ask update source render contex ... */
        auto varRenderThread = mmm_RenderPack->renderThread;
        varRenderThread->runInThisThread(UpdataSourceContex{ mmm_RenderPack });

        /*polish source window data ... */
        {
            PolishSourceWindow varPolish{mmm_RenderPack};
            varPolish();
        }

        /*sync render and draw in target ... */
        std::tuple<
            SyncSourceQSGDAta, 
            RenderSourceWindow , 
            DrawOnTarget > varDrawSteps{ mmm_RenderPack , mmm_RenderPack,mmm_RenderPack };

        auto varFutures = varRenderThread->applyInThisThread(std::move(varDrawSteps));
        if (varFutures) {
            /*sync need block main thread ...*/
            varFutures->data()->wait();
        }

    }

} /*namespace sstd*/












