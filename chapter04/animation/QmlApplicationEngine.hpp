#pragma once

#include <sstd_memory.hpp>
#include <QtQml/qqmlapplicationengine.h>

class QmlApplicationEngine : public QQmlApplicationEngine {
    Q_OBJECT
public:
    QmlApplicationEngine(QObject *parent = nullptr);
public:
    Q_SLOT void clearAllComponentCache();
private:
    using Super = QQmlApplicationEngine;
    SSTD_MEMORY_QOBJECT_DEFINE(QmlApplicationEngine)
};


