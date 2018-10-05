#include "QmlApplicationEngine.hpp"
#include "QuickImageProvider.hpp"
#include <QtQuick/qquickitem.h>
#include <QtQml/qqmlproperty.h>
#include "RenderThread.hpp"

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

QmlApplicationEngine::QmlApplicationEngine(QObject * parent) :Super(parent) {
    this->addImageProvider(sstd::QuickImageProvider::getIndexHeader(),
        sstd::QuickImageProvider::instance());
    this->load(sstd::getLocalFileFullPath(
        QStringLiteral(R"(myqml/invertimage/main.qml)")));
    {
        const auto varRoots = this->rootObjects();
        assert(varRoots.size());
        for (const auto & varI : varRoots) {
            mmm_ImageSource = varI->findChild<QQuickItem *>(QStringLiteral("imageSource"));
            mmm_ImageTarget = varI->findChild<QQuickItem *>(QStringLiteral("imageTarget"));
            if (mmm_ImageTarget && mmm_ImageSource) { break; }
        }
    }

    assert(mmm_ImageTarget);
    assert(mmm_ImageSource);

    {
        const auto varTestImagePath = sstd::getLocalFileFullPath(
            QStringLiteral("myqml/invertimage/test.png")).toLocalFile();
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
        },Qt::QueuedConnection);
        varThread->start(sstd::getLocalFileFullPath(
            QStringLiteral("myqml/invertimage/test.png")).toLocalFile());
    }

}



