#pragma once

#include <sstd_memory.hpp>
#include <QtQml/qqmlapplicationengine.h>
class QQuickItem;

class QmlApplicationEngine : public QQmlApplicationEngine {
    Q_OBJECT
public:
    QmlApplicationEngine(QObject *parent = nullptr);
private:
    QQuickItem * mmm_ImageSource{ nullptr };
    QQuickItem * mmm_ImageTarget{ nullptr };
    Q_SLOT void ppp_ObjectCreated(QObject *object, const QUrl &url);
private:
    using Super = QQmlApplicationEngine;
    SSTD_MEMORY_QOBJECT_DEFINE(QmlApplicationEngine)
};


