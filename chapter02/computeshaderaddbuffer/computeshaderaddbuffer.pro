CONFIG(debug,debug|release){
    TARGET =   computeshaderaddbuffer_debug
}else{
    TARGET =   computeshaderaddbuffer
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

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

SOURCES += $$PWD/main.cpp

SOURCES += $$PWD/Window.cpp
HEADERS += $$PWD/Window.hpp

SOURCES += $$PWD/Application.cpp
HEADERS += $$PWD/Application.hpp

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





