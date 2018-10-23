
CONFIG(debug,debug|release){
    TARGET =   qsgdrawpoint_debug
}else{
    TARGET =   qsgdrawpoint
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

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

SOURCES += $$PWD/main.cpp \
    $$PWD/Application.cpp \
    $$PWD/GetLocalFullPath.cpp \
    $$PWD/QmlApplicationEngine.cpp

HEADERS += \
    $$PWD/Application.hpp \
    $$PWD/QmlApplicationEngine.hpp

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/qsgdrawpoint/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

SOURCES += $$PWD/QuickPoint.cpp
HEADERS += $$PWD/QuickPoint.hpp

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






