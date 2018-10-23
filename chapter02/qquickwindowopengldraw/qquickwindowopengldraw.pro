
CONFIG(debug,debug|release){
    TARGET =   qquickwindowopengldraw_debug
}else{
    TARGET =   qquickwindowopengldraw
}


TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp \
    ApplicationEngine.cpp \
    OpenglDrawWindow.cpp \
    Application.cpp

HEADERS += \
    ApplicationEngine.hpp \
    OpenglDrawWindow.hpp \
    Application.hpp

QMLSOURCES += $$PWD/myqml/qquickwindowopengldraw/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

DISTFILES += $$QMLSOURCES \
    $$PWD/myqml/qquickwindowopengldraw/fragment.frag \
    $$PWD/myqml/qquickwindowopengldraw/vertex.vert

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

include($$PWD/this/this.pri)

CONFIG(debug,debug|release){
    LIBS += -L$$RootDestDir -lsstd_core_libraryd
    QMAKE_POST_LINK += $$DESTDIR/buildinstall_debug $$PWD "myqml"
    export(QMAKE_POST_LINK)
}else{
    LIBS += -L$$RootDestDir -lsstd_core_library
    QMAKE_POST_LINK += $$DESTDIR/buildinstall $$PWD "myqml"
    export(QMAKE_POST_LINK)
}

