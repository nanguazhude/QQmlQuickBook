#include "sstd_Window.hpp"
#include <ConstructQSurface.hpp>

namespace sstd {

    Window::Window(){

        mmm_Mutex = sstd::make_shared<ClassState>();

        this->setMinimumHeight(128);
        this->setMinimumWidth(128);

        /*创建OpenGL渲染环境*/
        this->setSurfaceType(QSurface::OpenGLSurface);
        this->setFormat(sstd::getDefaultOpenGLFormat());

    }

    Window::~Window() {

        auto varMutex = mmm_Mutex;
        varMutex->setDestory();

        /*rending need this data , so wait for rending finished*/
        for (;;) {
            if (varMutex->renderCount() > 0) {
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

} // namespace sstd











