/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include "QuickPoint.hpp"

namespace sstd {

    QuickPoint::QuickPoint(QQuickItem * p) :Super(p) {
        connect(this, &QuickPoint::pointColorChanged, this, &QuickPoint::ppp_ColorChanged, Qt::QueuedConnection);
        connect(this, &QuickPoint::pointPositionChanged, this, &QuickPoint::ppp_PositionChanged, Qt::QueuedConnection);
        connect(this, &QuickPoint::pointSizeChanged, this, &QuickPoint::ppp_PointSizeChanged, Qt::QueuedConnection);
        this->setFlag(QQuickItem::ItemHasContents ,true);
    }
       
    namespace {
        class PointNode : public QSGGeometry {
        public:
            PointNode() :QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),1){
                mmm_Point[0] = static_cast<GLfloat>(0);
                mmm_Point[1] = static_cast<GLfloat>(0);
                mmm_Point[2] = static_cast<GLfloat>(0);
                mmm_Point[3] = static_cast<GLfloat>(1);
                this->setDrawingMode(QSGGeometry::DrawPoints);
                {
                    mmm_PointNode = sstdNew<QSGGeometryNode>();
                    mmm_PointNode->setGeometry(this);
                    mmm_PointNode->setFlag(QSGNode::OwnsGeometry);
                }
                {
                    mmm_PointColor = sstdNew<QSGFlatColorMaterial>();
                    mmm_PointNode->setMaterial(mmm_PointColor);
                    mmm_PointNode->setFlag(QSGNode::OwnsMaterial);
                }
            }

            void updateData( qreal s, const QColor & varColor) {
                mmm_PointSize[0] = static_cast<GLfloat>(s);
                mmm_PointColor->setColor(varColor);
                mmm_PointNode->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
            }

        private:
            GLfloat mmm_Point[4];
            GLfloat mmm_PointSize[1];
            QSGGeometryNode *      mmm_PointNode = nullptr;
            QSGFlatColorMaterial * mmm_PointColor = nullptr;
        private:
            SSTD_MEMORY_DEFINE(PointNode)
        };
    }

    QSGNode * QuickPoint::updatePaintNode(
        QSGNode * oldNode,
        QQuickItem::UpdatePaintNodeData *) {
        
        PointNode * varNode = static_cast<PointNode *>(oldNode);
        if (oldNode==nullptr) {
            varNode = sstdNew<PointNode>();
        }
        
        varNode->updateData( this->mmm_PointSize,this->mmm_PointColor );
        
        return varNode;
    }

    void QuickPoint::ppp_ColorChanged() {
        this->update();
    }

    void QuickPoint::ppp_PositionChanged() {
        ppp_UpdatePointSizeAnsPosition();
    }

    void QuickPoint::ppp_PointSizeChanged() {
        ppp_UpdatePointSizeAnsPosition();
    }

    void QuickPoint::ppp_UpdatePointSizeAnsPosition() {
        auto varLeftTop = mmm_PointPosition;
        varLeftTop.setX(mmm_PointPosition.x() - mmm_PointSize * 0.5f);
        varLeftTop.setY(mmm_PointPosition.y() - mmm_PointSize * 0.5f);
        this->setX( varLeftTop.x() );
        this->setY( varLeftTop.y() );
        this->setWidth(mmm_PointSize);
        this->setHeight(mmm_PointSize);
    }

    static inline void registerThis() {
        qmlRegisterType<QuickPoint>("myqml.qsgdrawpoint", 1, 0, "QuickPoint");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*****/










