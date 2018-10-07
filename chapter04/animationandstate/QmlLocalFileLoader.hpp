#pragma once

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <sstd_memory.hpp>

class QmlLocalFileLoader : public QObject {
    Q_OBJECT
public:
    Q_SLOT QObject * getSource(QObject *, const QString &,const QString & arg) const ;
    static QmlLocalFileLoader * instance();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(QmlLocalFileLoader)
};













