#include <sstd_glew.hpp>
#include "sstd_Window.hpp"
#include <QtCore/qpointer.h>
#include <QtCore/qdebug.h>
#include <quick/sstd_QThread.hpp>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qopenglcontext.h>
#include <ConstructQSurface.hpp>
#include <QtGui/qoffscreensurface.h>
#include <QtGui/qopenglframebufferobject.h>
#include <QtQuick/qquickitem.h>
#include <QtQml/qqmlengine.h>

namespace sstd {

    Window::Window() {
        mmm_Thread = sstdNew<sstd::QuickThread>();
        connect(this, &Window::startRender, this, &Window::ppp_start_render);
    }

    Window::~Window() {
        mmm_Thread->runInThisThread([this]() {
            mmm_Thread->setLogicalQuit(true);
            mmm_Thread->quit();
        });
    }

    void Window::ppp_RenderFinished(const QImage &arg) {
        sstd::runInMainThread([thisp = QPointer(this), var = arg]() {
            auto varThis = thisp.data();
            if (varThis == nullptr) {
                return;
            }
            varThis->renderFinished(var);
        });
    }

    namespace {
        class RenderView : public QQuickWindow {
            using Super = QQuickWindow;
            QQmlEngine mmm_Engine;
            std::unique_ptr< QQuickItem > mmm_RootItem;
        public:

            RenderView(QQuickRenderControl * arg) :Super(arg) {
                if (!mmm_Engine.incubationController()) {
                    mmm_Engine.setIncubationController(this->incubationController());
                }
            }

            void setSource(const QUrl & arg) noexcept(false) {
                auto varComponent = sstd::sstdNew<QQmlComponent>(&mmm_Engine, arg);
                varComponent->deleteLater();
                if (varComponent->status() != QQmlComponent::Ready) {
                    if (varComponent->status() == QQmlComponent::Error) {
                        QString varErrorString;
                        for (const auto & varE : varComponent->errors()) {
                            varErrorString += varE.toString();
                            varErrorString += QChar('\n');
                        }
                        throw varErrorString;
                    }
                    throw QStringLiteral("can not load qml sync...");
                }
                auto varItemRaw = varComponent->create();
                auto varItem = qobject_cast<QQuickItem *>(varItemRaw);
                if (varItem == nullptr) {
                    if (varItemRaw) {
                        varItemRaw->deleteLater();
                    }
                    throw QStringLiteral("can not render ...");
                }
                const auto varWidth = varItem->width();
                const auto varHeight = varItem->height();
                if (varWidth < 1) {
                    if (varItemRaw) {
                        varItemRaw->deleteLater();
                    }
                    throw QStringLiteral("empty size");
                }
                if (varHeight < 1) {
                    if (varItemRaw) {
                        varItemRaw->deleteLater();
                    }
                    throw QStringLiteral("empty size");
                }
                this->setGeometry(0, 0, varWidth, varHeight);
                varItem->setParentItem(this->contentItem());
                varItem->setParent(this->contentItem());
            }

            ~RenderView() {
            }

        private:
            SSTD_MEMORY_QOBJECT_DEFINE(RenderView)
        };
    }/*namespace*/

    namespace {
        class RenderControl : public QQuickRenderControl {
            sstd::Window * const mmm_Window;
        public:

            RenderControl(sstd::Window * arg) :mmm_Window(arg) {

            }

            QWindow * renderWindow(QPoint *offset) override {
                if (offset) {
                    *offset = { 0,0 };
                }
                return mmm_Window;
            }

        private:
            SSTD_MEMORY_QOBJECT_DEFINE(RenderControl)
        };
    }/*namespace*/

    namespace {
        class ThisRenderPack {
        public:
            Window * const targetWindow;
            std::unique_ptr< RenderView > sourceWindow;
            std::unique_ptr< RenderControl > sourceControl;
            std::unique_ptr< QOpenGLContext > sourceContex;
            std::unique_ptr< QOpenGLFramebufferObject > sourceFBO;
            std::unique_ptr< QOffscreenSurface > sourceContexSurface;
            sstd::QuickThread * renderThread{ nullptr };
            std::atomic_bool hasError{ false };
        public:
            ThisRenderPack(Window * arg) :targetWindow(arg) {
            }

        private:
            SSTD_MEMORY_DEFINE(ThisRenderPack)
        };

        std::shared_ptr<ThisRenderPack> createRender(Window * window, const QString & path) {
            auto varAns = sstd::make_shared<ThisRenderPack>(window);
            /*set render thread ...*/
            varAns->renderThread = window->getRenderThread();
            /*create opengl contex ...*/
            varAns->sourceContex = sstd::make_unique<QOpenGLContext>();
            varAns->sourceContex->setFormat(sstd::getDefaultOpenGLFormat());
            varAns->sourceContex->create();
            /*create render control ...*/
            varAns->sourceControl = sstd::make_unique<RenderControl>(window);
            /*create render window ...*/
            varAns->sourceWindow = sstd::make_unique<RenderView>(varAns->sourceControl.get());
            /*try load data ...*/
            varAns->sourceWindow->setSource(QUrl::fromLocalFile(path));
            /*set render thread ...*/
            varAns->sourceControl->prepareThread(varAns->renderThread);
            varAns->sourceContex->moveToThread(varAns->renderThread);
            /*create tmp offscreensurface to openglcontex ...*/
            varAns->sourceContexSurface = sstd::make_unique<QOffscreenSurface>();
            return std::move(varAns);
        }

    }/*namespace */

    namespace {
        class ErrorRender {
            const QString mmm_ErrorString;
        public:
            ErrorRender(const QString &arg) :mmm_ErrorString(arg) {
            }
            QImage getErrorImage() const {
                return {};
            }
        };
    }/*namespace*/

#define BEGIN_TRY do {  if (varRenderPack->hasError.load()) { return; } \
    QImage varErrorImage ; \
    try { assert(true)/*begin try ... */

#define END_TRY } catch (const QString & error) { \
    varRenderPack->hasError.store(true); \
    qDebug() << error; \
    ErrorRender varError{ error }; \
    varErrorImage = varError.getErrorImage(); \
    ppp_RenderFinished(varErrorImage); \
    return; \
    } catch (...) { \
        varRenderPack->hasError.store(true); \
        qDebug() << QStringLiteral("unknow error!"); \
        ErrorRender varError{ QStringLiteral("unknow error!") }; \
        varErrorImage = varError.getErrorImage(); \
        ppp_RenderFinished(varErrorImage); \
        return; \
    } \
} while(false) \
/*end try ... */

    void Window::ppp_start_render(const QString & varQmlPath) {

        std::shared_ptr<ThisRenderPack> varRenderPack;

        BEGIN_TRY;
        varRenderPack = createRender(this, varQmlPath);
        END_TRY;

        /*init in another thread ...*/
        mmm_Thread->runInThisThread([this, varRenderPack]() {
            BEGIN_TRY;

            if (false == varRenderPack->sourceContex->makeCurrent(varRenderPack->sourceContexSurface.get())) {
                throw QStringLiteral("can not make current ...");
            }

            if (false == glewInitialize()) {
                throw QStringLiteral("can not init glew ...");
            }

            const QSize varRenderSize =
                varRenderPack->sourceWindow->size();
            /*create fbo ...*/
            varRenderPack->sourceFBO = sstd::make_unique<QOpenGLFramebufferObject>(varRenderSize,
                QOpenGLFramebufferObject::CombinedDepthStencil);
            varRenderPack->sourceWindow->setRenderTarget(varRenderPack->sourceFBO.get());
            /*init render control ...*/
            varRenderPack->sourceControl->initialize(varRenderPack->sourceContex.get());
            END_TRY;
        })->data()->wait();

        /*polish in main thread ...*/
        BEGIN_TRY;
        varRenderPack->sourceControl->polishItems();
        END_TRY;

        /*render in another thread...*/
        mmm_Thread->runInThisThread([this, varRenderPack]() {
            /*call sync ...*/
            BEGIN_TRY;
            varRenderPack->sourceControl->sync();
            END_TRY;
        }, [this, varRenderPack]() {
            /*call render ...*/
            BEGIN_TRY;
            varRenderPack->sourceControl->render();
            END_TRY;
        }, [this, varRenderPack]() {
            /*wait for finshied ...*/
            BEGIN_TRY;
            glFinish();
            END_TRY;
        }, [this, varRenderPack]() {
            /*get image ...*/
            BEGIN_TRY;
            const auto varTexture = varRenderPack->sourceFBO->texture();
            glBindTexture(GL_TEXTURE_2D, varTexture);

            END_TRY;
        }, [this, varRenderPack]() {
            /*destory data in this thread ...*/
            BEGIN_TRY;
            glFinish();
            varRenderPack->sourceFBO.reset();
            varRenderPack->sourceContex->aboutToBeDestroyed();
            varRenderPack->sourceContex->doneCurrent();
            varRenderPack->sourceContex->moveToThread(qApp->thread());
            varRenderPack->renderThread->runInMainThread([this, varRenderPack]() {
                /*destory data in main thread ...*/
                BEGIN_TRY;
                varRenderPack->sourceWindow.reset();
                varRenderPack->sourceContexSurface.reset();
                varRenderPack->sourceControl.reset();
                varRenderPack->sourceContex.reset();
                END_TRY;
            });
            END_TRY;
        })->data()->wait();

    }

}/****/

















