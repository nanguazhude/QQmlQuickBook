
CONFIG(debug,debug|release){
    TARGET = sstd_core_libraryd
}else{
    TARGET = sstd_core_library
}

TEMPLATE = lib

DEFINES *= _1_SSTD_CORE_LIBRARY

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

QT -= core
QT -= gui

SOURCES += $$PWD/../time/sstd_time.cpp
HEADERS += $$PWD/global_sstd_core_library.hpp

SOURCES += $$PWD/sstd_core_memory.cpp

SOURCES += $$PWD/sstd_geometry_2d.cpp
HEADERS += $$PWD/sstd_geometry_2d.hpp

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}







