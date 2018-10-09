#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qpointer.h>
#include <QtGui/qopenglwindow.h>
class ImageView;

class Window : public QOpenGLWindow {
    Q_OBJECT
public:
    Window();
    ~Window();
protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeGL(int w, int h) override;
private:
    class DrawData;
    DrawData * mmm_DrawData = nullptr ;
    ImageView * mmm_ImageView = nullptr;
    QPointer<QObject> mmm_WatcherImageView;
protected:
    bool event(QEvent *e) override;
    using Super = QOpenGLWindow;
public:
    SSTD_MEMORY_QOBJECT_DEFINE(Window)
};












