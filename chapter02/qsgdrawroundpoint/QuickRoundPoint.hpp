#pragma once

#include <mutex>
#include <shared_mutex>
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickframebufferobject.h>

namespace sstd {

    class QuickRoundPoint :public QQuickFramebufferObject {
        Q_OBJECT
        Q_PROPERTY(qreal pointSize READ getPointSize WRITE setPointSize NOTIFY pointSizeChanged)
        Q_PROPERTY(QColor pointColor READ getPointColor WRITE setPointColor NOTIFY pointColorChanged)
    public:
        qreal getPointSize() const; 
        void setPointSize(qreal a);  
        Q_SIGNAL void pointSizeChanged();

        QColor getPointColor() const;  
        void setPointColor(const QColor & arg);  
        Q_SIGNAL void pointColorChanged();
    public:
        QuickRoundPoint(QQuickItem *parent = nullptr);
    protected:
        Renderer *createRenderer() const override;
    public:
        class DrawData {
        public:
            std::shared_mutex mmm_ReadWriteLock;
            QColor  mmm_PointColor;
            qreal   mmm_PointSize{ 1.0 };
        };
    private:
        std::shared_ptr<DrawData> mmm_DrawData;
        Q_SLOT void ppp_ColorChanged();
        Q_SLOT void ppp_PointSizeChanged();
        void ppp_UpdatePointSizeAnsPosition();
        using Super = QQuickFramebufferObject;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(QuickRoundPoint)
    };

}/*sstd*/














