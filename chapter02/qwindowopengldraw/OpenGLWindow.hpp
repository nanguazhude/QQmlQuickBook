#pragma once

#include <sstd_memory.hpp>
#include <QtGui/qopenglwindow.h>
class QScreen;
class QOpenGLContext;

class OpenGLWindow :public QOpenGLWindow {
    Q_OBJECT
public:
    OpenGLWindow(QOpenGLWindow::UpdateBehavior updateBehavior = NoPartialUpdate, QWindow *parent = nullptr);
    ~OpenGLWindow();
private:
    using Super = QOpenGLWindow;
protected:

    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;

    virtual bool event(QEvent *ev) override;

    class DrawData;
    DrawData * _m_draw_data = nullptr ;
private:
    SSTD_MEMORY_DEFINE(OpenGLWindow)
};











