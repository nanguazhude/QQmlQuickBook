DEFINES *= GLEW_NO_GLU
DEFINES *= GLEW_STATIC

win32:{
LIBS+= -lopengl32 -lgdi32 -luser32 -lkernel32
}

linux:{
###libglu1-mesa-dev
###libgl1-mesa-dev
LIBS+=-lXmu -lXi -lGL -lXext -lX11
}

INCLUDEPATH += $$PWD/glew_source/glew

SOURCES += $$PWD/glew_source/glew.c
SOURCES += $$PWD/glew_source/qt_initializeglew.cpp
SOURCES += $$PWD/glew_source/windows_vsync.cpp

HEADERS += $$PWD/glew_source/glew/GL/glew.h
HEADERS += $$PWD/glew_source/glew/GL/eglew.h
HEADERS += $$PWD/glew_source/glew/GL/glxew.h
HEADERS += $$PWD/glew_source/glew/GL/wglew.h

INCLUDEPATH += $$PWD
HEADERS += $$PWD/sstd_glew.hpp



include($$PWD/glew_enable_gldebug.pri)







