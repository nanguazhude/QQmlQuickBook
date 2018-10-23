TARGET = $$qtLibraryTarget(qsgdrawtexturepoint)
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

SOURCES += $$PWD/main.cpp \
    $$PWD/Application.cpp \
    $$PWD/GetLocalFullPath.cpp \
    $$PWD/QmlApplicationEngine.cpp


HEADERS += \
    $$PWD/Application.hpp \
    $$PWD/QmlApplicationEngine.hpp


DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/qsgdrawtexturepoint/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

SOURCES += $$PWD/QuickTexturePoint.cpp
HEADERS += $$PWD/QuickTexturePoint.hpp

include($$PWD/this/this.pri)
LIBS += -L$$RootDestDir -l$$qtLibraryTarget(sstd_core_library)



