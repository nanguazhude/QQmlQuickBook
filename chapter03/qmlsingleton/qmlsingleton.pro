
TARGET =   $$qtLibraryTarget(qmlsingleton)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../QQmlQuickBook.pri)
include($$PWD/../../sstd_utility/sstd_quick.pri)
DESTDIR = $$RootDestDir

QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc


DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/qmlsingleton/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

HEADERS += $$PWD/QmlSigleton.hpp
SOURCES += $$PWD/QmlSigleton.cpp

HEADERS += $$PWD/Application.hpp
SOURCES += $$PWD/Application.cpp

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/GetLocalFullPath.cpp

SOURCES += $$PWD/RootWindow.cpp
HEADERS += $$PWD/RootWindow.hpp

include($$PWD/this/this.pri)











