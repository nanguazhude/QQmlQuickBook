#pragma once

#include <QtCore/qstring.h>
#include <QtGui/qopenglcontext.h>
#include <QtGui/qsurfaceformat.h>
#if defined(__glew_h__)||defined(__GLEW_H__)
    /***********************/
#else
#include <QtGui/qopenglfunctions.h>
#include <QtGui/qopenglfunctions_4_5_core.h>
#endif

#include <fstream>
#include "sstd_memory.hpp"

namespace sstd {

    inline QSurfaceFormat getDefaultOpenGLFormat() {
        auto varFormat = QSurfaceFormat::defaultFormat();
        varFormat.setVersion(4, 6);
        varFormat.setProfile(QSurfaceFormat::CoreProfile);
        varFormat.setSamples(4);
        varFormat.setAlphaBufferSize(8);
        varFormat.setBlueBufferSize(8);
        varFormat.setRedBufferSize(8);
        varFormat.setGreenBufferSize(8);
        varFormat.setDepthBufferSize(24);
        varFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
        varFormat.setRenderableType(QSurfaceFormat::OpenGL);
        varFormat.setSwapInterval(0)/*关闭垂直同步*/;
#if defined(ENABLE_GL_DEBUG)
        varFormat.setOption(QSurfaceFormat::DebugContext, true);
#else
        varFormat.setOption(QSurfaceFormat::DebugContext, false);
#endif
        return varFormat;
    }

    inline void setDefaultFormat() {
        QSurfaceFormat::setDefaultFormat(sstd::getDefaultOpenGLFormat());
    }

#if defined(__glew_h__)||defined(__GLEW_H__)
    /***********************/
#else
    using OpenGLFunctions = QOpenGLFunctions_4_5_Core;
#endif

}/*****/

#include "sstd_load_utf8_file.hpp"

