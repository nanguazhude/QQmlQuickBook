#pragma once

#include <atomic>
#include <memory>
#include <QtGui/qwindow.h>
#include <QtCore/qthread.h>
#include <QtGui/qopenglcontext.h>

class Window;
class ThreadBody;

class Thread : public QThread {
    Q_OBJECT
public:
    Thread();
    void run() override ;
           
    Q_SIGNAL void openglProcessImage(const QImage &, QImage *);
    void init();
private:
    Q_SIGNAL void __init();
    std::weak_ptr<Window> $m$Window;
    ThreadBody * $m$ThreadBody;
    std::atomic_bool $m$ValueSet{false};
};

class ThreadBody :public QObject {
    Q_OBJECT
public:
    friend Thread;
public slots:
    void init();
private:
    std::shared_ptr<Window> $m$Window;
};

class Window : public QWindow {
    Q_OBJECT
public:
    Window();

public slots:
    void openglProcessImage(const QImage &,QImage *);

private:
    QOpenGLContext $m$OpenGLContex;
};


