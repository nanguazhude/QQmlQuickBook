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
#include "RootWindow.hpp"

QObject * QmlLocalFileLoader::getSource(QObject * parent, const QString & arg) const {
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
    }
    auto varAnsComponent = sstdNew< RootWindow >( );
    varAnsComponent->load(varLocalFileNameUrl);
    return varAnsComponent;
}/****/

QmlLocalFileLoader * QmlLocalFileLoader::instance() {
    const static auto varThis = sstd::make_unique<QmlLocalFileLoader>();
    return varThis.get();
}

static inline void registerThis() {
    qmlRegisterSingletonType<QmlLocalFileLoader>("myqml.animationandstate",
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




