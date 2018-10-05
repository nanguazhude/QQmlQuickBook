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
        sstd::QuickImageProvider::addImage(varSourceImageID, QImage(varTestImagePath));
        QQmlProperty::write(mmm_ImageSource, QStringLiteral("source"), varSourceImageID);
    }

    {
        auto varThread = new sstd::RenderThread;
        connect(varThread, &sstd::RenderThread::renderFinished,
            this, [this](const QImage & argImage) {
            const auto varTargetImageID = sstd::QuickImageProvider::getNextIndexHeader();
            sstd::QuickImageProvider::addImage(varTargetImageID, argImage);
            QQmlProperty::write(mmm_ImageTarget, QStringLiteral("source"), varTargetImageID);
        });
        varThread->start(sstd::getLocalFileFullPath(
            QStringLiteral("myqml/invertimage/test.png")).toLocalFile());
    }

}



