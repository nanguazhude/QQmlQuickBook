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
    QGraphicsPixmapItem * mmm_Left = nullptr;
    QGraphicsPixmapItem * mmm_Right = nullptr;
    QImage mmm_LeftImage;
    QImage mmm_RightImage;
    void _update_image_pos() const;
    std::int32_t mmm_DrawImdex = 0;
private:
    SSTD_MEMORY_QOBJECT_DEFINE(ImageView)
};

