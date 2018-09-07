#pragma once

#include <sstd_memory.hpp>
#include <QtGui/qopenglwindow.h>
class ImageView;

class Window : public QOpenGLWindow {
    Q_OBJECT
public:
    Window();
    ~Window();
protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
private:
    class DrawData;
    DrawData * $m$DrawData = nullptr ;
    ImageView * $m$ImageView = nullptr;
public:
    SSTD_MEMORY_QOBJECT_DEFINE(Window)
};












