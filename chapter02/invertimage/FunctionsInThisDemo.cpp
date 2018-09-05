#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include <ThreadObject.hpp>
#include <functional>
#include <QtGui/qimage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qthreadpool.h>
#include <atomic>
#include "Window.hpp"

extern void constructMain();
extern void run(const QImage &, std::function<void(const QImage &)>);

namespace {

    class MainGlobal {
    public:

    private:
        SSTD_MEMORY_DEFINE(MainGlobal)
    };

    static std::atomic< MainGlobal *> $m$MainGlobal{ nullptr };

}/*namespace*/

/*在此创建共享opengl contex*/
void constructMain() {
    /*初始化主线程OpenGL Contex，其他线程的OpenGL Contex共享主线程的OpenGl Contex*/
    assert(QThread::currentThread() == qApp->thread());
    if ($m$MainGlobal)return;
    $m$MainGlobal = sstdNew<MainGlobal>();
    auto varMainContex = ThreadObject::getMainWindow()->getOpenGLContex();
    varMainContex->connect(varMainContex, &QOpenGLContext::aboutToBeDestroyed,
        varMainContex, []() {
        auto varData = $m$MainGlobal.load();
        $m$MainGlobal.store(nullptr);
        delete varData;
    }, Qt::DirectConnection);
       

}

namespace {

    void real_run(const QImage, std::function<void(const QImage &)> &) {
        /*此函数不能在main thread 里面run*/
        assert( QThread::currentThread()!=qApp->thread() );
        /*确保OpenGL环境初始化*/
        auto varThisThreadObject = ThreadObject::getInThisThread();
        (void)varThisThreadObject;

    }

    class RunTask : public QRunnable {
        const QImage $m$Image;
        std::function<void(const QImage &) > $m$AnsCallBack;
    public:
        void run() override {
            try {
                /*展开逻辑*/
                real_run($m$Image.convertToFormat(QImage::Format_RGBA8888), $m$AnsCallBack);
            }
            catch (...) {}
        }
        RunTask(const QImage & a, const std::function<void(const QImage &) > &b) :
            $m$Image(a),
            $m$AnsCallBack(b) {}
    private:
        SSTD_MEMORY_DEFINE(RunTask)
    };

}/**/

void run(const QImage & arg, std::function<void(const QImage &) >argAns) {
    if (bool(argAns) == false) { return; }
    if (arg.width() < 1) { return; }
    if (arg.height() < 1) { return; }
    QThreadPool::globalInstance()->start(sstdNew<RunTask>(arg, argAns));
}













