TARGET = $$qtLibraryTarget(qsgdrawuserender)
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

SOURCES += $$PWD/main.cpp \
    $$PWD/Application.cpp \
    $$PWD/GetLocalFullPath.cpp \
    $$PWD/main.cpp

HEADERS += \
    $$PWD/Application.hpp

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/qsgdrawuserender/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

SOURCES += $$PWD/QuickSimpleTriangle.cpp
HEADERS += $$PWD/QuickSimpleTriangle.hpp

include($$PWD/this/this.pri)

SOURCES += $$PWD/RootWindow.cpp
HEADERS += $$PWD/RootWindow.hpp

#CONFIG(debug,debug|release){
#    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#    QMAKE_LFLAGS_RELEASE =   $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
#    CONFIG+=declarative_debug
#    CONFIG+=qml_debug
#}




