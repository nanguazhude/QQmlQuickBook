#pragma once

#include <sstd_memory.hpp>
#include <QtGui/qimage.h>
#include <QtWidgets/qgraphicsview.h>
class QGraphicsPixmapItem;

class ImageView : public QGraphicsView {
    Q_OBJECT
public:
    ImageView();
    void showImage(const QImage &,const QImage &);
protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    using Super = QGraphicsView;
    QGraphicsPixmapItem * $m$Left = nullptr;
    QGraphicsPixmapItem * $m$Right = nullptr;
    QImage $m$LeftImage;
    QImage $m$RightImage;
    void _update_image_pos() const;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(ImageView)
};

