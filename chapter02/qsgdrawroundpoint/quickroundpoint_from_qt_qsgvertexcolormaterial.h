#pragma once

#include <QtQuick/qsgmaterial.h>

namespace sstd {
    namespace quickroundpoint {
        class QSGVertexColorMaterial : public QSGMaterial {
        public:
            QSGVertexColorMaterial();
            int compare(const QSGMaterial *other) const override;
        protected:
            QSGMaterialType *type() const override;
            QSGMaterialShader *createShader() const override;
        };
    }
}/****/


