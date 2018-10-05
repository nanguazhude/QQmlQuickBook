#include "QmlApplicationEngine.hpp"
#include "QuickImageProvider.hpp"

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

QmlApplicationEngine::QmlApplicationEngine(QObject * parent):Super(parent){
    this->addImageProvider( sstd::QuickImageProvider::getIndexHeader(),
                            sstd::QuickImageProvider::instance());
    this->load( sstd::getLocalFileFullPath(
                    QStringLiteral(R"(myqml/invertimage/main.qml)") ) );
}



