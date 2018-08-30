QT -= core
QT -= gui

CONFIG += console

#force remove debug
#CONFIG += release
#CONFIG -= debug

TARGET = $$qtLibraryTarget(buildinstall)
TEMPLATE = app

include($$PWD/../QQmlQuickBook.pri)

SOURCES += \
    main.cpp

DESTDIR = $$RootDestDir
DEFINES += FromDir=\\\"$$PWD\\\"
DEFINES += ToDir=\\\"$$DESTDIR\\\"

#force add application dir to library search dir
!win32 {
    QMAKE_LFLAGS += -Wl,-rpath .
}

#win32:{
#Local_Codec = GBK
#}else{
#Local_Codec = UTF-8
#}

#DEFINES += LOCAL_CODEC_NAME=$$join(Local_Codec,,"\\\"","\\\"")

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

