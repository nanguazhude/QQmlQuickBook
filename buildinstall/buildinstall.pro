QT -= core
QT -= gui
CONFIG += console

TARGET = $$qtLibraryTarget(buildinstall)
TEMPLATE = app

include($$PWD/../QQmlQuickBook.pri)

DEFINES += FromDir=\\\"$$PWD\\\"
DEFINES += ToDir=\\\"$$DESTDIR\\\"

SOURCES += \
    main.cpp

DESTDIR = $$RootDestDir

#xymoc_.input = HEADERS
#xymoc_.depends += $$HEADERS
#xymoc_.output  = xymoc_${QMAKE_FILE_BASE}.cpp
#xymoc_.commands = $$INSTALL_PATH_/xymoc ${QMAKE_FILE_NAME} ${QMAKE_FILE_OUT}
#xymoc_.variable_out = SOURCES
#QMAKE_EXTRA_COMPILERS += xymoc_

#QMAKE_EXTRA_COMPILERS
#export(QMAKE_EXTRA_COMPILERS)
#QMAKE_EXTRA_TARGETS
#export(QMAKE_EXTRA_TARGETS)
