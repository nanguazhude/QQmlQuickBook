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
#include <QtCore/qbuffer.h>
#include <QtCore/qfile.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextlayout.h>

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

namespace {
    /*这里不做缓存优化...*/
    void flipUpDownByComputShader(GLuint varTexture,const QSize & varSize) {
        const auto varComputeHeight = ( varSize.height() >> 1 );
        if (varComputeHeight < 0) {
            return;
        }
        GLuint varProgram{0};
        varProgram = glCreateProgram();

        {
            auto varShaderSource = u8R"___(
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding = 0,rgba8ui) uniform uimage2D argImageInputOutput;
layout(location = 1)        uniform int argImageHeight;

void main(void) {
    ivec2 varPos0 = ivec2( gl_WorkGroupID.xy ) ;
    ivec2 varPos1 = ivec2( varPos0.x,argImageHeight - varPos0.y );
    uvec4 varColor0 = imageLoad(argImageInputOutput,varPos0 );
    uvec4 varColor1 = imageLoad(argImageInputOutput,varPos1 );
    imageStore(argImageInputOutput,varPos0,varColor1);
    imageStore(argImageInputOutput,varPos1,varColor0);
}

/*一个简单的图像上下颠倒shader*/

)___"sv;

            auto varShader = glCreateShader(GL_COMPUTE_SHADER);
            {
                GLint varSL = static_cast<GLint>(varShaderSource.size());
                const char *varS[]{ varShaderSource.data() };
                glShaderSource(varShader, 1, varS, &varSL);
                glCompileShader(varShader);
                glAttachShader(varProgram, varShader);
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

                glDeleteShader(varShader);
            }
        }
        
        glUseProgram(varProgram);
        glUniform1i(1, varSize.height()-1);
        glBindImageTexture(0, varTexture, 0, false, 0, GL_READ_WRITE, GL_RGBA8UI);
        glDispatchCompute(varSize.width(), varComputeHeight, 1);
        glFinish();
        glDeleteProgram(varProgram);

    }
}/*namespace*/

namespace {
    inline QString urlPathToLocalPath(const QString & arg) {
        auto tmp = arg;
        tmp.replace(QChar('\\'), QChar('/'));
        if (tmp.startsWith(QStringLiteral("file:///"))) {
            if (tmp.count(QStringLiteral(":/")) > 1) {/*windows ...*/
                return arg.right(arg.size() - 8);
            }
            return arg.right(arg.size() - 7);
        }
        return arg;
    }
}/*namespace*/

namespace sstd {

    void Window::ppp_1_start_render(const QVariant & argQmlPath, const QVariant & argSavePath) {
        if (mmm_CurrentDuty) {
            qDebug() << QStringLiteral("last duty do not finished!");
            return;
        }
        ppp_start_render(argQmlPath.toString());
        mmm_CurrentDuty = connect(this, &Window::renderFinished, this, [this, argSavePath](const QImage & arg) {
            disconnect(*mmm_CurrentDuty);
            mmm_CurrentDuty.reset();
            const auto varSavePath = argSavePath.toString();
            QFile varFile{ urlPathToLocalPath(varSavePath) };
            if (false == varFile.open(QIODevice::WriteOnly)) {
                qDebug() << "can not write : " << varSavePath;
                return;
            }
            arg.save(&varFile, "png");

        });
    }

    Window::Window() {
        mmm_Thread = sstdNew<sstd::QuickThread>();
        connect(this, &Window::startRender, this, &Window::ppp_start_render);
        this->setResizeMode(Super::ResizeMode::SizeRootObjectToView);
        this->setMinimumSize(QSize{ 512,128 });
        this->setSource(getLocalFileFullPath(QStringLiteral("myqml/quickrendertoimage/main.qml")));
        auto varRoot = this->rootObject();
        if (varRoot) {
            connect(varRoot, SIGNAL(runARender(QVariant, QVariant)), this, SLOT(ppp_1_start_render(QVariant, QVariant)));
        }
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
                this->setColor(QColor(0,0,0,0));
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
            varAns->sourceWindow->setSource(QUrl::fromLocalFile(urlPathToLocalPath(path)));
            /*set render thread ...*/
            varAns->sourceControl->prepareThread(varAns->renderThread);
            varAns->sourceContex->moveToThread(varAns->renderThread);
            /*create tmp offscreensurface to openglcontex ...*/
            varAns->sourceContexSurface = sstd::make_unique<QOffscreenSurface>();
            varAns->sourceContexSurface->setFormat(sstd::getDefaultOpenGLFormat());
            varAns->sourceContexSurface->create();
            return std::move(varAns);
        }

    }/*namespace */

    namespace {
        class ErrorRender {
            const QString mmm_ErrorString;
        public:
            ErrorRender(const QString &arg) :mmm_ErrorString(arg) {
            }
            static QImage getDefaultErrorImage() {
                const static QImage varAns = []()->QImage {
                    QImage varAns{ sstd::getLocalFileFullPath(
                        QStringLiteral("myqml/quickrendertoimage/default_error.png"))
                        .toLocalFile()
                    };
                    assert(varAns.width()>0);
                    assert(varAns.height()>0);
                    return varAns.convertToFormat(QImage::Format_RGBA8888) ;
                }();
                return varAns.copy()/*get a deep clone ...*/;
            }
            QImage getErrorImage() const {
                auto varImage = getDefaultErrorImage();

                QPainter varPainter{ &varImage };
                const auto varImageWidth = varImage.width();
                const auto varImageHeight = varImage.height();
                {
                    auto varFont = varPainter.font();
                    varFont.setPixelSize( varImageHeight/15 );
                    varPainter.setFont(varFont);
                }
                double varLeading;
                {
                    const auto varFontMetrics = varPainter.fontMetrics();
                    varLeading = varFontMetrics.leading();
                }
                QTextLayout varLayout{ mmm_ErrorString,varPainter.font() };
                varLayout.setCacheEnabled(true) ;
                varLayout.beginLayout();
                double varHeight = 0;
                for(;;){
                     QTextLine varLine = varLayout.createLine();
                     if (!varLine.isValid()){
                              break;
                     }
                     varLine.setLineWidth(varImageWidth);
                    varHeight+=varLeading;
                    varLine.setPosition(QPointF(0,varHeight));
                    varHeight += varLine.height();
                    if(varHeight>varImageHeight){break;}
                }
                varLayout.endLayout();
                varLayout.draw(&varPainter,QPointF{0,0});

                return varImage;
            }
        };
    }/*namespace*/

#define BEGIN_TRY do { if(varRenderPack) { if (varRenderPack->hasError.load()) { return; } } \
    QImage varErrorImage ; \
    try { assert(true) \
/*begin try ... */

#define END_TRY } catch (const QString & error) { \
    if(varRenderPack) { varRenderPack->hasError.store(true); } \
    qDebug() << error; \
    ErrorRender varError{ error }; \
    varErrorImage = varError.getErrorImage(); \
    ppp_RenderFinished(varErrorImage); \
    return; \
    } catch (...) { \
        if(varRenderPack) { varRenderPack->hasError.store(true); } \
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
            /*create fbo ...
            there use default internal fotmat rgba8 ...*/
            varRenderPack->sourceFBO = sstd::make_unique<QOpenGLFramebufferObject>(
                varRenderSize,
                QOpenGLFramebufferObject::CombinedDepthStencil,
                GL_TEXTURE_2D);
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
            QImage varImage{ varRenderPack->sourceFBO->size(),QImage::Format_RGBA8888 };
            const auto varTexture = varRenderPack->sourceFBO->texture();
            /******************************************************************/
            /*flip up down by compute shader ... */
            flipUpDownByComputShader(varTexture, varRenderPack->sourceFBO->size());
            /******************************************************************/
            glBindTexture(GL_TEXTURE_2D, varTexture);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                const_cast<uchar *>(varImage.constBits())/*do not clone ...*/);
            ppp_RenderFinished(varImage);
            END_TRY;
        }, [this, varRenderPack]() {
            /*destory data in this thread ...*/
            BEGIN_TRY;
            glFinish();
            varRenderPack->sourceControl->invalidate();
            varRenderPack->sourceFBO.reset();
            varRenderPack->sourceContex->aboutToBeDestroyed();
            varRenderPack->sourceContex->doneCurrent();
            varRenderPack->sourceContex->moveToThread(qApp->thread());
            varRenderPack->renderThread->runInMainThread([this, varRenderPack]() {
                assert(qApp->thread()==QThread::currentThread());
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

















