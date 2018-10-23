
CONFIG(debug,debug|release){
    TARGET =   computeshaderfilterimage_debug
}else{
    TARGET =   computeshaderfilterimage
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
SOURCES += $$PWD/ImageView.cpp
SOURCES += $$PWD/Window.cpp

SOURCES += $$PWD/Application.cpp
HEADERS += $$PWD/Application.hpp

SOURCES += GetLocalFullPath.cpp

HEADERS += $$PWD/Window.hpp
HEADERS += $$PWD/ImageView.hpp

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

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}




