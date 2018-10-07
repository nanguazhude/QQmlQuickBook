TARGET = $$qtLibraryTarget(computeshaderinvertimage)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

SOURCES += $$PWD/main.cpp

SOURCES += $$PWD/GetLocalFullPath.cpp

SOURCES += $$PWD/RenderThread.cpp
HEADERS += $$PWD/RenderThread.hpp

HEADERS += $$PWD/QuickImageProvider.hpp
SOURCES += $$PWD/QuickImageProvider.cpp

SOURCES += $$PWD/QmlApplicationEngine.cpp
HEADERS += $$PWD/QmlApplicationEngine.hpp

SOURCES += $$PWD/Application.cpp
HEADERS += $$PWD/Application.hpp

QMLSOURCES += $$PWD/myqml/computeshaderinvertimage/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"


