#include "QmlApplicationEngine.hpp"

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

QmlApplicationEngine::QmlApplicationEngine(QObject * parent):Super(parent){
    this->load( sstd::getLocalFileFullPath(
                    QStringLiteral(R"(myqml/qsgdrawpoint/main.qml)") ) );
}



