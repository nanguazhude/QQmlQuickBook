
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

SOURCES += $$PWD/sstd_RenderControl.cpp
HEADERS += $$PWD/sstd_RenderControl.hpp

SOURCES += $$PWD/sstd_Render.cpp
HEADERS += $$PWD/sstd_Render.hpp

SOURCES += $$PWD/sstd_RenderThread.cpp
HEADERS += $$PWD/sstd_RenderThread.hpp

SOURCES += $$PWD/MainWidget.cpp
HEADERS += $$PWD/MainWidget.hpp

include($$PWD/this/this.pri)
