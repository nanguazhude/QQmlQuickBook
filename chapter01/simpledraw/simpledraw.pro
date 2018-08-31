TARGET = $$qtLibraryTarget(simpledraw)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp \
    ApplicationEngine.cpp \
    SimpleDraw.cpp

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

QMLSOURCES += $$PWD/myqml/simpledraw/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

DISTFILES += $$QMLSOURCES

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

HEADERS += \
    ApplicationEngine.hpp \
    SimpleDraw.hpp

