TARGET = $$qtLibraryTarget(computeshaderfilterimage)
TEMPLATE = app
QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

SOURCES += $$PWD/main.cpp \
    $$PWD/ImageView.cpp
SOURCES += $$PWD/Window.cpp

SOURCES += $$PWD/Application.cpp
HEADERS += $$PWD/Application.hpp

SOURCES += GetLocalFullPath.cpp

HEADERS += $$PWD/Window.hpp \
    $$PWD/ImageView.hpp

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

include($$PWD/this/this.pri)







