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

QObject * QmlLocalFileLoader::createRootView(const QString & name, const QString & arg) const {
    auto varRootWindow = sstdNew< RootWindow >( );
    varRootWindow->load(arg);
    varRootWindow->setTitle(name);
    return varRootWindow;
}/****/

QmlLocalFileLoader * QmlLocalFileLoader::instance() {
    const static auto varThis = sstd::make_unique<QmlLocalFileLoader>();
    return varThis.get();
}

QmlLocalFileLoader::QmlLocalFileLoader(){
    /*单例...*/
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

static inline void registerThis() {
    qmlRegisterSingletonType<QmlLocalFileLoader>("myqml.animationandstate",
        1, 0,
        "QmlLocalFileLoader",
        [](QQmlEngine *engine, QJSEngine *scriptEngine)->QObject * {
        auto varAns = QmlLocalFileLoader::instance();
        return varAns;
        (void)engine;
        (void)scriptEngine;
    });
}
Q_COREAPP_STARTUP_FUNCTION(registerThis)




