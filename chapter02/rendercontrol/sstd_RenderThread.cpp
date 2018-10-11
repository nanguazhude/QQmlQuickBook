#include "sstd_RenderThread.hpp"
#include "sstd_Render.hpp"
#include <QtCore/qcoreapplication.h>
#include <ConstructQSurface.hpp>

namespace sstd::private_sstd {

    RenderThreadBasic::RenderThreadBasic() {
        assert( QThread::currentThread() == qApp->thread() );
        {/*创建Surface...*/
            this->mmm_Surface = sstd::createDefaultRenderPack<sstd::RenderControl>();
        }
        connect(this,&QThread::finished,this,&QObject::deleteLater)/*自删除*/;
        connect(qApp,&QCoreApplication::aboutToQuit, this, &QThread::quit)/*主线程退出则退出*/;
    }

    RenderThreadBasic::~RenderThreadBasic(){
    }

}/*sstd::private_sstd*/

















