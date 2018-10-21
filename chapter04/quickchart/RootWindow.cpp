#include <QtGui/qcolor.h>
#include "RootWindow.hpp"

RootWindow::RootWindow(){
    this->setTitle( trUtf8( u8R"(Animation And State)" ) );
    this->setMinimumSize({256,256});
    this->setClearColor(QColor(0,0,0,1));
}







