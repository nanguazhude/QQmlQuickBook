#include "ChatSimpleReply.hpp"
#include <QtCore/qfile.h>

namespace {
    inline QByteArray getData(){
        const static QByteArray varAns=[](){
            QFile varFile{ QStringLiteral(":/chat/qml/SimpleReply.qml") };
            varFile.open(QIODevice::ReadOnly);
            return varFile.readAll();
        }();
        return varAns;
    }
}

namespace sstd {
    QByteArray ChatSimpleReply::getQmlBackgroundDelegateData() const {
        return getData();
    }
}

