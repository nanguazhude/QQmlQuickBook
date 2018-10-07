﻿#include "QmlApplicationEngine.hpp"
#include <QtQml/qqmlcontext.h>

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

QmlApplicationEngine::QmlApplicationEngine(QObject * parent):Super(parent){
    this->load( sstd::getLocalFileFullPath(
                    QStringLiteral(R"(myqml/animation/main.qml)") ) );
}

