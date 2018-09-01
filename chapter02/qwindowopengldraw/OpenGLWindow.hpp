#pragma once

#include <sstd_memory.hpp>
#include <QtGui/qwindow.h>
class QScreen;

class OpenGLWindow :public QWindow {
    Q_OBJECT
public:
    OpenGLWindow(QScreen * screen = nullptr);
private:
    using Super = QWindow;
private:
    SSTD_MEMORY_DEFINE(OpenGLWindow)
};











