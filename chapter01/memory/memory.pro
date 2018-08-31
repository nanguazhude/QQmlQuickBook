TARGET = $$qtLibraryTarget(memory)
TEMPLATE = app

QT -= gui
QT -= qml
QT -= core
QT -= quick
QT -= widgets

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

 

