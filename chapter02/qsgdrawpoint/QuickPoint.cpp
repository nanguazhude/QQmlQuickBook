/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include "QuickPoint.hpp"

namespace sstd {

    QuickPoint::QuickPoint(QQuickItem * p) :Super(p) {
        connect(this, &QuickPoint::pointColorChanged, this, &QuickPoint::ppp_ColorChanged, Qt::QueuedConnection);
        connect(this, &QuickPoint::pointSizeChanged, this, &QuickPoint::ppp_PointSizeChanged, Qt::QueuedConnection);
        this->setFlag(QQuickItem::ItemHasContents, true);
    }

    namespace {

        class PointNode : public QSGGeometryNode {
        public:

            class XYAndColor {
            public:
                GLfloat x;
                GLfloat y;
                GLfloat r;
                GLfloat g;
                GLfloat b;
                GLfloat a;
            };

            static const QSGGeometry::AttributeSet & getThisAttributeSet() {
                const static std::array<QSGGeometry::Attribute, 2> varAttributes{
                    QSGGeometry::Attribute::create(0, 2, GL_FLOAT, QSGGeometry::PositionAttribute),
                    QSGGeometry::Attribute::create(1, 4, GL_FLOAT, QSGGeometry::ColorAttribute),
                };
                const static auto varAns = QSGGeometry::AttributeSet{
                    2,
                    sizeof(XYAndColor),
                    varAttributes.data()
                };
                return varAns;
            }

            class Geometry : public QSGGeometry {
                using Super = QSGGeometry;
            public:
                Geometry() : QSGGeometry(getThisAttributeSet(),
                    1/*vertex count */,
                    0/*index count  */,
                    QSGGeometry::UnsignedShortType/*index type*/) {

                    this->setDrawingMode(QSGGeometry::DrawPoints);
                    this->updateData(1,1,1,1,1);

                }

                void updateData(qreal s, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {

                    this->setLineWidth(s)/*set point size*/;
                    auto varData = static_cast<XYAndColor *>(this->vertexData());
                    varData[0].x = 0;
                    varData[0].y = 0;

                    const constexpr auto varK = (1.0f / 255);
                    varData[0].r = varK * r;
                    varData[0].g = varK * g;
                    varData[0].b = varK * b;
                    varData[0].a = varK * a;
                     
                }

            private:
                SSTD_MEMORY_DEFINE(Geometry)
            };

            PointNode() {

                mmm_QSGGeometry = sstdNew<Geometry>();
                this->setGeometry( mmm_QSGGeometry );
                this->setFlag(QSGNode::OwnsGeometry);

                auto * varMaterial = new QSGVertexColorMaterial;
                this->setMaterial(varMaterial);
                this->setFlag(QSGNode::OwnsMaterial);

            }

            void updateData(qreal s, const QColor & varColor) {

                mmm_QSGGeometry->updateData(s,
                    varColor.red(),
                    varColor.green(),
                    varColor.blue(),
                    varColor.alpha());

                this->markDirty(QSGNode::DirtyGeometry);
            }
            
        private:
            Geometry * mmm_QSGGeometry = nullptr;
        private:
            SSTD_MEMORY_DEFINE(PointNode)
        };
    }

    QSGNode * QuickPoint::updatePaintNode(
        QSGNode * oldNode,
        QQuickItem::UpdatePaintNodeData *) {

        auto varPointNode = static_cast<PointNode*>(oldNode);
        if (oldNode == nullptr) {
            varPointNode = sstdNew<PointNode>();
        }

        varPointNode->updateData(mmm_PointSize, mmm_PointColor);

        return varPointNode;

    }

    void QuickPoint::ppp_ColorChanged() {
        this->update();
    }
    
    void QuickPoint::ppp_PointSizeChanged() {
        ppp_UpdatePointSizeAnsPosition();
    }

    void QuickPoint::ppp_UpdatePointSizeAnsPosition() {
        this->setWidth(mmm_PointSize);
        this->setHeight(mmm_PointSize);
    }

    static inline void registerThis() {
        qmlRegisterType<QuickPoint>("myqml.qsgdrawpoint", 1, 0, "QuickPoint");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*****/










