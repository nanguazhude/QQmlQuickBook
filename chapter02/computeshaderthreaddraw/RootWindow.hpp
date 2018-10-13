#pragma once

#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>

class RootWindow : public QWindow {
    Q_OBJECT
public:
    RootWindow();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(RootWindow)
};

















