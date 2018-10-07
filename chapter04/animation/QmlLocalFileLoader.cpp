#include "QmlLocalFileLoader.hpp"
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqmlcomponent.h>
#include <array>
#include <mutex>
#include <atomic>

QObject * QmlLocalFileLoader::getSource(QObject * parent, const QString & arg) const {
    QByteArray varData;
    QQmlContext * varParentContex;
    QUrl varLocalFileNameUrl;
    {
        if (parent == nullptr) {
            qDebug() << QStringLiteral("error call function : ");
            return{};
        }
        varParentContex = qmlContext(parent);
        if (varParentContex == nullptr) {
            qDebug() << QStringLiteral("error call function : ");
            return{};
        }
        varLocalFileNameUrl = varParentContex->resolvedUrl(arg);
        const auto varLocalFileName = varLocalFileNameUrl.toLocalFile();
        QFile varFile{ varLocalFileName };
        if (false == varFile.open(QIODevice::ReadOnly)) {
            qDebug() << (QStringLiteral("can not open : ") + varLocalFileName);
            return {};
        }

        QTextStream varStream{ &varFile };
        varData = varStream.readAll().toUtf8();
    }

    if (varData.isEmpty()) {
        qDebug() << (QStringLiteral("can not read : "));
        return {};
    }
    auto varAnsComponent = sstdNew< QQmlComponent >(varParentContex->engine());
    varAnsComponent->deleteLater();
    varAnsComponent->setData(varData, varLocalFileNameUrl);
    auto varAns = varAnsComponent->create();
    if (varAns == nullptr) {
        qDebug() << varAnsComponent->errorString();
        return {};
    }
    return varAns;
}/****/

QmlLocalFileLoader * QmlLocalFileLoader::instance() {
    const static auto varThis = sstd::make_unique<QmlLocalFileLoader>();
    return varThis.get();
}

static inline void registerThis() {
    qmlRegisterSingletonType<QmlLocalFileLoader>("myqml.animation",
        1, 0,
        "QmlLocalFileLoader",
        [](QQmlEngine *engine, QJSEngine *scriptEngine)->QObject * {
        auto varAns = QmlLocalFileLoader::instance();
        /*单例...*/
        QQmlEngine::setObjectOwnership(varAns, QQmlEngine::CppOwnership);
        return varAns;
        (void)engine;
        (void)scriptEngine;
    });
}
Q_COREAPP_STARTUP_FUNCTION(registerThis)




