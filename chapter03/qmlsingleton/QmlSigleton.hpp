/*QmlSigleton.hpp*/
#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>
#include "DataItem.hpp"

class QmlSigleton : public QObject {
    Q_OBJECT
private:
    Q_PROPERTY(QColor myColor READ getMyColor WRITE setMyColor NOTIFY myColorChanged)
public:
    QColor getMyColor() const ;
    inline void setMyColor(const QColor & arg) {
        if (mmm_MyColor.setData(arg)) {
            myColorChanged();
        }
    }
    Q_SIGNAL void myColorChanged();
public:
    static QObject * instance();
public:
    QmlSigleton();
private:
    sstd::qmlsigleton::DataItem<QColor> mmm_MyColor{ QColor(128,128,128,255) };
private:
    SSTD_MEMORY_QOBJECT_DEFINE(QmlSigleton)
};


















