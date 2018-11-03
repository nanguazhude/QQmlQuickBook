#qml quickchart all in one

CONFIG(debug,debug|release){
    TARGET =   multimediaplayer_debug
}else{
    TARGET =   multimediaplayer
}


TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

QT += charts

include($$PWD/../../QQmlQuickBook.pri)
include($$PWD/../../sstd_utility/sstd_quick.pri)
DESTDIR = $$RootDestDir


!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc


DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/multimediaplayer/main.qml


lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

HEADERS += $$PWD/Application.hpp
SOURCES += $$PWD/Application.cpp

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/GetLocalFullPath.cpp

SOURCES += $$PWD/RootWindow.cpp
HEADERS += $$PWD/RootWindow.hpp

SOURCES += $$PWD/MultimediaPlayer.cpp
HEADERS += $$PWD/MultimediaPlayer.hpp

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

include($$PWD/../../sstd_utility/sstd_ffmpeg/sstd_ffmpeg.pri)







