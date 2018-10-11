#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include <QtWidgets/qwidget.h>

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(MainWidget)
};







