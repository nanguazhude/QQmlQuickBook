TARGET = $$qtLibraryTarget(qsgdrawroundpoint)
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
    $$PWD/main.cpp \
    $$PWD/QmlApplicationEngine.cpp


HEADERS += \
    $$PWD/Application.hpp \
    $$PWD/QmlApplicationEngine.hpp


DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/qsgdrawroundpoint/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

SOURCES += $$PWD/QuickRoundPoint.cpp
SOURCES += $$PWD/quickroundpoint_from_qt_qsgvertexcolormaterial.cpp
HEADERS += $$PWD/QuickRoundPoint.hpp
HEADERS += $$PWD/quickroundpoint_from_qt_qsgvertexcolormaterial.h





