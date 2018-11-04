#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>
#include <QtGui/qimage.h>

namespace sstd_private {

    class FFMPEGDecoderPrivate : public QObject {
        Q_OBJECT
    public:
        Q_SIGNAL void imageChanged(const QImage &);
        Q_SIGNAL void checkImageChange();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(FFMPEGDecoderPrivate)
    };


}/**/







