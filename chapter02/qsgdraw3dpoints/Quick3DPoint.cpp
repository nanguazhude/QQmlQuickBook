/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include "Quick3DPoint.hpp"
#include "WindowOpenGLDrawControl.hpp"
#include <cassert>

namespace sstd {
    namespace quick3dpoint {
        class QSGVertexColorMaterial : public QSGMaterial {
        public:
            QSGVertexColorMaterial();
            int compare(const QSGMaterial *other) const override;
        protected:
            QSGMaterialType *type() const override;
            QSGMaterialShader *createShader() const override;
            SSTD_MEMORY_DEFINE(QSGVertexColorMaterial)
        };
    }
}/****/

namespace  sstd {
    namespace quick3dpoint {
        class QSGVertexColorMaterialShader : public QSGMaterialShader {
        public:
            QSGVertexColorMaterialShader();

            void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
            char const *const *attributeNames() const override;

            static QSGMaterialType type;

            const char * fragmentShader() const override {
                return u8R"(

#version 330
in  vec4 color ;
out vec4 finalColor ;

void main() {

    vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
    float ansA = sqrt( dot(p,p) ) ;
    ansA = color.a * sin( 3 * ansA );
    if( ansA < 0 ){ ansA = 0 ; }
    else if( ansA < 0.2 ){ ansA *= ansA; ansA *= ansA; ansA *= ansA; }
    else{ ansA *= ansA; }

    finalColor = vec4( color.r , color.g , color.b , color.a * ansA ) ;

}

)";
            }

            const char * vertexShader() const override {
                return u8R"(
#version 330

in vec4 vertexCoord;
in vec4 vertexColor;

uniform mat4  matrix;
uniform float opacity;

out vec4 color;

void main() {
    gl_Position = matrix      * vertexCoord;
    color       = vertexColor * opacity;
}

)";
            }

            void activate() override;
            void deactivate() override;

        private:
            void initialize() override;
            int m_matrix_id;
            int m_opacity_id;
            using Super = QSGMaterialShader;
            SSTD_MEMORY_DEFINE(QSGVertexColorMaterialShader)
        };

        QSGMaterialType QSGVertexColorMaterialShader::type;

        QSGVertexColorMaterialShader::QSGVertexColorMaterialShader() { }


        void QSGVertexColorMaterialShader::updateState(const RenderState &state, QSGMaterial * /*newEffect*/, QSGMaterial *) {

            const auto varFunctions = state.context()->functions();
            varFunctions->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE );

            if (state.isOpacityDirty()) {
                program()->setUniformValue(m_opacity_id, state.opacity());
            }

            if (state.isMatrixDirty()) {
                program()->setUniformValue(m_matrix_id, state.combinedMatrix());
            }

        }

        void QSGVertexColorMaterialShader::activate() {
            return Super::activate();
        }

        void QSGVertexColorMaterialShader::deactivate() {
            const auto varFunctions = QOpenGLContext::currentContext()->functions();
            return Super::deactivate();
        }

        char const *const *QSGVertexColorMaterialShader::attributeNames() const {
            static const char *const attr[] = { "vertexCoord", "vertexColor", nullptr };
            return attr;
        }

        void QSGVertexColorMaterialShader::initialize() {
            m_matrix_id = program()->uniformLocation("matrix");
            m_opacity_id = program()->uniformLocation("opacity");
        }

        QSGVertexColorMaterial::QSGVertexColorMaterial() {
            setFlag(Blending, true);
        }

        int QSGVertexColorMaterial::compare(const QSGMaterial * other) const {
            assert(other && (other->type() == this->type()));
            if constexpr (true) {
                /*所有数据来自顶点着色器，因而片段着色器总是相同*/
                return 0;
            }
            else {
                return static_cast<int>
                    (reinterpret_cast<const char *>(dynamic_cast<const void *>(this)) -
                    (reinterpret_cast<const char *>(dynamic_cast<const void *>(other))));
            }
        }

        QSGMaterialType *QSGVertexColorMaterial::type() const {
            return &QSGVertexColorMaterialShader::type;
        }

        QSGMaterialShader *QSGVertexColorMaterial::createShader() const {
            return sstdNew<QSGVertexColorMaterialShader>();
        }

    }/*namespace quick3dpoint*/
}/*namespace sstd*/

namespace sstd {

    Quick3DPoint::Quick3DPoint(QQuickItem * p) :Super(p) {
        connect(this, &Quick3DPoint::pointColorChanged, this, &Quick3DPoint::ppp_ColorChanged, Qt::QueuedConnection);
        connect(this, &Quick3DPoint::pointSizeChanged, this, &Quick3DPoint::ppp_PointSizeChanged, Qt::QueuedConnection);
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
                    this->updateData(1, 1, 1, 1, 1);

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
                this->setGeometry(mmm_QSGGeometry);
                this->setFlag(QSGNode::OwnsGeometry);

                auto * varMaterial = sstdNew<sstd::quick3dpoint::QSGVertexColorMaterial>();
                this->setMaterial(varMaterial);
                this->setFlag(QSGNode::OwnsMaterial);

            }

            void updateData(qreal s, const QColor & varColor) {

                mmm_QSGGeometry->updateData(s,
                    varColor.red(),
                    varColor.green(),
                    varColor.blue(),
                    varColor.alpha());

                this->markDirty(QSGNode::DirtyGeometry | DirtyMaterial);
            }

        private:
            Geometry * mmm_QSGGeometry = nullptr;
        private:
            SSTD_MEMORY_DEFINE(PointNode)
        };
    }

    QSGNode * Quick3DPoint::updatePaintNode(
        QSGNode * oldNode,
        QQuickItem::UpdatePaintNodeData *) {
        
        auto varPointNode = static_cast<PointNode*>(oldNode);
        if (oldNode == nullptr) {
            varPointNode = sstdNew<PointNode>();
        }

        varPointNode->updateData(mmm_PointSize, mmm_PointColor);

        return varPointNode;

    }

    void Quick3DPoint::ppp_ColorChanged() {
        this->update();
    }

    void Quick3DPoint::ppp_PointSizeChanged() {
        ppp_UpdatePointSizeAnsPosition();
    }

    void Quick3DPoint::ppp_UpdatePointSizeAnsPosition() {
        this->setWidth(mmm_PointSize);
        this->setHeight(mmm_PointSize);
    }

    static inline void registerThis() {
        qmlRegisterType<Quick3DPoint>("myqml.qsgdrawpoint", 1, 0, "Quick3DPoint");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*****/











