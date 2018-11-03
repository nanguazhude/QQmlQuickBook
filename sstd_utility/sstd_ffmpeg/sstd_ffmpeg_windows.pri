include($$PWD/../../OutDirPath.pri)

LIBS += -L$$RootDestDir -lavcodec
LIBS += -L$$RootDestDir -lavdevice
LIBS += -L$$RootDestDir -lavfilter
LIBS += -L$$RootDestDir -lavformat
LIBS += -L$$RootDestDir -lavutil
LIBS += -L$$RootDestDir -lpostproc
LIBS += -L$$RootDestDir -lswresample
LIBS += -L$$RootDestDir -lswscale

INCLUDEPATH += $$PWD/../windows_ffmpeg

