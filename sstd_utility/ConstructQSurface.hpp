#pragma once

#include <QtGui/qopenglcontext.h>
#include <QtGui/qsurfaceformat.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/qopenglfunctions_4_5_compatibility.h>

namespace sstd {

    inline void setDefaultFormat() {
        QSurfaceFormat varFormat = QSurfaceFormat::defaultFormat();
        if (varFormat.majorVersion() < 4) {
            varFormat.setVersion(4, 5);
        }
        if ((varFormat.majorVersion() == 4) && (varFormat.minorVersion() < 5)) {
            varFormat.setVersion(4, 5);
        }
        varFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
        QSurfaceFormat::setDefaultFormat(varFormat);
    }

    using OpenGLFunctions = QOpenGLFunctions_4_5_Compatibility;

}/*****/

