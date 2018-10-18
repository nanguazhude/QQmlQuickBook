#qml animationandstate all in one

TARGET =   $$qtLibraryTarget(animationandstate)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

include($$PWD/../../QQmlQuickBook.pri)
include($$PWD/../../sstd_utility/sstd_quick.pri)
DESTDIR = $$RootDestDir

QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES += $$PWD/../../qqmlquickglobal.qrc


DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

QMLSOURCES += $$PWD/myqml/animationandstate/main.qml
QMLSOURCES += $$PWD/myqml/animationandstate/main_private/MainListItem.qml
QMLSOURCES += $$PWD/myqml/animationandstate/main_private/MainListModel.qml
QMLSOURCES += $$PWD/myqml/animationandstate/main_private/MainListView.qml

QMLSOURCES += $$PWD/myqml/animationandstate/DemoBasic.qml
QMLSOURCES += $$PWD/myqml/animationandstate/DemoPropertyAnimation.qml
QMLSOURCES += $$PWD/myqml/animationandstate/DemoParallelStateAnimation.qml
QMLSOURCES += $$PWD/myqml/animationandstate/DemoParallelStateSelectRun.qml
QMLSOURCES += $$PWD/myqml/animationandstate/DemoCircleState.qml
QMLSOURCES += $$PWD/myqml/animationandstate/DemoCircleStatePause.qml
QMLSOURCES += $$PWD/myqml/animationandstate/DemoCircleStateQuit.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}
DISTFILES += $$QMLSOURCES

HEADERS += $$PWD/QmlLocalFileLoader.hpp
SOURCES += $$PWD/QmlLocalFileLoader.cpp

HEADERS += $$PWD/Application.hpp
SOURCES += $$PWD/Application.cpp

SOURCES += $$PWD/main.cpp
SOURCES += $$PWD/GetLocalFullPath.cpp

SOURCES += $$PWD/RootWindow.cpp
HEADERS += $$PWD/RootWindow.hpp

include($$PWD/this/this.pri)
LIBS += -L$$RootDestDir -l$$qtLibraryTarget(sstd_core_library)










