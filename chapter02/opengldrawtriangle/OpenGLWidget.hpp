#ifndef OPENGLWIDGET_HPP
#define OPENGLWIDGET_HPP

#include <sstd_memory.hpp>
#include <ConstructQSurface.hpp>
#include <QtWidgets/qopenglwidget.h>

class OpenGLWidget :
        public QOpenGLWidget,
        public sstd::OpenGLFunctions{
    Q_OBJECT
public:
    OpenGLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~OpenGLWidget();
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
private:
    using Super = QOpenGLWidget;
    class DrawData;
    DrawData * _m_draw_data = nullptr ;
private:
    SSTD_MEMORY_DEFINE(OpenGLWidget)
};

#endif // OPENGLWIDGET_HPP
