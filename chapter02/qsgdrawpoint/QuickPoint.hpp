#pragma once

#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>

namespace sstd{

    class QuickPoint :public QQuickItem{
        Q_OBJECT
        Q_PROPERTY(qreal pointSize READ getPointSize WRITE setPointSize NOTIFY pointSizeChanged)
        Q_PROPERTY(QPointF pointPosition READ getPointPosition WRITE setPointPosition NOTIFY pointPositionChanged)
        Q_PROPERTY(QColor pointColor READ getPointColor WRITE setPointColor NOTIFY pointColorChanged)
    public:
        qreal getPointSize() const { return mmm_PointSize; }
        void setPointSize(qreal a) { if (a < 0)a = 0; if (a == mmm_PointSize)return; mmm_PointSize = a; pointSizeChanged(); }
        Q_SIGNAL void pointSizeChanged();

        QPointF getPointPosition() const { return mmm_PointPosition; }
        void setPointPosition(const QPointF & arg){if(arg==mmm_PointPosition)return;mmm_PointPosition = arg; pointPositionChanged();}
        Q_SIGNAL void pointPositionChanged();

        QColor getPointColor() const { return mmm_PointColor; }
        void setPointColor(const QColor & arg){ if(arg==mmm_PointColor)return;mmm_PointColor=arg;pointColorChanged(); }
        Q_SIGNAL void pointColorChanged();
    public:
        QuickPoint(QQuickItem *parent = nullptr);
    protected:
        QSGNode * updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
    private:
        QPointF mmm_PointPosition;
        QColor  mmm_PointColor;
        qreal   mmm_PointSize{1.0};
        using Super = QQuickItem;
        Q_SLOT void ppp_ColorChanged();
        Q_SLOT void ppp_PositionChanged();
        Q_SLOT void ppp_PointSizeChanged();
        void ppp_UpdatePointSizeAnsPosition();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(QuickPoint)
    };



}/*sstd*/














