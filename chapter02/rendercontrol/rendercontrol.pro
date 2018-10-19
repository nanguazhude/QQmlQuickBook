
TARGET =   $$qtLibraryTarget(rendercontrol)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc


DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/rendercontrol/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

HEADERS += $$PWD/Application.hpp
SOURCES += $$PWD/Application.cpp

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/GetLocalFullPath.cpp

HEADERS += $$PWD/sstd_Window.hpp
SOURCES += $$PWD/sstd_Window.cpp

include($$PWD/this/this.pri)
include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

HEADERS += $$PWD/sstd_WindowState.hpp
HEADERS += $$PWD/sstd_RenderPack.hpp
SOURCES += $$PWD/sstd_RenderPack.cpp

include($$PWD/../../sstd_utility/sstd_quick.pri)

LIBS += -L$$RootDestDir -l$$qtLibraryTarget(sstd_core_library)

SOURCES += $$PWD/cuberenderer.cpp
SOURCES += $$PWD/window_multithreaded.cpp

HEADERS += $$PWD/cuberenderer.h
HEADERS += $$PWD/window_multithreaded.h


