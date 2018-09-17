TARGET = $$qtLibraryTarget(chat)
TEMPLATE = app

QT += gui
QT += qml
QT += core
QT += quick
QT += widgets
QT += concurrent

QT += gui_private
QT += qml_private
QT += core_private
QT += quick_private
QT += widgets_private

include($$PWD/../../QQmlQuickBook.pri)
DESTDIR = $$RootDestDir

#buildinstall
QMAKE_POST_LINK += $$DESTDIR/$$qtLibraryTarget(buildinstall) $$PWD "myqml"
export(QMAKE_POST_LINK)

DISTFILES +=  

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

RESOURCES +=

SOURCES += $$PWD/Application.cpp \
    $$PWD/QmlApplicationEngine.cpp \
    $$PWD/qml_chat_qtextdocumentlayout.cpp

SOURCES += $$PWD/GetLocalFullPath.cpp
SOURCES += $$PWD/main.cpp

HEADERS += $$PWD/Application.hpp \
    $$PWD/QmlApplicationEngine.hpp \
    $$PWD/qml_chat_qtextdocumentlayout_p.h

QMLSOURCES += $$PWD/myqml/chat/main.qml

lupdate_only{
    SOURCES += $$QMLSOURCES
}

DEFINES += CURRENT_DEBUG_PATH=\\\"$$PWD\\\"

RESOURCES += $$PWD/../../qqmlquickglobal.qrc

