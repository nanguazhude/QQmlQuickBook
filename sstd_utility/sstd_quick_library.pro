#sstd_quick_library
CONFIG(debug,debug|release){
    TARGET = sstd_quick_libraryd
}else{
    TARGET = sstd_quick_library
}

TEMPLATE = lib

DEFINES *= _1_SSTD_QUICK_LIBRARY

include($$PWD/../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

QT += qml
QT += quick
QT += widgets
QT += quickwidgets

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/quick/QuickView.hpp \
    $$PWD/quick/QuickViewWidget.hpp \
    $$PWD/quick/QuickViewWindow.hpp \
    $$PWD/quick/sstd_QThread.hpp

SOURCES += \
    $$PWD/quick/sstd_QuickView.cpp \
    $$PWD/quick/sstd_QuickViewWidget.cpp \
    $$PWD/quick/sstd_QuickViewWindow.cpp \
    $$PWD/quick/sstd_QThread.cpp

DEFINES *= NO_SSTD_DEBUG_CPP
DEFINES *= NO_SSTD_TIME_CPP

CONFIG(debug,debug|release){
    LIBS += -L$$RootDestDir -lsstd_core_libraryd
}else{
    LIBS += -L$$RootDestDir -lsstd_core_library
}

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

SOURCES += $$PWD/quick/sstd_geometry_2d.cpp
HEADERS += $$PWD/quick/sstd_geometry_2d.hpp













