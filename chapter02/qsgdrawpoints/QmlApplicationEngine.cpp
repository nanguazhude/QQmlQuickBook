#include "QmlApplicationEngine.hpp"
#include "chatsource/ChatView.hpp"

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

QmlApplicationEngine::QmlApplicationEngine(QObject * parent):Super(parent){
    sstd::ChatView::registerChat();
    this->load( sstd::getLocalFileFullPath(
                    QStringLiteral(R"(myqml/chat/main.qml)") ) );
}



