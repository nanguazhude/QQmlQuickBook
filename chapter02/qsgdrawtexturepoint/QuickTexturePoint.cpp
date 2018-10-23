/*这里应当有预编译头文件优化...*/
#include <QtCore>
#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include "QuickTexturePoint.hpp"
#include <cassert>
#include <map>

namespace sstd {
    namespace texturepoint {
        class TexturePointMaterialShader;
        class TexturePointMaterial : public QSGMaterial {
        public:
            TexturePointMaterial(QQuickItem *);
            int compare(const QSGMaterial *other) const override;
            void loadImage(qreal, const QImage *);
            void reloadImage();
            void releaseTexture();
        protected:
            friend class TexturePointMaterialShader;
            qreal mmm_Rotate = 0;
            QQuickItem * const super;
            std::unique_ptr< QSGTexture   > mmm_Texture{ nullptr };
            std::shared_ptr< const QImage > mmm_Image;
            QSGMaterialType *type()                   const override;
            QSGMaterialShader *createShader()         const override;
            SSTD_MEMORY_DEFINE(TexturePointMaterial)
        };
    }
}/****/

namespace  sstd {
    namespace texturepoint {

        class TexturePointMaterialShader : public QSGMaterialShader {
        public:
            TexturePointMaterialShader(TexturePointMaterial *);

            void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect) override;
            char const *const *attributeNames() const override;

            static QSGMaterialType type;

            const char * fragmentShader() const override {
                return u8R"(

#version 450
in  vec4 color             ;
out vec4 finalColor        ;
uniform mat2 pointRotateMat;

uniform sampler2D qt_Texture;

void main() {
    
    vec4 texture_color = texture2D( qt_Texture , pointRotateMat * (gl_PointCoord-vec2(0.5))+vec2(0.5) );
           
    vec2 p =    gl_PointCoord  * 2.0 - vec2(1.0)  ;
    float ansA = 1 -  sqrt( dot(p,p) )  ;
    if( ansA > 0.3 ){ 
        ansA = 1;
     } else if( ansA > 0.25) {
        ansA = 2*ansA + 0.4 ;
    }else if( ansA > 0.22  ) {
        ansA = 6.667*ansA -0.7667 ;
    }
    else if( ansA > 0.2) {
        ansA = 35*ansA - 7 ;
    } 
    else{ 
        ansA = 0;
    }
    
    if( texture_color.a < 0.00001 ){
        finalColor = vec4( color.r , color.g , color.b , ansA )/*进行背景颜色替换*/;
    }else{
        finalColor = vec4( texture_color.r , texture_color.g , texture_color.b , ansA );
    }

}

)";
            }

            const char * vertexShader() const override {
                return u8R"(
#version 450

in vec4 vertexCoord;
in vec4 vertexColor;

uniform mat4  matrix ;
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
            int m_rotate_matrix_id;
            GLint mmm_GL_BLEND_SRC_RGB, mmm_GL_BLEND_SRC_ALPHA, mmm_GL_BLEND_DST_RGB, mmm_GL_BLEND_DST_ALPHA;
            TexturePointMaterial * m_parent = nullptr;
            using Super = QSGMaterialShader;
            SSTD_MEMORY_DEFINE(TexturePointMaterialShader)
        };

        QSGMaterialType TexturePointMaterialShader::type;

        TexturePointMaterialShader::TexturePointMaterialShader(TexturePointMaterial * arg) : m_parent(arg) {
        }

        void TexturePointMaterialShader::updateState(
            const RenderState &state,
            QSGMaterial * varNew,
            QSGMaterial * varOld) {

            const auto varFunctions = state.context()->functions();
            /*进行RGB混色，但是不改变Alpha值*/
            /*因为QQuickWindow的背景颜色的Alpha是1，所以最终结果的Alpha也是1*/
            varFunctions->glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);

            /******************************************************/
            auto varTexture = static_cast<TexturePointMaterial*>(varNew)->mmm_Texture.get();
            if (varTexture) {
                if (varOld == nullptr) {
                    varTexture->bind();
                } else if (false == bool(static_cast<TexturePointMaterial*>(varOld)->mmm_Image)) {
                    varTexture->bind();
                } else if (static_cast<TexturePointMaterial*>(varOld)->mmm_Image->bits() !=
                    static_cast<TexturePointMaterial*>(varNew)->mmm_Image->bits()) {
                    varTexture->bind();
                } else {
                    varTexture->updateBindOptions();
                }
            }
            /******************************************************/

            if (state.isOpacityDirty()) {
                program()->setUniformValue(m_opacity_id, state.opacity());
            }

            if (state.isMatrixDirty()) {
                program()->setUniformValue(m_matrix_id, state.combinedMatrix());
            }


            QMatrix2x2 varTo;
            {
                const auto varAngle = static_cast<GLfloat>(static_cast<TexturePointMaterial*>(varNew)->mmm_Rotate *(3.141592654 / 180));
                GLfloat varToTmp[4];
                varToTmp[2] = ::sinf(varAngle);
                varToTmp[3] = ::cosf(varAngle);
                varToTmp[0] = varToTmp[3];
                varToTmp[1] = -varToTmp[2];
                varTo = QMatrix2x2{ varToTmp };
            }
            program()->setUniformValue(m_rotate_matrix_id, varTo);

        }

        void TexturePointMaterialShader::activate() {
            /*保存OpengGL状态*/
            const auto varFunctions = QOpenGLContext::currentContext()->functions();
            varFunctions->glGetIntegerv(GL_BLEND_SRC_RGB, &mmm_GL_BLEND_SRC_RGB);
            varFunctions->glGetIntegerv(GL_BLEND_SRC_ALPHA, &mmm_GL_BLEND_SRC_ALPHA);
            varFunctions->glGetIntegerv(GL_BLEND_DST_RGB, &mmm_GL_BLEND_DST_RGB);
            varFunctions->glGetIntegerv(GL_BLEND_DST_ALPHA, &mmm_GL_BLEND_DST_ALPHA);

            /*尝试重新加载资源到显卡*/
            if (false == bool(this->m_parent->mmm_Texture)) {
                m_parent->reloadImage();
            }

            return Super::activate();
        }

        void TexturePointMaterialShader::deactivate() {

            /*恢复OpengGL状态*/
            const auto varFunctions = QOpenGLContext::currentContext()->functions();
            varFunctions->glBlendFuncSeparate(
                mmm_GL_BLEND_SRC_RGB,
                mmm_GL_BLEND_DST_RGB,
                mmm_GL_BLEND_SRC_ALPHA,
                mmm_GL_BLEND_DST_ALPHA);

            /*释放显卡Texture资源*/
            this->m_parent->releaseTexture();

            return Super::deactivate();
        }

        char const *const *TexturePointMaterialShader::attributeNames() const {
            static const char *const attr[] = { "vertexCoord", "vertexColor", nullptr };
            return attr;
        }

        void TexturePointMaterialShader::initialize() {
            m_matrix_id = program()->uniformLocation("matrix");
            m_opacity_id = program()->uniformLocation("opacity");
            m_rotate_matrix_id = program()->uniformLocation("pointRotateMat");
        }

        TexturePointMaterial::TexturePointMaterial(QQuickItem * arg) : super(arg) {
            setFlag(Blending, true);
            setFlag(RequiresDeterminant, true);
        }

        void TexturePointMaterial::loadImage(qreal r, const QImage *arg) {
            mmm_Rotate = r;
            /***********************************************/
            //比较两个图片是否相等
            if (bool(mmm_Image) && ((mmm_Image->bits()) == (arg->bits()))) {
                return;
            }
            mmm_Image = std::make_shared<const QImage>(*arg);
            reloadImage();
        }

        void TexturePointMaterial::reloadImage() {
            auto varWindow = super->window();
            releaseTexture();
            mmm_Texture.reset(varWindow->createTextureFromImage(*mmm_Image));
            mmm_Texture->setFiltering(QSGTexture::Nearest);

        }

        int TexturePointMaterial::compare(const QSGMaterial * other) const {
            assert(other && (other->type() == this->type()));
            if constexpr (false) {
                /*所有数据来自顶点着色器，因而片段着色器总是相同*/
                return 0;
            } else {
                auto varOther = static_cast<const TexturePointMaterial *>(other);
                if (this->mmm_Rotate != varOther->mmm_Rotate) {
                    return (this->mmm_Rotate - varOther->mmm_Rotate) < 0.001 ? -1 : 1;
                }
                return static_cast<int>((this->mmm_Image ? this->mmm_Image->bits() : nullptr) -
                    (varOther->mmm_Image ? varOther->mmm_Image->bits() : nullptr));
            }
        }

        QSGMaterialType *TexturePointMaterial::type() const {
            return &TexturePointMaterialShader::type;
        }

        QSGMaterialShader *TexturePointMaterial::createShader() const {
            return sstdNew<TexturePointMaterialShader>(const_cast<TexturePointMaterial*>(this));
        }

        void TexturePointMaterial::releaseTexture() {
            if (this->mmm_Texture) {
                mmm_Texture.release()->deleteLater();
            }
        }

    }/*namespace quick3dpoint*/
}/*namespace sstd*/

namespace sstd {

    QuickTexturePoint::QuickTexturePoint(QQuickItem * p) :Super(p) {
        connect(this, &QuickTexturePoint::imageIndexChanged, this, &QuickTexturePoint::ppp_ImageIndexChanged, Qt::QueuedConnection);
        connect(this, &QuickTexturePoint::pointColorChanged, this, &QuickTexturePoint::ppp_ColorChanged, Qt::QueuedConnection);
        connect(this, &QuickTexturePoint::pointSizeChanged, this, &QuickTexturePoint::ppp_PointSizeChanged, Qt::QueuedConnection);
        this->setFlag(QQuickItem::ItemHasContents, true);
        ppp_ImageIndexChanged();
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
                    QSGGeometry::Attribute::createWithAttributeType(0, 2, GL_FLOAT, QSGGeometry::PositionAttribute),
                    QSGGeometry::Attribute::createWithAttributeType(1, 4, GL_FLOAT, QSGGeometry::ColorAttribute),
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
                    varData[0].x = s * 0.5f;
                    varData[0].y = s * 0.5f;

                    const constexpr auto varK = (1.0f / 255);
                    varData[0].r = varK * r;
                    varData[0].g = varK * g;
                    varData[0].b = varK * b;
                    varData[0].a = varK * a;

                }

            private:
                SSTD_MEMORY_DEFINE(Geometry)
            };

            QQuickItem * const super;
            PointNode(QQuickItem * arg) : super(arg) {

                mmm_QSGGeometry = sstdNew<Geometry>();
                this->setGeometry(mmm_QSGGeometry);
                this->setFlag(QSGNode::OwnsGeometry);

                mmm_Material = sstdNew<sstd::texturepoint::TexturePointMaterial>(arg);
                this->setMaterial(mmm_Material);
                this->setFlag(QSGNode::OwnsMaterial);

            }

            void updateData(qreal r, qreal s, const QColor & varColor, const QImage * varImage) {

                /*更新顶点着色器*/
                mmm_QSGGeometry->updateData(s,
                    varColor.red(),
                    varColor.green(),
                    varColor.blue(),
                    varColor.alpha());

                /*更新片段着色器*/
                mmm_Material->loadImage(r, varImage);

                this->markDirty(QSGNode::DirtyGeometry | DirtyMaterial);

            }

        private:
            Geometry * mmm_QSGGeometry = nullptr;
            sstd::texturepoint::TexturePointMaterial * mmm_Material = nullptr;
        private:
            SSTD_MEMORY_DEFINE(PointNode)
        };
    }

    void QuickTexturePoint::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) {
        if (change == QQuickItem::ItemRotationHasChanged) {
            this->update();
        }
        return Super::itemChange(change, value);
    }

    QSGNode * QuickTexturePoint::updatePaintNode(
        QSGNode * oldNode,
        QQuickItem::UpdatePaintNodeData *) {

        auto varPointNode = static_cast<PointNode*>(oldNode);
        if (oldNode == nullptr) {
            varPointNode = sstdNew<PointNode>(this);
        }

        assert(mmm_ImageSource);
        varPointNode->updateData(this->rotation(), mmm_PointSize, mmm_PointColor, mmm_ImageSource);

        return varPointNode;

    }

    void QuickTexturePoint::ppp_ColorChanged() {
        this->update();
    }

    void QuickTexturePoint::ppp_PointSizeChanged() {
        ppp_UpdatePointSizeAnsPosition();
    }

    void QuickTexturePoint::ppp_UpdatePointSizeAnsPosition() {
        this->setWidth(mmm_PointSize);
        this->setHeight(mmm_PointSize);
    }

    static inline std::pair<const QString, const QImage> operator""_load_qimage(const char * a, std::size_t b) {
        const QString varFileName(QString::fromUtf8(a, static_cast<int>(b)));
        const QDir varDir{ qApp->applicationDirPath() };
        const auto varImageFileName = varDir.absoluteFilePath(QStringLiteral("myqml/qsgdrawtexturepoint/") + varFileName);
        QImage varImage{ varImageFileName };
        varImage = varImage.convertToFormat(QImage::Format_RGBA8888);
        assert(varImage.width() > 0);
        assert(varImage.height() > 0);
        return { varFileName,std::move(varImage) };
    }

    static inline void loadResource(const QString & arg, const QImage * & source) {
        using map = std::map<QString,
            const QImage,
            std::less<void>,
            sstd::allocator< std::pair<const QString, const QImage> > >;
        const static map varResources = []()->map {
            map varAns;
            varAns.insert("000.png"_load_qimage);
            varAns.insert("001.png"_load_qimage);
            varAns.insert("002.png"_load_qimage);
            varAns.insert("003.png"_load_qimage);
            varAns.insert("004.png"_load_qimage);
            varAns.insert("005.png"_load_qimage);
            varAns.insert("006.png"_load_qimage);
            varAns.insert("007.png"_load_qimage);
            varAns.insert("008.png"_load_qimage);
            varAns.insert("009.png"_load_qimage);
            varAns.insert("010.png"_load_qimage);
            varAns.insert("011.png"_load_qimage);
            varAns.insert("012.png"_load_qimage);
            varAns.insert("013.png"_load_qimage);
            varAns.insert("014.png"_load_qimage);
            varAns.insert("015.png"_load_qimage);
            varAns.insert("016.png"_load_qimage);
            varAns.insert("017.png"_load_qimage);
            varAns.insert("018.png"_load_qimage);
            varAns.insert("019.png"_load_qimage);
            varAns.insert("020.png"_load_qimage);
            varAns.insert("021.png"_load_qimage);
            varAns.insert("022.png"_load_qimage);
            varAns.insert("023.png"_load_qimage);
            varAns.insert("024.png"_load_qimage);
            varAns.insert("025.png"_load_qimage);
            varAns.insert("026.png"_load_qimage);
            varAns.insert("027.png"_load_qimage);
            varAns.insert("028.png"_load_qimage);
            varAns.insert("029.png"_load_qimage);
            varAns.insert("030.png"_load_qimage);
            varAns.insert("031.png"_load_qimage);
            varAns.insert("032.png"_load_qimage);
            varAns.insert("033.png"_load_qimage);
            varAns.insert("034.png"_load_qimage);
            varAns.insert("035.png"_load_qimage);
            varAns.insert("036.png"_load_qimage);
            varAns.insert("037.png"_load_qimage);
            varAns.insert("038.png"_load_qimage);
            varAns.insert("039.png"_load_qimage);
            varAns.insert("040.png"_load_qimage);
            varAns.insert("041.png"_load_qimage);
            varAns.insert("042.png"_load_qimage);
            varAns.insert("043.png"_load_qimage);
            varAns.insert("044.png"_load_qimage);
            varAns.insert("045.png"_load_qimage);
            varAns.insert("046.png"_load_qimage);
            varAns.insert("047.png"_load_qimage);
            varAns.insert("048.png"_load_qimage);
            varAns.insert("049.png"_load_qimage);
            varAns.insert("050.png"_load_qimage);
            varAns.insert("051.png"_load_qimage);
            varAns.insert("052.png"_load_qimage);
            varAns.insert("053.png"_load_qimage);
            varAns.insert("054.png"_load_qimage);
            varAns.insert("055.png"_load_qimage);
            varAns.insert("056.png"_load_qimage);
            varAns.insert("057.png"_load_qimage);
            varAns.insert("058.png"_load_qimage);
            varAns.insert("059.png"_load_qimage);
            varAns.insert("060.png"_load_qimage);
            varAns.insert("061.png"_load_qimage);
            varAns.insert("062.png"_load_qimage);
            varAns.insert("063.png"_load_qimage);
            varAns.insert("064.png"_load_qimage);
            varAns.insert("065.png"_load_qimage);
            varAns.insert("066.png"_load_qimage);
            varAns.insert("067.png"_load_qimage);
            varAns.insert("068.png"_load_qimage);
            varAns.insert("069.png"_load_qimage);
            varAns.insert("070.png"_load_qimage);
            varAns.insert("071.png"_load_qimage);
            varAns.insert("072.png"_load_qimage);
            varAns.insert("073.png"_load_qimage);
            varAns.insert("074.png"_load_qimage);
            varAns.insert("075.png"_load_qimage);
            varAns.insert("076.png"_load_qimage);
            varAns.insert("077.png"_load_qimage);
            return std::move(varAns);
        }();
        auto varPos = varResources.find(arg);
        if (varPos == varResources.end()) {
            varPos = varResources.begin();
        }
        source = &(varPos->second);
    }

    void QuickTexturePoint::ppp_ImageIndexChanged() {
        /***************************************************/
        //这仅仅是一个演示示例，仅仅支持受限的资源索引
        loadResource(this->mmm_ImageIndex, this->mmm_ImageSource);
        assert(mmm_ImageSource);
        /***************************************************/
        this->update();
    }

    static inline void registerThis() {
        {/*强制资源载入*/
            const QImage * arg = nullptr;
            loadResource(QStringLiteral("any"), arg);
        }
        qmlRegisterType<QuickTexturePoint>("myqml.qsgdrawpoint", 1, 0, "QuickTexturePoint");
    }
    Q_COREAPP_STARTUP_FUNCTION(registerThis)

}/*****/











