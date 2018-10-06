#include "QmlApplicationEngine.hpp"
#include <QtQml/qqmlcontext.h>

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

QmlApplicationEngine::QmlApplicationEngine(QObject * parent):Super(parent){
    this->rootContext()->setContextProperty(QStringLiteral("engine"),this);
    this->load( sstd::getLocalFileFullPath(
                    QStringLiteral(R"(myqml/animation/main.qml)") ) );
}

/*清除缓存，支持热更新*/
void QmlApplicationEngine::clearAllComponentCache(){
    this->clearComponentCache();
}

