#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include <ThreadObject.hpp>
#include <functional>
#include <QtGui/qimage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qthreadpool.h>

extern void constructMain();
extern void run(const QImage &, std::function<void(const QImage &)>);

namespace {




    class Global {
    public:
        static Global * instance() {
            static auto * varGlobal = sstdNew<Global>();
            return varGlobal;
        }
    private:
        SSTD_MEMORY_DEFINE(Global)
    };
         
}/*namespace*/

/*在此创建共享opengl contex*/
void constructMain() {

}

namespace {

    void real_run(const QImage, std::function<void(const QImage &)> &) {
        /*确保OpenGL环境初始化*/
        auto varThisThreadObject = ThreadObject::getInThisThread();
        (void)varThisThreadObject;

    }

    class RunTask : public QRunnable{
        const QImage $m$Image;
        std::function<void(const QImage &) > $m$AnsCallBack;
    public:
        void run() override {
            try {
                /*展开逻辑*/
                real_run($m$Image.convertToFormat(QImage::Format_RGBA8888), $m$AnsCallBack);
            }
            catch (...){}
        }
        RunTask(const QImage & a, const std::function<void(const QImage &) > &b):
            $m$Image(a),
            $m$AnsCallBack(b){}
    private:
        SSTD_MEMORY_DEFINE(RunTask)
    };

}/**/

void run(const QImage & arg,std::function<void(const QImage &) >argAns) {
    if (bool(argAns) == false) { return; }
    if (arg.width() < 1) { return; }
    if (arg.height() < 1) { return; }
    QThreadPool::globalInstance()->start(sstdNew<RunTask>(arg,argAns));
}













