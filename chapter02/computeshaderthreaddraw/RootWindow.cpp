#include <QtGui/qcolor.h>
#include "RootWindow.hpp"
#include "RenderSize.hpp"

RootWindow::RootWindow() {
    /**/
    mmm_Mutex = sstd::make_shared< MutexPointer::element_type >();
    /*创建OpenGL渲染环境*/

    /**/
    this->setTitle(trUtf8(u8R"(Root Window)"));
    this->setMinimumSize({ getRenderWidth(),getRenderHeight() });
    this->setMaximumSize({ getRenderWidth(),getRenderHeight() });
}

RootWindow::~RootWindow() {

    auto varMutex = std::move( mmm_Mutex );

    /*rending need this data , so wait for rending finished*/
    for(;;){
        std::unique_lock varDestoryLock{ *varMutex };
        varMutex->isDesotry=true;
        if( varMutex->isRending ){ continue; }
        else{ break; }
    }

    /*destory the contex*/
    if(this->getContex()) {
        this->getContex()->doneCurrent();
        this->getContex()->deleteLater();
    }

}




