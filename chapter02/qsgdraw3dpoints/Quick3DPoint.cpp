/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include "Quick3DPoint.hpp"
#include <cassert>
#include <map>

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

        GLint mmm_GL_BLEND_SRC_RGB, mmm_GL_BLEND_SRC_ALPHA, mmm_GL_BLEND_DST_RGB, mmm_GL_BLEND_DST_ALPHA;
        void QSGVertexColorMaterialShader::updateState(const RenderState &state, QSGMaterial * /*newEffect*/, QSGMaterial *) {

            const auto varFunctions = state.context()->functions();
            /*进行RGB混色，但是不改变Alpha值*/
            /*因为QQuickWindow的背景颜色的Alpha是1，所以最终结果的Alpha也是1*/
            varFunctions->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

            if (state.isOpacityDirty()) {
                program()->setUniformValue(m_opacity_id, state.opacity());
            }

            if (state.isMatrixDirty()) {
                program()->setUniformValue(m_matrix_id, state.combinedMatrix());
            }

        }

        void QSGVertexColorMaterialShader::activate() {
            /*保存OpengGL状态*/
            const auto varFunctions = QOpenGLContext::currentContext()->functions();
            varFunctions->glGetIntegerv(GL_BLEND_SRC_RGB, &mmm_GL_BLEND_SRC_RGB);
            varFunctions->glGetIntegerv(GL_BLEND_SRC_ALPHA, &mmm_GL_BLEND_SRC_ALPHA);
            varFunctions->glGetIntegerv(GL_BLEND_DST_RGB, &mmm_GL_BLEND_DST_RGB);
            varFunctions->glGetIntegerv(GL_BLEND_DST_ALPHA, &mmm_GL_BLEND_DST_ALPHA);
            return Super::activate();
        }

        void QSGVertexColorMaterialShader::deactivate() {
            /*恢复OpengGL状态*/
            const auto varFunctions = QOpenGLContext::currentContext()->functions();
            varFunctions->glBlendFuncSeparate(
                mmm_GL_BLEND_SRC_RGB,
                mmm_GL_BLEND_DST_RGB,
                mmm_GL_BLEND_SRC_ALPHA,
                mmm_GL_BLEND_DST_ALPHA);
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
            if constexpr (false) {
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

    static inline std::pair<const QString, const QImage> operator""_load_qimage(const char * a, std::size_t b) {
        const QString varFileName(QString::fromUtf8(a, static_cast<int>(b)));
        const QDir varDir{ qApp->applicationDirPath() };
        const auto varImageFileName = varDir.absoluteFilePath(QStringLiteral("") + QChar('/') + varFileName);
        QImage varImage{ varImageFileName };
        varImage = varImage.convertToFormat(QImage::Format_RGBA8888);
        return { varFileName,std::move(varImage) };
    }

    static inline void loadResource(const QString & arg, const QImage * & source) {
        using map = std::map<QString,
            const QImage,
            std::less<void>,
            sstd::allocator< std::pair<const QString, const QImage> > >;
        const static map varResources = []()->map {
            map varAns;
            varAns.insert("00.png"_load_qimage);
            varAns.insert("01.png"_load_qimage);
            varAns.insert("02.png"_load_qimage);
            varAns.insert("03.png"_load_qimage);
            varAns.insert("04.png"_load_qimage);
            varAns.insert("05.png"_load_qimage);
            varAns.insert("06.png"_load_qimage);
            varAns.insert("07.png"_load_qimage);
            varAns.insert("08.png"_load_qimage);
            varAns.insert("09.png"_load_qimage);
            varAns.insert("10.png"_load_qimage);
            varAns.insert("11.png"_load_qimage);
            varAns.insert("12.png"_load_qimage);
            varAns.insert("13.png"_load_qimage);
            varAns.insert("14.png"_load_qimage);
            varAns.insert("15.png"_load_qimage);
            varAns.insert("16.png"_load_qimage);
            varAns.insert("17.png"_load_qimage);
            varAns.insert("18.png"_load_qimage);
            varAns.insert("19.png"_load_qimage);
            varAns.insert("20.png"_load_qimage);
            varAns.insert("21.png"_load_qimage);
            varAns.insert("22.png"_load_qimage);
            varAns.insert("23.png"_load_qimage);
            varAns.insert("24.png"_load_qimage);
            varAns.insert("25.png"_load_qimage);
            varAns.insert("26.png"_load_qimage);
            varAns.insert("27.png"_load_qimage);
            varAns.insert("28.png"_load_qimage);
            varAns.insert("29.png"_load_qimage);
            varAns.insert("30.png"_load_qimage);
            varAns.insert("31.png"_load_qimage);
            varAns.insert("32.png"_load_qimage);
            varAns.insert("33.png"_load_qimage);
            varAns.insert("34.png"_load_qimage);
            varAns.insert("35.png"_load_qimage);
            varAns.insert("36.png"_load_qimage);
            varAns.insert("37.png"_load_qimage);
            varAns.insert("38.png"_load_qimage);
            varAns.insert("39.png"_load_qimage);
            varAns.insert("40.png"_load_qimage);
            varAns.insert("41.png"_load_qimage);
            varAns.insert("42.png"_load_qimage);
            varAns.insert("43.png"_load_qimage);
            varAns.insert("44.png"_load_qimage);
            varAns.insert("45.png"_load_qimage);
            varAns.insert("46.png"_load_qimage);
            varAns.insert("47.png"_load_qimage);
            varAns.insert("48.png"_load_qimage);
            varAns.insert("49.png"_load_qimage);
            varAns.insert("50.png"_load_qimage);
            varAns.insert("51.png"_load_qimage);
            varAns.insert("52.png"_load_qimage);
            varAns.insert("53.png"_load_qimage);
            varAns.insert("54.png"_load_qimage);
            varAns.insert("55.png"_load_qimage);
            varAns.insert("56.png"_load_qimage);
            varAns.insert("57.png"_load_qimage);
            varAns.insert("58.png"_load_qimage);
            varAns.insert("59.png"_load_qimage);
            varAns.insert("60.png"_load_qimage);
            varAns.insert("61.png"_load_qimage);
            varAns.insert("62.png"_load_qimage);
            varAns.insert("63.png"_load_qimage);
            varAns.insert("64.png"_load_qimage);
            varAns.insert("65.png"_load_qimage);
            varAns.insert("66.png"_load_qimage);
            varAns.insert("67.png"_load_qimage);
            varAns.insert("68.png"_load_qimage);
            varAns.insert("69.png"_load_qimage);
            varAns.insert("70.png"_load_qimage);
            varAns.insert("71.png"_load_qimage);
            varAns.insert("72.png"_load_qimage);
            varAns.insert("73.png"_load_qimage);
            varAns.insert("74.png"_load_qimage);
            varAns.insert("75.png"_load_qimage);
            varAns.insert("76.png"_load_qimage);
            varAns.insert("77.png"_load_qimage);
            return std::move(varAns);
        }();
        auto varPos = varResources.find(arg);
        if (varPos == varResources.end()) {
            varPos = varResources.begin();
        }
        source = &(varPos->second);
    }

    void Quick3DPoint::ppp_ImageIndexChanged() {
        /***************************************************/
        //这仅仅是一个演示示例，仅仅支持受限的资源索引
        loadResource(this->mmm_ImageIndex, this->mmm_ImageSource);
        /***************************************************/
        this->update();
    }

    static inline void registerThis() {
        {/*强制资源载入*/
            const QImage * arg = nullptr;
            loadResource(QStringLiteral("any"), arg);
        }
        qmlRegisterType<Quick3DPoint>("myqml.qsgdrawpoint", 1, 0, "Quick3DPoint");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*****/











