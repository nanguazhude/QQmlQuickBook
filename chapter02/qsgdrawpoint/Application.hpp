#pragma once

#include <sstd_memory.hpp>
#include <QtWidgets/qapplication.h>

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char **argv, int = ApplicationFlags);
private:
    using Super = QApplication;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(Application)
};










