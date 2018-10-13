
TARGET = $$qtLibraryTarget(computeshaderthreaddraw)
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

DISTFILES += $$QMLSOURCES

HEADERS += $$PWD/Application.hpp
SOURCES += $$PWD/Application.cpp

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/GetLocalFullPath.cpp

SOURCES += $$PWD/RootWindow.cpp
HEADERS += $$PWD/RootWindow.hpp

SOURCES += $$PWD/RenderThread.cpp
HEADERS += $$PWD/RenderThread.hpp

HEADERS += $$PWD/RenderSize.hpp

include($$PWD/this/this.pri)
include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)







