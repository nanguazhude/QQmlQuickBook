#include <sstd_glew.hpp>
#include "sstd_RenderThread.hpp"
#include <QtCore/qcoreapplication.h>
#include <ConstructQSurface.hpp>

namespace sstd {

    RenderThread::RenderThread(std::shared_ptr<sstd::RenderPack> arg) : mmm_RenderPack(std::move(arg)){
        this->moveToThread(qApp->thread());
        connect(this,&QThread::finished,this,&RenderThread::on_this_thread_finished,Qt::QueuedConnection);
        connect(qApp,&QCoreApplication::aboutToQuit,this,&QThread::quit,Qt::QueuedConnection);
    }

    RenderThread::~RenderThread() {

    }

    void RenderThread::quitRender(){
        std::call_once(this->mmm_CallOnce,[this](){
             mmm_RenderPack->targetWindowState->subRenderCount();
        });
    }

    void RenderThread::on_this_thread_finished() {
        this->deleteLater();
        quitRender();
    }

    void RenderThread::run() {
        /******************************************/
        mmm_RenderPack->sourceContex = sstd::make_unique<QOpenGLContext>()  ;
        mmm_RenderPack->sourceContex->setFormat(sstd::getDefaultOpenGLFormat());
        mmm_RenderPack->sourceContex->create();
        mmm_RenderPack->sourceContex->setShareContext(mmm_RenderPack->targetWindowContex);
        mmm_RenderPack->sourceContex->makeCurrent(mmm_RenderPack->sourceOffscreenSurface.get());
        glewInitialize();
        /******************************************/
        mmm_RenderPack->sourceViewControl->moveToThread(this);
        mmm_RenderPack->sourceViewControl->prepareThread(this);
        /******************************************/
        mmm_RenderPack->targetWindowState->addRenderCount();
        /******************************************/
        this->exec();
    }

} /* namespace sstd */


