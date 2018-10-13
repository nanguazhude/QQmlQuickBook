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

    /*rending need this data , so wait for rending finished*/
    for (;;) {
        {
            std::unique_lock varDestoryLock{ *varMutex };
            varMutex->isDesotry = true;
            if (varMutex->isRending > 0) {
            } else {
                break;
            }
        }
        std::this_thread::sleep_for(10ns);
    }

    /*destory the contex*/
    if (this->getContex()) {
        this->getContex()->deleteLater();
    }

}

