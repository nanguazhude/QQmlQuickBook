#include "ChatSimpleRequest.hpp"
#include <QtCore/qfile.h>

namespace {
    inline QByteArray getData(){
        const static QByteArray varAns=[](){
            QFile varFile{ QStringLiteral(":/chat/qml/SimpleRequest.qml") };
            varFile.open(QIODevice::ReadOnly);
            return varFile.readAll();
        }();
        return varAns;
    }
}

namespace sstd {
    QByteArray ChatSimpleRequest::getQmlBackgroundDelegateData() const {
        return getData();
    }
}

