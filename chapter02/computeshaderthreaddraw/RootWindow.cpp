#include <QtGui/qcolor.h>
#include "RootWindow.hpp"
#include "RenderSize.hpp"

RootWindow::RootWindow() {
    /**/
    mmm_Mutex = sstd::make_shared< MutexPointer::element_type >();
    /*创建OpenGL渲染环境*/

    /**/
    this->setTitle(trUtf8(u8R"(Root Window)"));
    /*固定窗口大小*/
    this->setMinimumSize({ getRenderWidth(),getRenderHeight() });
    this->setMaximumSize({ getRenderWidth(),getRenderHeight() });

}

RootWindow::~RootWindow() {

    auto varMutex = std::move(mmm_Mutex);
    varMutex->setDestory();

    /*rending need this data , so wait for rending finished*/
    for (;;) {
        if ( varMutex->renderCount()>0 ) {
            std::this_thread::sleep_for(10ns);
        } else {
            break;
        }
    }

    /*destory the contex*/
    if (this->getContex()) {
        this->getContex()->deleteLater();
    }

}

