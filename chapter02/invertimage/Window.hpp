#pragma once

#include <atomic>
#include <memory>
#include <QtGui/qwindow.h>
#include <QtCore/qthread.h>
#include <QtGui/qopenglcontext.h>

class Window : public QWindow {
    Q_OBJECT
public:
    Window();

public slots:
     
private:
    QOpenGLContext * $m$OpenGLContex;
};


