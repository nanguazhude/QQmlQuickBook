#ifndef APPLICATIONENGINE_HPP
#define APPLICATIONENGINE_HPP

#include <sstd_memory.hpp>
#include <QtQml/qqmlapplicationengine.h>

class ApplicationEngine : public QQmlApplicationEngine {
    Q_OBJECT
public:
    ApplicationEngine(QObject * parent = nullptr);
private:
    using Super = QQmlApplicationEngine;
    SSTD_MEMORY_QOBJECT_DEFINE(ApplicationEngine)
};

#endif // APPLICATIONENGINE_HPP










