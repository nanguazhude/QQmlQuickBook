#include <sstd_glew.hpp>
#include <ConstructQSurface.hpp>

#include <sstd_memory.hpp>
#include <sstd_RenderPack.hpp>

#include "sstd_Window.hpp"
#include "cuberenderer.h"
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOffscreenSurface>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickRenderControl>
#include <QCoreApplication>

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

            const auto varGLWindowSize = getSize(mmm_RenderPack);
            glViewport(0, 0, varGLWindowSize.width(), varGLWindowSize.height());
            glClearColor(1, 0, 0, 1);
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

}/*namespace*/

CubeRenderer::CubeRenderer(std::shared_ptr<sstd::RenderPack> arg)
    :mmm_RenderPack(std::move(arg)) {
}

CubeRenderer::~CubeRenderer() {
    mmm_RenderPack->targetContex->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());


    mmm_RenderPack->targetContex->doneCurrent();
}

void CubeRenderer::init(QWindow *w, QOpenGLContext *share) {
    mmm_RenderPack->targetContex = sstd::make_unique<QOpenGLContext>();
    mmm_RenderPack->targetContex->setShareContext(share);
    mmm_RenderPack->targetContex->setFormat(w->requestedFormat());
    mmm_RenderPack->targetContex->create();
    if (!mmm_RenderPack->targetContex->makeCurrent(w)) {
        return;
    }
    glewInitialize();
}

void CubeRenderer::render(QWindow *w, QOpenGLContext *share) {

    if (!mmm_RenderPack->targetContex) {
        init(w, share);
    }

    if (!mmm_RenderPack->targetContex->makeCurrent(w)) {
        return;
    }

    ConstructDrawTargetWindowOpenGLProgram constructDrawTargetWindowOpenGLProgram{ mmm_RenderPack };
    constructDrawTargetWindowOpenGLProgram();

    ConstructDrawTargetWindowVAO constructDrawTargetWindowVAO{ mmm_RenderPack };
    constructDrawTargetWindowVAO();

    DrawTargetWindow drawTargetWindow{ mmm_RenderPack };
    drawTargetWindow();

    mmm_RenderPack->targetContex->swapBuffers(w);
}

static const QEvent::Type INIT = QEvent::Type(QEvent::User + 1);
static const QEvent::Type RENDER = QEvent::Type(QEvent::User + 2);
static const QEvent::Type RESIZE = QEvent::Type(QEvent::User + 3);
static const QEvent::Type STOP = QEvent::Type(QEvent::User + 4);

static const QEvent::Type UPDATE = QEvent::Type(QEvent::User + 5);

QuickRenderer::QuickRenderer()
    :
   // m_context(nullptr),
    //m_surface(nullptr),
    //m_window(nullptr),
    //    m_quickWindow(nullptr),
   // m_renderControl(nullptr),
    m_cubeRenderer(nullptr),
    m_quit(false) {
}

void QuickRenderer::requestInit() {
    QCoreApplication::postEvent(this, new QEvent(INIT));
}

void QuickRenderer::requestRender() {
    QCoreApplication::postEvent(this, new QEvent(RENDER));
}

void QuickRenderer::requestResize() {
    QCoreApplication::postEvent(this, new QEvent(RESIZE));
}

void QuickRenderer::requestStop() {
    QCoreApplication::postEvent(this, new QEvent(STOP));
}

bool QuickRenderer::event(QEvent *e) {
    QMutexLocker lock(&m_mutex);

    switch (int(e->type())) {
        case INIT:
            init();
            return true;
        case RENDER:
            render(&lock);
            return true;
        case RESIZE:
            
            return true;
        case STOP:
            cleanup();
            return true;
        default:
            return QObject::event(e);
    }
}

void QuickRenderer::init() {
    auto m_context = mmm_RenderPack->sourceContex.get();
    auto m_renderControl = mmm_RenderPack->sourceViewControl.get();
    
    m_context->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());

    // Pass our offscreen surface to the cube renderer just so that it will
    // have something is can make current during cleanup. QOffscreenSurface,
    // just like QWindow, must always be created on the gui thread (as it might
    // be backed by an actual QWindow).
    m_cubeRenderer = new CubeRenderer(mmm_RenderPack);
   

    m_renderControl->initialize(m_context);
}

void QuickRenderer::cleanup() {

    auto m_context = mmm_RenderPack->sourceContex.get();
    auto m_renderControl = mmm_RenderPack->sourceViewControl.get();

    m_context->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());

    m_renderControl->invalidate();

    //delete m_fbo;
    //m_fbo = nullptr;

    delete m_cubeRenderer;
    m_cubeRenderer = nullptr;

    m_context->doneCurrent();
    m_context->moveToThread(QCoreApplication::instance()->thread());

    m_cond.wakeOne();
}

void QuickRenderer::ensureFbo() {

    ConstructDrawTargetFBO constructDrawTargetFBO{ mmm_RenderPack };
    constructDrawTargetFBO();

}

void QuickRenderer::render(QMutexLocker *lock) {

    auto m_context = mmm_RenderPack->sourceContex.get();
    auto m_renderControl = mmm_RenderPack->sourceViewControl.get();
    auto m_window = mmm_RenderPack->targetWindow;

    Q_ASSERT(QThread::currentThread() != m_window->thread());

    if (!m_context->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get())) {
        qWarning("Failed to make context current on render thread");
        return;
    }

    ensureFbo();

    // Synchronization and rendering happens here on the render thread.
    m_renderControl->sync();

    // The gui thread can now continue.
    m_cond.wakeOne();
    lock->unlock();

    // Meanwhile on this thread continue with the actual rendering (into the FBO first).
    m_renderControl->render();
    glFlush();

    // The cube renderer uses its own context, no need to bother with the state here.

    // Get something onto the screen using our custom OpenGL engine.
    QMutexLocker quitLock(&m_quitMutex);
    if (!m_quit) {
        m_cubeRenderer->render(m_window, m_context);
    }
}

void QuickRenderer::aboutToQuit() {
    QMutexLocker lock(&m_quitMutex);
    m_quit = true;
}



sstd::Window::Window()
    :
    //m_qmlComponent(nullptr),
    //m_rootItem(nullptr),
    m_quickInitialized(false),
    m_psrRequested(false) {
    setSurfaceType(QSurface::OpenGLSurface);

    mmm_RenderPack = makeRenderPack();
    mmm_RenderPack->targetWindow = this;

    setFormat(sstd::getDefaultOpenGLFormat());

    ConstructRenderPack constructRenderPack{ mmm_RenderPack };
    constructRenderPack();

    m_quickRenderer = new QuickRenderer;
    m_quickRenderer->mmm_RenderPack = this->mmm_RenderPack;


    // The QOpenGLContext and the QObject representing the rendering logic on
    // the render thread must live on that thread.
    //m_context->moveToThread(mmm_RenderPack->renderThread);
    m_quickRenderer->moveToThread(mmm_RenderPack->renderThread);

    //m_quickRendererThread->start();

    // Now hook up the signals. For simplicy we don't differentiate
    // between renderRequested (only render is needed, no sync) and
    // sceneChanged (polish and sync is needed too).
    connect(mmm_RenderPack->sourceViewControl.get() , &QQuickRenderControl::renderRequested, this, &sstd::Window::requestUpdate);
    connect(mmm_RenderPack->sourceViewControl.get(), &QQuickRenderControl::sceneChanged, this, &sstd::Window::requestUpdate);
}

sstd::Window::~Window() {
    return;
    // Release resources and move the context ownership back to this thread.
 //  m_quickRenderer->mutex()->lock();
 //  m_quickRenderer->requestStop();
 //  m_quickRenderer->cond()->wait(m_quickRenderer->mutex());
 //  m_quickRenderer->mutex()->unlock();
 //
 //  m_quickRendererThread->quit();
 //  m_quickRendererThread->wait();
 //
 //  delete m_renderControl;
 //  delete m_qmlComponent;
 //  delete m_quickWindow;
 //  delete m_qmlEngine;
 //
 //  delete m_offscreenSurface;
 //  delete m_context;
}

void sstd::Window::requestUpdate() {
    if (m_quickInitialized && !m_psrRequested) {
        m_psrRequested = true;
        QCoreApplication::postEvent(this, new QEvent(UPDATE));
    }
}

bool sstd::Window::event(QEvent *e) {
    if (e->type() == UPDATE) {
        polishSyncAndRender();
        m_psrRequested = false;
        return true;
    } else if (e->type() == QEvent::Close) {
        // Avoid rendering on the render thread when the window is about to
        // close. Once a QWindow is closed, the underlying platform window will
        // go away, even though the QWindow instance itself is still
        // valid. Operations like swapBuffers() are futile and only result in
        // warnings afterwards. Prevent this.
        m_quickRenderer->aboutToQuit();
    }
    return QWindow::event(e);
}

void sstd::Window::polishSyncAndRender() {
    Q_ASSERT(QThread::currentThread() == thread());

    // Polishing happens on the gui thread.
    mmm_RenderPack->sourceViewControl.get()->polishItems();
    // Sync happens on the render thread with the gui thread (this one) blocked.
    QMutexLocker lock(m_quickRenderer->mutex());
    m_quickRenderer->requestRender();
    // Wait until sync is complete.
    m_quickRenderer->cond()->wait(m_quickRenderer->mutex());
    // Rendering happens on the render thread without blocking the gui (main)
    // thread. This is good because the blocking swap (waiting for vsync)
    // happens on the render thread, not blocking other work.
}

void sstd::Window::run() {
    //disconnect(m_qmlComponent, &QQmlComponent::statusChanged, this, &sstd::Window::run);


}

void sstd::Window::updateSizes() {

    mmm_RenderPack->targetWindowHeight = height();
    mmm_RenderPack->targetWindowWidth = width();

    // Behave like SizeRootObjectToView.
    mmm_RenderPack->sourceRootItem->setWidth(width());
    mmm_RenderPack->sourceRootItem->setHeight(height());



    mmm_RenderPack->sourceView->setGeometry(0, 0, width(), height());
}

void sstd::Window::startQuick(const QUrl &filename) {
    auto m_qmlComponent = sstd::sstdNew< QQmlComponent>(mmm_RenderPack->sourceQQmlEngine.get(), filename);
    m_qmlComponent->deleteLater();

    if (m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

    QObject *rootObject = m_qmlComponent->create();
    if (m_qmlComponent->isError()) {
        const QList<QQmlError> errorList = m_qmlComponent->errors();
        for (const QQmlError &error : errorList)
            qWarning() << error.url() << error.line() << error;
        return;
    }

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

    m_quickInitialized = true;

    // Initialize the render thread and perform the first polish/sync/render.
    m_quickRenderer->requestInit();
    polishSyncAndRender();

}

void sstd::Window::exposeEvent(QExposeEvent *) {
    if (isExposed()) {
        if (!m_quickInitialized) {
            startQuick(QStringLiteral("myqml/quickrendercontrol/main.qml"));
        }
    }
}

void sstd::Window::resizeEvent(QResizeEvent *) {
    // If this is a resize after the scene is up and running, recreate the fbo and the
    // Quick item and scene.
    if (mmm_RenderPack->sourceRootItem) {
        updateSizes();
        m_quickRenderer->requestResize();
        polishSyncAndRender();
    }
}

void sstd::Window::mousePressEvent(QMouseEvent *e) {
    // Use the constructor taking localPos and screenPos. That puts localPos into the
    // event's localPos and windowPos, and screenPos into the event's screenPos. This way
    // the windowPos in e is ignored and is replaced by localPos. This is necessary
    // because QQuickWindow thinks of itself as a top-level window always.
    QMouseEvent mappedEvent(e->type(), e->localPos(), e->screenPos(), e->button(), e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(mmm_RenderPack->sourceView.get(), &mappedEvent);
}

void sstd::Window::mouseReleaseEvent(QMouseEvent *e) {
    QMouseEvent mappedEvent(e->type(), e->localPos(), e->screenPos(), e->button(), e->buttons(), e->modifiers());
    QCoreApplication::sendEvent(mmm_RenderPack->sourceView.get(), &mappedEvent);
}
