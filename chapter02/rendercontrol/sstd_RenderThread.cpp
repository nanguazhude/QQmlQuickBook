#include "sstd_RenderThread.hpp"
#include <QtCore/qcoreapplication.h>

namespace sstd {

    RenderThread::RenderThread(std::shared_ptr<sstd::RenderPack> arg) : mmm_RenderPack(std::move(arg)){
        this->moveToThread(qApp->thread());
        connect(this,&QThread::finished,this,&RenderThread::on_this_thread_finished,Qt::QueuedConnection);
        connect(qApp,&QCoreApplication::aboutToQuit,this,&QThread::quit,Qt::QueuedConnection);
    }

    RenderThread::~RenderThread() {

    }

    void RenderThread::on_this_thread_finished() {
        this->deleteLater();
    }

    void RenderThread::run() {
        /******************************************/

        /******************************************/
        this->exec();
    }

} /* namespace sstd */


