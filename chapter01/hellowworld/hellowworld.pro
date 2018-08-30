TARGET = $$qtLibraryTarget(hellowworld)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/buildinstall $$PWD "myqml"
export(QMAKE_POST_LINK)

QMLSOURCES += $$PWD/myqml/hellowword/main.qml
QMLSOURCES += $$PWD/myqml/hellowword/MainText.qml
QMLSOURCES += $$PWD/myqml/hellowword/MainRectangle.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

SOURCES += $$PWD/main.cpp

DISTFILES += $$QMLSOURCES


