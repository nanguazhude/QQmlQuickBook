#include "ImageView.hpp"
#include <QtCore/qcoreevent.h>
#include <QtWidgets/qgraphicsscene.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtGui/qevent.h>


ImageView::ImageView() {
    this->setAttribute(Qt::WA_DeleteOnClose, false);
    $m$Left = sstdNew< QGraphicsPixmapItem >();
    $m$Right = sstdNew< QGraphicsPixmapItem >();
    auto varScene = this->scene();
    if (varScene == nullptr) {
        varScene = sstdNew<QGraphicsScene>(this);
        this->setScene(varScene);
    }
    varScene->addItem($m$Left);
    varScene->addItem($m$Right);
    this->setBackgroundBrush(QColor(100, 100, 100));
    this->setMinimumHeight(128);
    this->setMinimumWidth(128);
    this->resize( 512,512 );
}

void ImageView::showImage(const QImage & s, const QImage & t) {
    $m$LeftImage = s;
    $m$RightImage = t;
    _update_image_pos();
}

void ImageView::_update_image_pos() const {

    const auto varSR = this->sceneRect();
    const auto varSS = (varSR.size() * 0.5) + QSizeF{ 1.0,1.0 };

#define ___ERROR_RETURN $m$Left->setPixmap({});$m$Right->setPixmap({}); return

    QSize varImageSize{ 1,1 };

    do {
        const auto varImageSizeSource = $m$LeftImage.size();
        if (varImageSizeSource.width() < 1) {
            ___ERROR_RETURN;
        }
        if (varImageSizeSource.height() < 1) {
            ___ERROR_RETURN;
        }

        const auto varRateIS = varImageSizeSource.width() / double(varImageSizeSource.height());
        const auto varRateIT = varSS.width() / varSS.height();

        if (varRateIS == varRateIT) {
            varImageSize = varImageSizeSource;
            break;
        }

        if (varRateIS > varRateIT) {
            const auto varHeight = static_cast<int>((varSS.width() / varImageSizeSource.width())
                *varImageSizeSource.height());
            varImageSize = { varImageSizeSource.width() ,varHeight };
        }
        else {
            const auto varWidth = static_cast<int>((varSS.height() / varImageSizeSource.height())
                * varImageSizeSource.width());
            varImageSize = { varWidth,varImageSizeSource.height() };
        }

    } while (false);

    {
        const auto varImage = $m$LeftImage.scaled(varImageSize.width(), varImageSize.height(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        $m$Left->setPixmap(QPixmap::fromImage(varImage));
    }

    {
        const auto varImage = $m$RightImage.scaled(varImageSize.width(), varImageSize.height(),
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        $m$Right->setPixmap(QPixmap::fromImage(varImage));
    }

    const auto varY = varImageSize.height()*0.5;
    $m$Left->setPos(-varImageSize.width(), varY);
    $m$Right->setPos(0, varY);
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
        this->hide();
        argEvent->accept();
        return true;
    }

    return Super::event(argEvent);

}







