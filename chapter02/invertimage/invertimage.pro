TARGET = $$qtLibraryTarget(invertimage)
TEMPLATE = app

QT += gui
QT += qml
QT += core
#QT += quick #in this demo we do not need quick
#QT += widgets #in this demo we do not need widgets
#QT += concurrent # in this demo we do not need concurrent

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/Window.cpp
HEADERS += $$PWD/Window.hpp




