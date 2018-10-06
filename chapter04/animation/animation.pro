#qml animation all in one

TARGET =   $$qtLibraryTarget(animation)
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

QMLSOURCES += $$PWD/myqml/animation/main.qml
QMLSOURCES += $$PWD/myqml/animation/main_private/MainListItem.qml
QMLSOURCES += $$PWD/myqml/animation/main_private/MainListModel.qml

QMLSOURCES += $$PWD/myqml/animation/DemoBasic.qml
QMLSOURCES += $$PWD/myqml/animation/DemoPropertyAnimation.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

HEADERS += $$PWD/QmlApplicationEngine.hpp
SOURCES += $$PWD/QmlApplicationEngine.cpp

HEADERS += $$PWD/Application.hpp
SOURCES += $$PWD/Application.cpp

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/GetLocalFullPath.cpp















