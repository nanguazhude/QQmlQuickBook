/*OpenglDrawWindow.hpp*/
#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <ConstructQSurface.hpp>

class OpenglDrawWindow : public QQuickItem {
    Q_OBJECT
public:
    OpenglDrawWindow(QQuickItem *parent = nullptr);
private slots:
    void handleWindowChanged(QQuickWindow *);
    void sync();
    void cleanup();
private:
private:
    using Super = QQuickItem;
private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindow)
};

class OpenglDrawWindowRender : 
    public QObject ,
    public sstd::OpenGLFunctions {
    Q_OBJECT
public:
    OpenglDrawWindowRender();
    ~OpenglDrawWindowRender();
private:
    using Super = QObject;
private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindowRender)
};

/***************************/












