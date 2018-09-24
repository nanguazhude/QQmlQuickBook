#include <sstd_memory.hpp>
#include "quickroundpoint_from_qt_qsgvertexcolormaterial.h"
#include <qopenglshaderprogram.h>

namespace  sstd {
    namespace quickroundpoint {
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

        private:
            void initialize() override;
            int m_matrix_id;
            int m_opacity_id;
        };

        QSGMaterialType QSGVertexColorMaterialShader::type;

        QSGVertexColorMaterialShader::QSGVertexColorMaterialShader() { }
               
        
        void QSGVertexColorMaterialShader::updateState(const RenderState &state, QSGMaterial * /*newEffect*/, QSGMaterial *) {
            if (state.isOpacityDirty())
                program()->setUniformValue(m_opacity_id, state.opacity());
            if (state.isMatrixDirty())
                program()->setUniformValue(m_matrix_id, state.combinedMatrix());
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

        int QSGVertexColorMaterial::compare(const QSGMaterial * /* other */) const {
            return 0;
        }

        QSGMaterialType *QSGVertexColorMaterial::type() const {
            return &QSGVertexColorMaterialShader::type;
        }


        QSGMaterialShader *QSGVertexColorMaterial::createShader() const {
            return sstdNew<QSGVertexColorMaterialShader>();
        }
    }/*namespace quickroundpoint*/
}/*namespace sstd*/

