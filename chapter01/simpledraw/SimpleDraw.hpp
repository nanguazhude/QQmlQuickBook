#ifndef SIMPLEDRAW_HPP
#define SIMPLEDRAW_HPP

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <ConstructQSurface.hpp>

class SimpleDraw : public QQuickItem {
    Q_OBJECT
public:
    SimpleDraw(QQuickItem *parent = nullptr);
private slots:
    void handleWindowChanged(QQuickWindow *);
    void sync();
    void cleanup();
private:
private:
    using Super = QQuickItem;
private:
    SSTD_MEMORY_DEFINE(SimpleDraw)
};

class SimpleDrawOpenglRender : 
    public QObject ,
    public sstd::OpenGLFunctions {
    Q_OBJECT
public:
    SimpleDrawOpenglRender();
    ~SimpleDrawOpenglRender();
private:
    using Super = QObject;
private:
    SSTD_MEMORY_DEFINE(SimpleDrawOpenglRender)
};

#endif // SIMPLEDRAW_HPP












