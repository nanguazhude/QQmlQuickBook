#include <ThreadObject.hpp>
#include <functional>
#include <QtGui/qimage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>

extern void constructMain();
extern void run(const QImage &, std::function<void(const QImage &)>);

/*在此创建共享opengl contex*/
void constructMain() {

}

namespace {
    void real_run(const QImage, std::function<void(const QImage &)> &) {

    }
}/**/

void run(const QImage & arg,std::function<void(const QImage &) >argAns) {
    if (bool(argAns) == false) { return; }
    if (arg.width() < 1) { return; }
    if (arg.height() < 1) { return; }
    /*确保OpenGL环境初始化*/
    auto varThisThreadObject = ThreadObject::getInThisThread();
    (void)varThisThreadObject;
    /*展开逻辑*/
    real_run(arg.convertToFormat(QImage::Format_RGBA8888), argAns);
}













