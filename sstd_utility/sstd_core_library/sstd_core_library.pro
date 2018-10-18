
TARGET = $$qtLibraryTarget(sstd_core_library)
TEMPLATE = lib

DEFINES *= _1_SSTD_CORE_LIBRARY

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

QT -= core
QT -= gui

SOURCES += $$PWD/../time/sstd_time.cpp
HEADERS += $$PWD/global_sstd_core_library.hpp

SOURCES += $$PWD/sstd_core_memory.cpp









