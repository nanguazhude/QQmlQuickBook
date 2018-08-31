#include <GL/glew.h>
#include "SimpleDraw.hpp"
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickwindow.h>
extern bool glewInitialize();

SimpleDraw::SimpleDraw(QQuickItem *parent ):Super(parent){
    this->setFlag(QQuickItem::ItemHasContents,true);
    connect(this, &QQuickItem::windowChanged, this, &SimpleDraw::setOpenGLWindow);
}

void SimpleDraw::setOpenGLWindow(QQuickWindow *window) {
    if (window == nullptr) { return; }
    connect(window, &QQuickWindow::beforeSynchronizing, this, &SimpleDraw::setViewPort, Qt::DirectConnection);
    connect(window, &QQuickWindow::sceneGraphInvalidated, this, &SimpleDraw::clearOpenGL, Qt::DirectConnection);
    window->setClearBeforeRendering(false);
    qDebug() << window;
}

void SimpleDraw::clearOpenGL(){
    
}

void SimpleDraw::setViewPort() {
    if (_m_is_setviewport_first_run) {
        _m_is_setviewport_first_run = false;
        connect(window(), &QQuickWindow::beforeRendering, this, &SimpleDraw::openGLPaint, Qt::DirectConnection);
        _m_glcontex = sstd::make_unique<QOpenGLContext>();
        {
            QSurfaceFormat requestedFormat;
            requestedFormat.setVersion(3, 3);
            requestedFormat.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
            _m_glcontex->setFormat(requestedFormat);
        }
        qDebug() << _m_glcontex->create();
        _m_glcontex->makeCurrent(window());
        glewInitialize();
       // _m_glcontex->doneCurrent();
        

    }

    _m_glcontex->makeCurrent(window());
    glViewport(0, 0, width(), height());
   // qDebug() <<window()->size() * window()->devicePixelRatio() ;
    
}
 
void SimpleDraw::openGLPaint(){
        
   /* static int a =0 ;
    qDebug() << ++a;
    glViewport(x(), y(), width(), height());*/
       
    //glClearDepth(0);
   // glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
         
    GLfloat data [] = { (std::rand() & 255) / 256.0,(std::rand() & 255) / 256.0,(std::rand() & 255) / 256.0,1};
     
    glClearNamedFramebufferfv(0, GL_DEPTH, 0, data);
    glClearNamedFramebufferfv(0, GL_COLOR, 0, data);




}




