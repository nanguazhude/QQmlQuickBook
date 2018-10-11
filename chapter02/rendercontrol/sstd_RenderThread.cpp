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
        connect(this,&QThread::finished,this,&QObject::deleteLater);
    }

    RenderThreadBasic::~RenderThreadBasic(){
    }

}/*sstd::private_sstd*/

















