#pragma once 

#include "sstd_RenderPack.hpp"
#include <QtCore/qthread.h>

namespace sstd{
     
    class RenderThread : public QThread {
        Q_OBJECT
    public:
        RenderThread();

    };

}
















