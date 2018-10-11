#include "sstd_RenderThread.hpp"
#include <QtCore/qcoreapplication.h>
#include <ConstructQSurface.hpp>

namespace sstd::private_sstd {

    namespace  {
        class SurfaceWrap   {
        public:
            SurfaceWrap(){
                assert( QThread::currentThread() == qApp->thread() );
                mmm_Data = sstdNew<QOffscreenSurface>();
            }
            ~SurfaceWrap(){
                mmm_Data->deleteLater();
            }
            QOffscreenSurface * mmm_Data;
        private:
            SSTD_MEMORY_DEFINE(SurfaceWrap)
        };
    }/*namespace*/

    RenderThreadBasic::RenderThreadBasic() {
        assert( QThread::currentThread() == qApp->thread() );
        {/*创建Surface...*/
            auto varData = sstd::make_shared<SurfaceWrap>() ;
            mmm_Surface = std::shared_ptr< QOffscreenSurface >(
                        varData,varData->mmm_Data);
        }
        connect(this,&QThread::finished,this,&QObject::deleteLater);
        /*创建Surface...*/
        mmm_Surface->setFormat(sstd::getDefaultOpenGLFormat());
        mmm_Surface->create();
    }

    RenderThreadBasic::~RenderThreadBasic(){
    }

}/*sstd::private_sstd*/

















