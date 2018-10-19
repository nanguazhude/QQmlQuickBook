#include <sstd_glew.hpp>
#include "sstd_Window.hpp"
#include <ConstructQSurface.hpp>
#include <QtQuick/qquickview.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuick/qquickitem.h>
#include <QtGui/qoffscreensurface.h>
#include <QtGui/qopenglframebufferobject.h>


namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

namespace /*渲染所需数据数据*/ {

    class WindowRenderPack final : public sstd::RenderPack {
    public:
        std::unique_ptr<QOpenGLFramebufferObject> sourceFrameBufferObject;
        GLuint targetVAO{ 0 };
        GLuint targetVAOBuffer{ 0 };
        GLuint targetVAOIndexBuffer{ 0 };
        GLuint targetProgram{ 0 };
    private:
        SSTD_MEMORY_DEFINE(WindowRenderPack)
    };

}/*namespace*/

namespace /*渲染所需控制器*/ {

    class RenderControl : public QQuickRenderControl {
        QWindow * const mmm_Window;
    public:
        RenderControl(QWindow * r) : mmm_Window(r) {
        }
        QWindow * renderWindow(QPoint *offset) {
            if (offset) {
                *offset = { 0,0 };
            }
            return mmm_Window;
        }
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(RenderControl)
    };

}/*namespace*/

namespace /*渲染所需方法*/ {

    using RPT = std::shared_ptr<sstd::RenderPack>;

    /*准备渲染环境... 在非gui线程... */
    class UpdataSourceContex {
        RPT mmm_RenderPack;
    public:

        UpdataSourceContex(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() const {
            mmm_RenderPack->sourceContex->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());
        }

    };

    /*更新Qml数据... 必须在gui线程... */
    class PolishSourceWindow {
        RPT mmm_RenderPack;
    public:
        PolishSourceWindow(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() const {
            auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
            varRenderControl->polishItems();
        }

    };

    /*生成QSG树... 在非gui线程... 同时锁定gui线程... */
    class SyncSourceQSGDAta {
        RPT mmm_RenderPack;
    public:
        SyncSourceQSGDAta(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void operator()() const {
            auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
            varRenderControl->sync();
            glFinish();
        }

    };

    /*将QQuickWindow内容渲染到FBO... 非GUI线程... */
    class RenderSourceWindow {
        RPT mmm_RenderPack;
    public:
        RenderSourceWindow(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

        void updateFBO() const {
            auto varPack = static_cast<WindowRenderPack *>(mmm_RenderPack.get());
            const auto varPixelRatio = varPack->targetWindowDevicePixelRatio.load();
            const auto varHeight = static_cast<int>(varPack->targetWindowHeight.load() * varPixelRatio);
            const auto varWidth = static_cast<int>(varPack->targetWindowWidth.load() *varPixelRatio);
            do {
                if (varPack->sourceFrameBufferObject) {
                    const auto varSize = varPack->sourceFrameBufferObject->size();
                    if (varSize.width() != varWidth) {
                        break;
                    }
                    if (varSize.height() != varHeight) {
                        break;
                    }
                } else {
                    break;
                }
                return;
            } while (false);
            varPack->sourceFrameBufferObject.reset();
            varPack->sourceFrameBufferObject =
                sstd::make_unique<QOpenGLFramebufferObject>(QSize(varWidth, varHeight), QOpenGLFramebufferObject::CombinedDepthStencil);
            mmm_RenderPack->sourceView->setRenderTarget(varPack->sourceFrameBufferObject.get());
        }

        void operator()() const {
            auto varRenderControl = mmm_RenderPack->sourceViewControl.get();
            this->updateFBO();
            varRenderControl->render();
            glFinish();
        }

    };

    /*将FBO数据渲染到QWindow ... 非GUI线程 ...*/
    class DrawOnTarget {
        RPT mmm_RenderPack;
    public:
        DrawOnTarget(RPT arg) : mmm_RenderPack(std::move(arg)) {
        }

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

        void initProgram() const {
            auto varPack = static_cast<WindowRenderPack *>(mmm_RenderPack.get());
            if (varPack->targetProgram) {
                return;
            }

            varPack->targetProgram = buildVFShader(
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

        void initVAO() const {
            auto varPack = static_cast<WindowRenderPack *>(mmm_RenderPack.get());
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

        void draw() const {
            auto varPack = static_cast<WindowRenderPack *>(mmm_RenderPack.get());
            initProgram();
            initVAO();
            glUseProgram(varPack->targetProgram);
            glBindVertexArray(varPack->targetVAO);
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, varPack->sourceFrameBufferObject->texture());
            glBindTextureUnit(1, varPack->sourceFrameBufferObject->texture());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
            glFinish();
        }

        void operator()() const {
            mmm_RenderPack->targetContex->makeCurrent(mmm_RenderPack->targetWindow);
            draw();
            mmm_RenderPack->targetContex->swapBuffers(mmm_RenderPack->targetWindow);
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
                varPack->sourceViewControl->invalidate();
                /*about to desotry contex ... */
                varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
                static_cast<WindowRenderPack*>(varPack.get())->sourceFrameBufferObject.reset();
                varPack->sourceContex->doneCurrent();
                /*desdestory opengl data ...*/
                varPack->targetContex->makeCurrent(varPack->targetWindow);
                glDeleteProgram( static_cast<WindowRenderPack*>(varPack.get())->targetProgram );
                glDeleteVertexArrays(1,&( static_cast<WindowRenderPack*>(varPack.get())->targetVAO ));
                glDeleteBuffers(1,&( static_cast<WindowRenderPack*>(varPack.get())->targetVAOBuffer ));
                glDeleteBuffers(1,&( static_cast<WindowRenderPack*>(varPack.get())->targetVAOIndexBuffer ));
                varPack->targetContex->doneCurrent();
                /*delete contex in main thread ...*/
                varPack->sourceContex->moveToThread(qApp->thread());
                varPack->targetContex->moveToThread(qApp->thread());
                /*the thread will delete it self */
                varPack->renderThread->quit();
            });
            varFutures->data()->wait();
            mmm_RenderPack->sourceViewControl.reset();
            mmm_RenderPack->sourceView.reset();
            mmm_RenderPack->sourceQQmlEngine.reset();
            mmm_RenderPack->sourceOffscreenSurface.reset();
            mmm_RenderPack->sourceContex.reset();
            mmm_RenderPack->targetContex.reset();
        }

        /*destory the contex*/
        if (this->getContex()) {
            this->getContex()->deleteLater();
        }

    }

    bool Window::event(QEvent *event) {
        switch (event->type()) {
            case QEvent::UpdateRequest:
        {
                this->ppp_Init();
                ppp_SceneChanged();
        }
                break;
        case QEvent::Close:
            break;
        default:break;
        }
        return QWindow::event(event);
    }

    void Window::exposeEvent(QExposeEvent *event) {
        this->ppp_Init();
        ppp_SceneChanged();
        return QWindow::exposeEvent(event);
    }

    void Window::resizeEvent(QResizeEvent * e){
        this->ppp_Init();
        ppp_SceneChanged();
        return QWindow::resizeEvent(e );
    }

    void Window::ppp_Init() {

        if (mmm_Contex == nullptr) {
            mmm_Contex = sstdNew<QOpenGLContext>();
            mmm_Contex->setFormat(sstd::getDefaultOpenGLFormat());
            mmm_Contex->create();
            /*make current in this thread*/
            mmm_Contex->makeCurrent(this);
            glewInitialize();
        }

        /*************************************************/
        makeRender();
        /*************************************************/
        loadQml();
        /*************************************************/
    }

    void Window::ppp_UpdateSize() {
        /* Behave like SizeRootObjectToView ... */
        mmm_RenderPack->sourceRootItem->setWidth(this->width());
        mmm_RenderPack->sourceRootItem->setHeight(this->height());
        mmm_RenderPack->sourceView->setGeometry(0, 0, this->width(), this->height());
    }

    void Window::loadQml() {
        std::call_once(mmm_LoadQml, [this]() {
            auto varRootCompoinent = sstdNew<QQmlComponent>(mmm_RenderPack->sourceQQmlEngine.get());
            varRootCompoinent->deleteLater();
            varRootCompoinent->loadUrl(getLocalFileFullPath(QStringLiteral("myqml/rendercontrol/main.qml")));
            auto varRootItem = qobject_cast<QQuickItem *>(varRootCompoinent->create());
            if (varRootCompoinent->isError()) {
                qDebug() << varRootCompoinent->errors();
                assert(false);
                return;
            }
            varRootItem->setParent(mmm_RenderPack->sourceView->contentItem());
            varRootItem->setParentItem(mmm_RenderPack->sourceView->contentItem());
            mmm_RenderPack->sourceRootItem = varRootItem;
        });
        ppp_UpdateSize();
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
            auto varPack = sstd::make_shared<WindowRenderPack>();
            mmm_RenderPack = varPack;
            /*copy this data */
            varPack->targetWindow = this;
            varPack->globalWindowContex = this->mmm_Contex;
            /*create offscreen surface*/
            varPack->sourceOffscreenSurface = sstd::make_unique<QOffscreenSurface>();
            varPack->sourceOffscreenSurface->setFormat(sstd::getDefaultOpenGLFormat());
            varPack->sourceOffscreenSurface->create();
            /*make render control*/
            varPack->sourceViewControl = sstd::make_unique<RenderControl>(this);
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
                    varPack->sourceContex->setShareContext(varPack->globalWindowContex);
                    varPack->sourceContex->makeCurrent(varPack->sourceOffscreenSurface.get());
                    glewInitialize();
                    varPack->sourceViewControl->initialize(varPack->sourceContex.get());
                    /*create opengl contex in the thread ... */
                    varPack->targetContex = sstd::make_unique<QOpenGLContext>();
                    varPack->targetContex->setFormat(sstd::getDefaultOpenGLFormat());
                    varPack->targetContex->create();
                    varPack->targetContex->setShareContext(varPack->globalWindowContex);
                    varPack->targetContex->makeCurrent(varPack->targetWindow);
                   
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

        auto varRenderThread = mmm_RenderPack->renderThread;
        /*render and draw in target ... */
        std::tuple<
            RenderSourceWindow,
            DrawOnTarget > varDrawSteps{ mmm_RenderPack,mmm_RenderPack };
        varRenderThread->applyInThisThread(std::move(varDrawSteps));

    }

    void Window::ppp_SceneChanged() {
        assert(QThread::currentThread() == thread());
        ppp_Init();

        /*ask update source render contex ... */
        auto varRenderThread = mmm_RenderPack->renderThread;
        varRenderThread->runInThisThread(UpdataSourceContex{ mmm_RenderPack });

        /*polish source window data ... */
        {
            PolishSourceWindow varPolish{ mmm_RenderPack };
            varPolish();
        }

        /*sync render and draw in target ... */
        std::tuple<
            SyncSourceQSGDAta,
            RenderSourceWindow,
            DrawOnTarget > varDrawSteps{ mmm_RenderPack , mmm_RenderPack,mmm_RenderPack };

        auto varFutures = varRenderThread->applyInThisThread(std::move(varDrawSteps));
        if (varFutures) {
            /*sync need block main thread ...*/
            varFutures->data()->wait();
        }

    }

} /*namespace sstd*/












