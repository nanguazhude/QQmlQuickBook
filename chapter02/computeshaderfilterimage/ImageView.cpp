#include "ImageView.hpp"
#include <QtCore/qcoreevent.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtGui/qevent.h>

ImageView::ImageView() {
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    mmm_Left = sstdNew< QGraphicsPixmapItem >();
    mmm_Right = sstdNew< QGraphicsPixmapItem >();
    auto varScene = this->scene();
    if (varScene == nullptr) {
        varScene = sstdNew<QGraphicsScene>(this);
        this->setScene(varScene);
    }
    varScene->addItem(mmm_Left);
    varScene->addItem(mmm_Right);
    this->setBackgroundBrush(QColor(100, 100, 100));
    this->setMinimumHeight(128);
    this->setMinimumWidth(128);
    this->resize(1024, 399);
}

void ImageView::showImage(const QImage & s, const QImage & t) {
    mmm_LeftImage = s;
    mmm_RightImage = t;
    this->setWindowTitle( QString::number(++mmm_DrawImdex)+
        QStringLiteral(" : compute shader filter image") );
    _update_image_pos();
}

void ImageView::_update_image_pos() const {

    const auto varSR = this->sceneRect();
    const auto varSS = QSizeF{ 0.5* varSR.width(),varSR.height() } +QSizeF{ 1.0,1.0 };

#define ___ERROR_RETURN mmm_Left->setPixmap({});mmm_Right->setPixmap({}); return

    QSize varImageSize{ 1,1 };

    do {

        const auto varImageSizeSource = mmm_LeftImage.size();
        if (varImageSizeSource.width() < 1) {
            ___ERROR_RETURN;
        }
        if (varImageSizeSource.height() < 1) {
            ___ERROR_RETURN;
        }

        const auto varRateIS = varImageSizeSource.width() / double(varImageSizeSource.height());
        const auto varRateIT = varSS.width() / varSS.height();

        if (varRateIS == varRateIT) {
            varImageSize = varSS.toSize();
            break;
        }

        if (varRateIS > varRateIT) {
            const auto varHeight = static_cast<int>((varSS.width() / varImageSizeSource.width())
                *varImageSizeSource.height());
            varImageSize = { static_cast<int>(varSS.width()) ,varHeight };
        }
        else {
            const auto varWidth = static_cast<int>((varSS.height() / varImageSizeSource.height())
                * varImageSizeSource.width());
            varImageSize = { varWidth,static_cast<int>(varSS.height()) };
        }

    } while (false);

    {
        const auto varImage = mmm_LeftImage.scaled(
            varImageSize.width(),
            varImageSize.height(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mmm_Left->setPixmap(QPixmap::fromImage(varImage));
    }

    {
        const auto varImage = mmm_RightImage.scaled(
            varImageSize.width(),
            varImageSize.height(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mmm_Right->setPixmap(QPixmap::fromImage(varImage));
    }

    const auto varY = (-0.5)* varImageSize.height();
    mmm_Left->setPos(-varImageSize.width(), varY);
    mmm_Right->setPos(0, varY);

}

void ImageView::resizeEvent(QResizeEvent *event) {
    Super::resizeEvent(event);
    auto varScene = this->scene();
    if (varScene) {
        const QSizeF varSize = event->size();
        const QRectF varRect{
            varSize.width()*(-0.5),
            varSize.height()*(-0.5),
            varSize.width(),
            varSize.height() };
        if constexpr (false) {
            varScene->setSceneRect(varRect);
            varScene->sceneRectChanged(varRect);
        }
        else {
            this->setSceneRect(varRect);
            this->setAlignment(Qt::AlignCenter);
        }
        _update_image_pos();
    }
}

bool ImageView::event(QEvent * argEvent) {

    if (argEvent->type() == QEvent::Close) {
        this->deleteLater();
        argEvent->accept();
        return true;
    }

    return Super::event(argEvent);

}







