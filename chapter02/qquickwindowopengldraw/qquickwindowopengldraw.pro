TARGET = $$qtLibraryTarget(qquickwindowopengldraw)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp \
    ApplicationEngine.cpp \
    OpenglDrawWindow.cpp \
    Application.cpp

HEADERS += \
    ApplicationEngine.hpp \
    OpenglDrawWindow.hpp \
    Application.hpp

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

QMLSOURCES += $$PWD/myqml/qquickwindowopengldraw/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

DISTFILES += $$QMLSOURCES \
    $$PWD/myqml/qquickwindowopengldraw/fragment.frag \
    $$PWD/myqml/qquickwindowopengldraw/vertex.vert

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

include($$PWD/../../sstd_utility/glew.pri)
include($$PWD/../../sstd_utility/glm.pri)

