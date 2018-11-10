#include <QtGui/qcolor.h>
#include "RootWindow.hpp"

RootWindow::RootWindow(){
    setTitle( trUtf8( u8R"(Hellow World!)" ) );
    this->setMinimumSize({256,256});
    this->setClearColor(QColor(0,0,0,1));
}







