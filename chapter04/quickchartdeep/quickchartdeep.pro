TARGET = $$qtLibraryTarget(quickchartdeep)
TEMPLATE = app

QT += gui
QT += core
QT += qml
QT += quick
QT += widgets

QT += charts

QT += gui_private
QT += core_private
QT += qml_private
QT += quick_private
QT += widgets_private

QT += charts_private

include($$PWD/../../QQmlQuickBook.pri)
include($$PWD/../../sstd_utility/sstd_quick.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp

SOURCES += $$PWD/RootWindow.cpp
HEADERS += $$PWD/RootWindow.hpp

SOURCES += $$PWD/Application.cpp
HEADERS += $$PWD/Application.hpp

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

QMLSOURCES += $$PWD/myqml/quickchartdeep/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

DISTFILES += $$QMLSOURCES

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"
SOURCES += $$PWD/GetLocalFullPath.cpp

include($$PWD/this/this.pri)
LIBS += -L$$RootDestDir -l$$qtLibraryTarget(sstd_core_library)

HEADERS += $$PWD/SSTDChartView.hpp
SOURCES += $$PWD/SSTDChartView.cpp

