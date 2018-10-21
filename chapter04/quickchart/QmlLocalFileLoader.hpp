#pragma once

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <sstd_memory.hpp>

class QmlLocalFileLoader : public QObject {
    Q_OBJECT
public:
    Q_SLOT QObject * createRootView(const QString & argName,const QString & argLocalPath) const ;
    static QmlLocalFileLoader * instance();
    QmlLocalFileLoader();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(QmlLocalFileLoader)
};













