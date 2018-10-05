#include <GL/glew.h>
#include "RenderThread.hpp"
#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtGui/qimage.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qwindow.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qopenglcontext.h>
#include <QtGui/qoffscreensurface.h>
#include <ConstructQSurface.hpp>
extern bool glewInitialize();

sstd::RenderThread::RenderThread() {
    assert(qApp);
    {
        /*thread完成时自删除*/
        this->moveToThread(qApp->thread());
        connect(this, &QThread::finished, this, &QThread::deleteLater);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
            if (this->isRunning()) {
                this->quit();
                return;
            }
            this->deleteLater();
        });
    }
    /*此函数必须在main thread 或 gui thread 调用*/
    mmm_Surface = sstdNew<QOffscreenSurface>();
    mmm_Surface->setFormat(sstd::getDefaultOpenGLFormat());
    mmm_Surface->create();
}

void sstd::RenderThread::start(const QString & arg) {
    mmm_ImageFileName = arg;
    Super::start();
}

namespace {

    class Render {
        QOpenGLContext * mmm_OpenGLContex = nullptr;
    public:
        Render(sstd::RenderThread * arg) {
            /*this function will run in any thread*/
            mmm_OpenGLContex = sstdNew<QOpenGLContext>();
            mmm_OpenGLContex->setFormat(sstd::getDefaultOpenGLFormat());
            mmm_OpenGLContex->create();
            mmm_OpenGLContex->makeCurrent(arg->getSurface());
            glewInitialize();
        }
        ~Render() {
            delete mmm_OpenGLContex;
        }
    };

}/*namespace*/

void sstd::RenderThread::run() try {

    class RenderState {
    public:
        sstd::RenderThread * super = nullptr;
        QImage image;

        RenderState(sstd::RenderThread * a, QImage && arg) :super(a), image(std::move(arg)) {
            image = image.convertToFormat(QImage::Format::Format_RGBA8888);
        }

        void checkImageFormat() {
            if (image.width() < 1) { throw 1; }
            if (image.height() < 1) { throw 2; }
        }

        ~RenderState() { super->renderFinished(image); }

    }varState(this, [this]() { return QImage(this->mmm_ImageFileName); }());
    varState.checkImageFormat();

    std::unique_ptr<Render> varRender{ sstdNew<Render>(this) };
    using OpenGLResourceData = std::tuple<GLuint, GLuint>;
    class OpenGLResource : public OpenGLResourceData {
    public:
        enum {
            Program = 0,
            ImageTexture
        };
        OpenGLResource() :OpenGLResourceData(0, 0) {}
        ~OpenGLResource() {
            glDeleteProgram(std::get<Program>(*this));
            glDeleteTextures(1, &std::get<ImageTexture>(*this));
        }
    };

    OpenGLResource varOpenGLResource;

    /*初始化OpenGL Program*/

    /*初始化ImageTexture*/

    /*反转像素*/



}
catch (...) {
    qDebug() << "image format error : ";
}

sstd::RenderThread::~RenderThread() {
    mmm_Surface->deleteLater();
}

/** QQuickRenderControl Example  **/













