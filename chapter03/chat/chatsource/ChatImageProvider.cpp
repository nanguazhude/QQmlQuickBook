#include "ChatImageProvider.hpp"
#include <QtGui/qimagereader.h>
#include <QtCore/qdebug.h>

namespace sstd {

    ChatImageProvider::ChatImageProvider() :
        Super(QQuickImageProvider::Image) {
    }
    
    QImage ChatImageProvider::requestImage(const QString &id, 
        QSize *size, 
        const QSize &requestedSize) {

        int varWidth = 10;
        int varHeight = 10;

        {
            QImageReader varReader{ id };
            const auto varSize = varReader.size();
            varWidth = std::max(varWidth, varSize.width());
            varHeight = std::max(varHeight, varSize.height());
            if ( (varSize.width()<1)||(varSize.height()<1) ) {
                qDebug() << "can not load" << id;
            }
        }

        varWidth = std::max(varWidth, requestedSize.width());
        varHeight = std::max(varHeight, requestedSize.height());

        if (size) { *size = { varWidth,varHeight }; }

        /*这仅仅是一个占位符，返回一个透明像素即可*/
        const static QImage varImage = []() {
            QImage varImage{ 1 ,1 ,QImage::Format_RGBA8888 };
            varImage.fill(QColor(0, 0, 0, 0));
            return std::move(varImage);
        }();
        return varImage;

    }

}/*namespace sstd*/




















