#ifndef SIMPLEDRAW_HPP
#define SIMPLEDRAW_HPP

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <QtGui/qopenglcontext.h>

class SimpleDraw : public QQuickItem {
    Q_OBJECT
public:
    SimpleDraw(QQuickItem *parent = nullptr);
private slots:
    void setOpenGLWindow(QQuickWindow *window);
    void clearOpenGL();
    void openGLPaint();
    void setViewPort();
private:
    bool _m_is_setviewport_first_run = true;
    std::unique_ptr<QOpenGLContext> _m_glcontex;
private:
    using Super = QQuickItem;
private:
    SSTD_MEMORY_DEFINE(SimpleDraw)
};

#endif // SIMPLEDRAW_HPP












