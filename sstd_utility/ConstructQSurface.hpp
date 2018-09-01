#pragma once

#include <QtCore/qstring.h>
#include <QtGui/qopenglcontext.h>
#include <QtGui/qsurfaceformat.h>
#include <QtGui/qopenglfunctions.h>
#include <QtGui/qopenglfunctions_4_5_core.h>
#include <fstream>
#include "sstd_memory.hpp"

namespace sstd {

    inline void setDefaultFormat() {
        QSurfaceFormat varFormat = QSurfaceFormat::defaultFormat();
        if (varFormat.majorVersion() < 4) {
            varFormat.setVersion(4, 5);
        }
        if ((varFormat.majorVersion() == 4) && (varFormat.minorVersion() < 5)) {
            varFormat.setVersion(4, 5);
        }
        varFormat.setProfile(QSurfaceFormat::CoreProfile);
        varFormat.setSamples(4);
        varFormat.setAlphaBufferSize(8);
        varFormat.setBlueBufferSize(8);
        varFormat.setRedBufferSize(8);
        varFormat.setGreenBufferSize(8);
        varFormat.setDepthBufferSize(24);
        QSurfaceFormat::setDefaultFormat(varFormat);
    }

    using OpenGLFunctions = QOpenGLFunctions_4_5_Core;

    

}/*****/

#include "sstd_load_utf8_file.hpp"

