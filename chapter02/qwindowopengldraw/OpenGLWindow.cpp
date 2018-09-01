#include <GL/glew.h>
#include "OpenGLWindow.hpp"

OpenGLWindow::OpenGLWindow(QScreen *screen  ):Super(screen){
    this->setMinimumWidth(64);
    this->setMinimumHeight(64);
    this->resize(512,512);
    glewInit();
}


















