#pragma once

#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
class QImage;

namespace sstd{

    class QuickTexturePoint :public QQuickItem {
        Q_OBJECT
        Q_PROPERTY(qreal   pointSize  READ getPointSize  WRITE setPointSize  NOTIFY pointSizeChanged)
        Q_PROPERTY(QColor  pointColor READ getPointColor WRITE setPointColor NOTIFY pointColorChanged)
        Q_PROPERTY(QString imageIndex READ getImageIndex WRITE setImageIndex NOTIFY imageIndexChanged)
    public:
        qreal getPointSize() const { return mmm_PointSize; }
        void setPointSize(qreal a) { if (a < 0)a = 0; if (a == mmm_PointSize)return; mmm_PointSize = a; pointSizeChanged(); }
        Q_SIGNAL void pointSizeChanged();

        QColor getPointColor() const { return mmm_PointColor; }
        void setPointColor(const QColor & arg){ if(arg==mmm_PointColor)return;mmm_PointColor=arg;pointColorChanged(); }
        Q_SIGNAL void pointColorChanged();

        QString getImageIndex() const { return mmm_ImageIndex; }
        void setImageIndex(const QString &arg){if(arg==mmm_ImageIndex){return;}mmm_ImageIndex=arg;imageIndexChanged();}
        Q_SIGNAL void imageIndexChanged();
    public:
        QuickTexturePoint(QQuickItem *parent = nullptr);
    protected:
        QSGNode * updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
    private:
        QColor  mmm_PointColor;
        qreal   mmm_PointSize{1.0};
        QString mmm_ImageIndex{QStringLiteral("000.png")};
        const QImage * mmm_ImageSource = nullptr;
        using Super = QQuickItem;
        Q_SLOT void ppp_ColorChanged();
        Q_SLOT void ppp_PointSizeChanged();
        Q_SLOT void ppp_ImageIndexChanged();
        void ppp_UpdatePointSizeAnsPosition();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(QuickTexturePoint)
    };

}/*sstd*/














