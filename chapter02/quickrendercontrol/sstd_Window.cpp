#include <sstd_glew.hpp>
#include <ConstructQSurface.hpp>
#include <sstd_memory.hpp>
#include <sstd_RenderPack.hpp>
#include <QtGui/qopenglframebufferobject.h>
#include "sstd_Window.hpp"

namespace {

    class RenderControl : public QQuickRenderControl {
    public:

        RenderControl(sstd::Window *w) : mmm_Window(w) {
        }

        QWindow *renderWindow(QPoint *offset) override {
            if (offset) {
                *offset = { 0,0 };
            }
            return mmm_Window;
        }

    private:
        sstd::Window * const mmm_Window;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(RenderControl)
    };

}/*namesapce*/

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

namespace {

    class ThisRenderPack : public sstd::RenderPack {
    public:
        GLuint targetVAO{ 0 };
        GLuint targetVAOBuffer{ 0 };
        GLuint targetVAOIndexBuffer{ 0 };
        GLuint targetProgram{ 0 };
        std::unique_ptr<QOpenGLFramebufferObject> sourceFrameBufferObject;
    private:
        SSTD_MEMORY_DEFINE(ThisRenderPack)
    };

    inline std::shared_ptr<sstd::RenderPack> makeRenderPack() {
        auto varAns = sstd::make_shared<ThisRenderPack>();
        varAns->renderThread = sstdNew<sstd::QuickThread>();
        return varAns;
    }

    template<typename std_shared_ptr_sstd_renderpack>
    inline ThisRenderPack * get(const std_shared_ptr_sstd_renderpack & arg) {
        return static_cast<ThisRenderPack *>(arg.get());
    }

    template<typename std_shared_ptr_sstd_renderpack>
    inline QSize getSize(const std_shared_ptr_sstd_renderpack & arg) {
        auto varPack = get(arg);
        const auto varPR = varPack->targetWindowDevicePixelRatio.load();
        const auto varW = varPack->targetWindowWidth.load();
        const auto varH = varPack->targetWindowHeight.load();
        return{ static_cast<int>(varW*varPR),static_cast<int>(varH*varPR) };
    }

    class FunctionBasic {
    protected:
        std::shared_ptr<ThisRenderPack> mmm_RenderPack;
    public:
        FunctionBasic(const std::shared_ptr<sstd::RenderPack> & arg) :
            mmm_RenderPack(std::static_pointer_cast<ThisRenderPack>(arg)) {
        }
        FunctionBasic(std::shared_ptr<sstd::RenderPack> && arg) :
            mmm_RenderPack(std::static_pointer_cast<ThisRenderPack>(std::move(arg))) {
        }
    private:
        SSTD_MEMORY_DEFINE(FunctionBasic)
    };

    class ConstructDrawTargetContext final : public FunctionBasic {
        void ppp_init(QWindow *w, QOpenGLContext *share) const {
            mmm_RenderPack->targetContex = sstd::make_unique<QOpenGLContext>();
            mmm_RenderPack->targetContex->setShareContext(share);
            mmm_RenderPack->targetContex->setFormat(w->requestedFormat());
            mmm_RenderPack->targetContex->create();
            if (!mmm_RenderPack->targetContex->makeCurrent(w)) {
                return;
            }
            glewInitialize();
        }
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {

            if (!mmm_RenderPack->targetContex) {
                ppp_init(mmm_RenderPack->targetWindow, mmm_RenderPack->sourceContex.get());
            }

            if (!mmm_RenderPack->targetContex->makeCurrent(mmm_RenderPack->targetWindow)) {
                return;
            }
        }
    };

    class ConstructDrawTargetWindowOpenGLProgram final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;

        inline static GLuint buildVFShader(std::string_view varVShaderSource, std::string_view varFShaderSource) {

            class ShaderFree {
            public:
                std::array<GLuint, 2> data;
                inline ShaderFree() {
                    data[0] = 0;
                    data[1] = 0;
                }
                inline ~ShaderFree() {
                    glDeleteShader(data[1]);
                    glDeleteShader(data[0]);
                }
            } varShaders{};

            GLuint * varShader = &(varShaders.data[0]);
            varShader[0] = glCreateShader(GL_VERTEX_SHADER);
            varShader[1] = glCreateShader(GL_FRAGMENT_SHADER);

            {
                const GLchar * sources[] = { varVShaderSource.data(),varFShaderSource.data() };
                GLint lengths[] = {
                    (GLint)(varVShaderSource.size()),
                    (GLint)(varFShaderSource.size())
                };

                glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
                glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

                glCompileShader(varShader[0]);
                glCompileShader(varShader[1]);

            }

            GLuint varProgram = glCreateProgram();
            glAttachShader(varProgram, varShader[0]);
            glAttachShader(varProgram, varShader[1]);
            glLinkProgram(varProgram);

            if constexpr (false) {

                auto printProgramInfo = [](GLuint e) {
                    /*获得错误大小*/
                    GLint log_length = 0;
                    glGetProgramiv(e, GL_INFO_LOG_LENGTH, &log_length);
                    log_length += 16;

                    /*获得一段内存，并初始化为0*/
                    sstd::string infos_;
                    infos_.resize(log_length);

                    /*获得错误并输出*/
                    char * info = infos_.data();
                    glGetProgramInfoLog(e, 1024, nullptr, info);
                    qDebug() << info;
                };

                printProgramInfo(varProgram);
            }

            return varProgram;

        }

        void operator()() const {
            if (mmm_RenderPack->targetProgram) {
                return;
            }

            mmm_RenderPack->targetProgram = buildVFShader(
                u8R"(
/*简单顶点着色器，用于渲染一个图片*/
#version 450

layout( location = 0 ) in vec4 argPosition  ;
layout( location = 1 ) in vec4 argTexturePos;
out vec4 ioTexturePos ;

void main(){
    ioTexturePos = argTexturePos ;
    gl_Position = argPosition    ;
}

)"sv,
u8R"(

/*简单片段着色器,用于渲染一个图像*/
#version 450

in  vec4 ioTexturePos                           ;
out vec4 outColor                               ;
layout (binding=1) uniform sampler2D argTexture ;

void main(){
    outColor = texture2D( argTexture , ioTexturePos.xy ) ;
}

)"sv);

        }
    };

    class ConstructDrawTargetWindowVAO  final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            auto varPack = mmm_RenderPack.get();

            if (varPack->targetVAO) {
                return;
            }

            glCreateVertexArrays(1, &(varPack->targetVAO));
            glBindVertexArray(varPack->targetVAO);
            glCreateBuffers(1, &(varPack->targetVAOBuffer));
            glCreateBuffers(1, &(varPack->targetVAOIndexBuffer));
            class DataRow {
            public:
                GLfloat x, y, z, w;
                GLfloat s, t, p, q;
            };
            constexpr const static std::array<DataRow, 4> varVAOData{ DataRow {-1,1,0,1,/**/0,1,0,1},
            DataRow {-1,-1,0,1,/**/0,0,0,1},
            DataRow {1,-1,0,1,/**/1,0,0,1},
            DataRow {1,1,0,1,/**/1,1,0,1}
            };
            constexpr const static std::array<std::uint16_t, 6> varVAOIndex{
                3,2,1,
                3,1,0
            };
            glNamedBufferData(varPack->targetVAOBuffer, sizeof(varVAOData), varVAOData.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexArrayVertexBuffer(varPack->targetVAO, 0, varPack->targetVAOBuffer, 0, sizeof(DataRow));
            glVertexArrayAttribFormat(varPack->targetVAO, 0, 4, GL_FLOAT, false, 0);
            glVertexArrayAttribBinding(varPack->targetVAO, 0, 0);

            glEnableVertexAttribArray(1);
            glVertexArrayVertexBuffer(varPack->targetVAO, 1, varPack->targetVAOBuffer, (sizeof(DataRow) >> 1), sizeof(DataRow));
            glVertexArrayAttribFormat(varPack->targetVAO, 1, 4, GL_FLOAT, false, 0);
            glVertexArrayAttribBinding(varPack->targetVAO, 1, 1);

            glNamedBufferData(varPack->targetVAOIndexBuffer, sizeof(varVAOIndex), varVAOIndex.data(), GL_STATIC_DRAW);
            glVertexArrayElementBuffer(varPack->targetVAO, varPack->targetVAOIndexBuffer);

        }
    };

    class DrawTargetWindow  final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            auto varPack = mmm_RenderPack.get();

            if (varPack->isClose.load()) {
                return;
            }

            const auto varGLWindowSize = getSize(mmm_RenderPack);
            glViewport(0, 0, varGLWindowSize.width(), varGLWindowSize.height());
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(varPack->targetProgram);
            glBindVertexArray(varPack->targetVAO);
            glActiveTexture(GL_TEXTURE0);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, varPack->sourceFrameBufferObject->texture());
            glBindTextureUnit(1, varPack->sourceFrameBufferObject->texture());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
            glFinish();

        }
    };

    class DrawTargetSwapBuffers final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            mmm_RenderPack->targetContex->swapBuffers(mmm_RenderPack->targetWindow);
        }
    };

    class ConstructDrawTargetFBO final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {

            const auto varTargetSize = getSize(mmm_RenderPack);
            auto varPack = mmm_RenderPack.get();

            do {

                if (false == bool(varPack->sourceFrameBufferObject)) {
                    break;
                }

                if (varPack->sourceFrameBufferObject->size() != varTargetSize) {
                    break;
                }

                return;

            } while (false);


            varPack->sourceFrameBufferObject = sstd::make_unique<QOpenGLFramebufferObject>(varTargetSize,
                QOpenGLFramebufferObject::CombinedDepthStencil);
            varPack->sourceView->setRenderTarget(varPack->sourceFrameBufferObject.get());

        }
    };

    class ConstructRenderPack final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            auto varPack = get(mmm_RenderPack);

            varPack->sourceContex = sstd::make_unique<QOpenGLContext>();
            varPack->sourceContex->setFormat(sstd::getDefaultOpenGLFormat());
            varPack->sourceContex->create();

            varPack->sourceOffscreenSurface = sstd::make_unique< QOffscreenSurface>();
            varPack->sourceOffscreenSurface->setFormat(sstd::getDefaultOpenGLFormat());
            varPack->sourceOffscreenSurface->create();

            varPack->sourceViewControl = sstd::make_unique<RenderControl>(static_cast<sstd::Window *>(varPack->targetWindow));
            varPack->sourceView = sstd::make_unique<QQuickWindow>(varPack->sourceViewControl.get());

            varPack->sourceQQmlEngine = sstd::make_unique<QQmlEngine>();
            if (!varPack->sourceQQmlEngine->incubationController()) {
                varPack->sourceQQmlEngine->setIncubationController(varPack->sourceView->incubationController());
            }

            varPack->sourceViewControl->prepareThread(varPack->renderThread);
            varPack->sourceContex->moveToThread(varPack->renderThread);

        }
    };

    class DrawSourcePolishItems : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            mmm_RenderPack->sourceViewControl->polishItems();
        }
    };

    class DrawSourceMakeCurrent final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            mmm_RenderPack->sourceContex->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());
        }
    };

    class DrawSourceSync final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            mmm_RenderPack->sourceViewControl->sync();
        }
    };

    class DrawSourceRender final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            mmm_RenderPack->sourceViewControl->render();
            glFlush();
        }
    };

    class DestoryRenderPack final : public FunctionBasic {
    public:
        using FunctionBasic::FunctionBasic;
        void operator()() const {
            auto varPack = get(mmm_RenderPack);
            varPack->renderThread->setLogicalQuit(true);

            if (varPack->targetContex) {
                varPack->targetContex->makeCurrent(varPack->targetWindow);
                glDeleteProgram(varPack->targetProgram);
                glDeleteVertexArrays(1, &(varPack->targetVAO));
                glDeleteBuffers(1, &(varPack->targetVAOBuffer));
                glDeleteBuffers(1, &(varPack->targetVAOIndexBuffer));
                varPack->targetContex->doneCurrent();
                varPack->targetContex.reset();
            }

            if (varPack->sourceContex) {
                varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
                varPack->sourceViewControl->invalidate();
                varPack->sourceFrameBufferObject.reset();
                varPack->sourceContex->doneCurrent();
                varPack->sourceContex->moveToThread(qApp->thread());
            }

            /*the thread will delete it self ...*/
            varPack->renderThread->quit();
        }
    };

}/*namespace*/

sstd::Window::Window() :
    mmm_QuickInitialized(false) {
    setSurfaceType(QSurface::OpenGLSurface);

    mmm_RenderPack = makeRenderPack();
    mmm_RenderPack->targetWindow = this;

    setFormat(sstd::getDefaultOpenGLFormat());

    ConstructRenderPack constructRenderPack{ mmm_RenderPack };
    constructRenderPack();

    // Now hook up the signals. For simplicy we don't differentiate
    // between renderRequested (only render is needed, no sync) and
    // sceneChanged (polish and sync is needed too).
    connect(mmm_RenderPack->sourceViewControl.get(), &QQuickRenderControl::renderRequested, this, &sstd::Window::justRender);
    connect(mmm_RenderPack->sourceViewControl.get(), &QQuickRenderControl::sceneChanged, this, &sstd::Window::requestUpdate);
}

sstd::Window::~Window() {
    mmm_RenderPack->isClose.store(true);
    mmm_RenderPack->renderThread->runInThisThread(DestoryRenderPack{ mmm_RenderPack })
        ->data()->wait();
    mmm_RenderPack->sourceContex.reset();
    mmm_RenderPack->sourceViewControl.reset();
    mmm_RenderPack->sourceView.reset();
}

bool sstd::Window::isResizing() const {
    const auto varCurrentTime = std::chrono::steady_clock::now();
    return (std::chrono::abs(varCurrentTime - mmm_RenderPack->lastResizeTime.load() ) < 500ms);
}

void sstd::Window::requestUpdate() {
    if (mmm_QuickInitialized) {

        if (isResizing()) {
            sstd::runInMainThread([this]() {
                polishSyncAndRenderResize();
            });
            return;
        }

        sstd::runInMainThread([this]() {
            polishSyncAndRender();
        });
    }
}

void sstd::Window::justRender() {
    if (mmm_QuickInitialized) {

        if (isResizing()) {
            sstd::runInMainThread([this]() {
                polishSyncAndRenderResize();
            });
            return;
        }

        sstd::runInMainThread([this]() {
            ppp_PolishSyncAndRender<false, false, false>();
        });
    }
}

bool sstd::Window::event(QEvent *e) {
    if (e->type() == QEvent::Close) {
        mmm_RenderPack->isClose = true;
    }
    return QWindow::event(e);
}

template<bool needPolish, bool needSync, bool isResize>
void sstd::Window::ppp_PolishSyncAndRender() {
    assert(QThread::currentThread() == thread());

    auto varRenderThread = mmm_RenderPack->renderThread;
    if constexpr (needPolish) {
        std::tuple< DrawSourcePolishItems > varPolishItems{ mmm_RenderPack };
        varRenderThread->applyHere(std::move(varPolishItems));
    }

    if constexpr (needSync) {
        std::tuple<
            DrawSourceMakeCurrent,
            ConstructDrawTargetFBO,
            DrawSourceSync,
            DrawSourceRender,
            ConstructDrawTargetContext,
            ConstructDrawTargetWindowOpenGLProgram,
            ConstructDrawTargetWindowVAO,
            DrawTargetWindow,
            DrawTargetSwapBuffers
        > varDrawCommands{
            /*00*/mmm_RenderPack,
            /*01*/mmm_RenderPack,
            /*02*/mmm_RenderPack,
            /*03*/mmm_RenderPack,
            /*04*/mmm_RenderPack,
            /*05*/mmm_RenderPack,
            /*06*/mmm_RenderPack,
            /*07*/mmm_RenderPack,
            /*08*/mmm_RenderPack
        };
        auto varFutrues = varRenderThread->applyInThisThread(std::move(varDrawCommands));
        if (varFutrues) {
            varFutrues->data()[(isResize == false) ? 2 : 8].wait();
        }

    } else {
        std::tuple<
            DrawSourceMakeCurrent,
            ConstructDrawTargetFBO,
            DrawSourceRender,
            ConstructDrawTargetContext,
            ConstructDrawTargetWindowOpenGLProgram,
            ConstructDrawTargetWindowVAO,
            DrawTargetWindow,
            DrawTargetSwapBuffers
        > varDrawCommands{
            mmm_RenderPack,
            mmm_RenderPack,
            mmm_RenderPack,
            mmm_RenderPack,
            mmm_RenderPack,
            mmm_RenderPack,
            mmm_RenderPack,
            mmm_RenderPack
        };
        varRenderThread->applyInThisThread(std::move(varDrawCommands));
    }

}

void sstd::Window::polishSyncAndRender() {
    ppp_PolishSyncAndRender<true, true, false>();
}

void sstd::Window::polishSyncAndRenderResize() {
    ppp_PolishSyncAndRender<true, true, true>();
}

void sstd::Window::updateSizes() {
    mmm_RenderPack->targetWindowDevicePixelRatio = this->devicePixelRatio();
    mmm_RenderPack->targetWindowHeight = this->height();
    mmm_RenderPack->targetWindowWidth = this->width();

    /*Behave like SizeRootObjectToView.*/
    mmm_RenderPack->sourceRootItem->setWidth(width());
    mmm_RenderPack->sourceRootItem->setHeight(height());

    mmm_RenderPack->sourceView->setGeometry(0, 0, width(), height());
}

void sstd::Window::startQuick(const QUrl &filename) {

    auto m_qmlComponent =
        sstd::sstdNew< QQmlComponent>(mmm_RenderPack->sourceQQmlEngine.get(), filename);
    m_qmlComponent->deleteLater();

    if (m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

    QObject * rootObject = m_qmlComponent->create();

    mmm_RenderPack->sourceRootItem = qobject_cast<QQuickItem *>(rootObject);
    if (!mmm_RenderPack->sourceRootItem) {
        qWarning("run: Not a QQuickItem");
        assert(false);
        return;
    }

    // The root item is ready. Associate it with the window.
    mmm_RenderPack->sourceRootItem->setParentItem(mmm_RenderPack->sourceView->contentItem());
    mmm_RenderPack->sourceRootItem->setParent(mmm_RenderPack->sourceView->contentItem());

    // Update item and rendering related geometries.
    updateSizes();

    mmm_QuickInitialized = true;

    /*Initialize the render thread and perform the first polish/sync/render.*/
    mmm_RenderPack->renderThread->runInThisThread([varPack = mmm_RenderPack]() {
        varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
        varPack->sourceViewControl->initialize(varPack->sourceContex.get());
    })->data()->wait();

    polishSyncAndRenderResize();

}

void sstd::Window::exposeEvent(QExposeEvent *) {
    if (isExposed()) {
        if (!mmm_QuickInitialized) {
            startQuick(QStringLiteral("myqml/quickrendercontrol/main.qml"));
        }
    }
}

void sstd::Window::resizeEvent(QResizeEvent *) {
    mmm_RenderPack->lastResizeTime = std::chrono::steady_clock::now();
    if (mmm_RenderPack->sourceRootItem) {
        updateSizes();
        polishSyncAndRenderResize();
    }
}

void sstd::Window::mousePressEvent(QMouseEvent *e) {
    QMouseEvent varMappedEvent(
        e->type(), e->localPos(),
        e->screenPos(), e->button(),
        e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(mmm_RenderPack->sourceView.get(), &varMappedEvent);
}

void sstd::Window::mouseReleaseEvent(QMouseEvent *e) {
    QMouseEvent varMappedEvent(
        e->type(), e->localPos(),
        e->screenPos(), e->button(),
        e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(mmm_RenderPack->sourceView.get(), &varMappedEvent);
}








