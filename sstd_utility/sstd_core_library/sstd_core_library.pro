
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
SOURCES += $$PWD/sstd_core_function.cpp

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

include($$PWD/../sstd_boost/sstd_boost.pri)

win32 {
CONFIG(debug,debug|release){
    LIBS += -L$$PWD/windows_jemalloc -ljemallocd
}else{
    LIBS += -L$$PWD/windows_jemalloc -ljemalloc
}
    INCLUDEPATH += $$PWD/windows_jemalloc/include
}else{
    LIBS += -L$$RootDestDir -ljemalloc
    INCLUDEPATH += $$PWD/posix_jemalloc/include
}




