#include "RootWindow.hpp"
#include "QuickImageProvider.hpp"
#include <QtQuick/qquickitem.h>
#include <QtQml/qqmlproperty.h>
#include <QtQuick/qquickwindow.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/qopenglcontext.h>
#include "RenderThread.hpp"

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

RootWindow::RootWindow() :Super() {

    this->setTitle( trUtf8( u8R"(Compute Shader Invert Image)" ) );
    this->setMinimumSize({256,256});
    this->setClearColor(QColor(0,0,0,1));

    this->engine()->addImageProvider(sstd::QuickImageProvider::getIndexHeader(),
        sstd::QuickImageProvider::instance());
    this->load(sstd::getLocalFileFullPath(
        QStringLiteral(R"(myqml/computeshaderinvertimage/main.qml)")));

    {
        const auto varRoot = this->rootObject();
        assert(varRoot);
        mmm_ImageSource = varRoot->findChild<QQuickItem *>(QStringLiteral("imageSource"));
        mmm_ImageTarget = varRoot->findChild<QQuickItem *>(QStringLiteral("imageTarget"));
    }

    assert(mmm_ImageTarget);
    assert(mmm_ImageSource);

    {
        const auto varTestImagePath = sstd::getLocalFileFullPath(
            QStringLiteral("myqml/computeshaderinvertimage/test.png")).toLocalFile();
        const auto varSourceImageID = sstd::QuickImageProvider::getNextIndexHeader();
        /*erase old image*/
        sstd::QuickImageProvider::addImage(QQmlProperty::read(mmm_ImageSource, QStringLiteral("source")).toString(), {});
        /*add new image*/
        sstd::QuickImageProvider::addImage(varSourceImageID, QImage(varTestImagePath));
        /*udpate image*/
        QQmlProperty::write(mmm_ImageSource, QStringLiteral("source"), varSourceImageID);
    }

    {
        auto varThread = new sstd::RenderThread;
        connect(varThread, &sstd::RenderThread::renderFinished,
            this, [this](const QImage & argImage) {
            const auto varTargetImageID = sstd::QuickImageProvider::getNextIndexHeader();
            /*erase old image*/
            sstd::QuickImageProvider::addImage(QQmlProperty::read(mmm_ImageTarget, QStringLiteral("source")).toString(), {});
            /*add new image*/
            sstd::QuickImageProvider::addImage(varTargetImageID, argImage);
            /*udpate image*/
            QQmlProperty::write(mmm_ImageTarget, QStringLiteral("source"), varTargetImageID);
        }, Qt::QueuedConnection);
        varThread->start(sstd::getLocalFileFullPath(
            QStringLiteral("myqml/computeshaderinvertimage/test.png")).toLocalFile());
    }

}



