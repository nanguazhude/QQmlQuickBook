#include <QtGui/qcolor.h>
#include "RootWindow.hpp"

RootWindow::RootWindow(){
    setTitle( trUtf8( u8R"(物体与坐标关联)" ) );
    this->setMinimumSize({256,256});
    this->setClearColor(QColor(0,0,0,1));
}







