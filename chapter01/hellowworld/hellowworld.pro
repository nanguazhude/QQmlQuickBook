TARGET = $$qtLibraryTarget(hellowworld)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

SOURCES += $$PWD/main.cpp

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

QMLSOURCES += $$PWD/myqml/hellowword/main.qml
QMLSOURCES += $$PWD/myqml/hellowword/MainText.qml
QMLSOURCES += $$PWD/myqml/hellowword/MainRectangle.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

DISTFILES += $$QMLSOURCES

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

