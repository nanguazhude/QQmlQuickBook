/*OpenglDrawWindow.hpp*/
#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <ConstructQSurface.hpp>
class QQuickWindow;

class  OpenglDrawWindowItemRender;

class OpenglDrawWindowItem : public QQuickItem {
    Q_OBJECT
public:
    OpenglDrawWindowItem(QQuickItem *parent = nullptr);
private slots:
    void handleWindowChanged(QQuickWindow *)/*windows changed*/;
    void sync()/*update data before opengl draw*/;
    void cleanup()/*clean up when opengl contex destory*/;
private:
    OpenglDrawWindowItemRender * _m_render = nullptr;
private:
    using Super = QQuickItem;
    
private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindowItem)
};

class OpenglDrawWindowItemRender : 
    public QObject   {
    Q_OBJECT
public:
    OpenglDrawWindowItemRender();
    ~OpenglDrawWindowItemRender();
public slots:
    void initializeGL() ;
    void paintGL() ;
    void resizeGL(int w, int h) ;
public:
    inline void resizeGL(const QSize & arg) { resizeGL(arg.width(), arg.height()); }
    inline void setWindow(QQuickWindow * arg) { _m_window = arg; }
private:
    QQuickWindow * _m_window = nullptr;
    class DrawData;
    DrawData * _m_draw_data = nullptr;
private:
    using Super = QObject;
private:
    SSTD_MEMORY_DEFINE(OpenglDrawWindowItemRender)
};

/***************************/

//参考Qt帮助
//Scene Graph - OpenGL Under QML 

/***************************/











