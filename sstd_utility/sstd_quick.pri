QT += qml
QT += quick
QT += widgets
QT += quickwidgets

INCLUDEPATH += $$PWD

include($$PWD/../OutDirPath.pri)
LIBS += -L$$RootDestDir -l$$qtLibraryTarget(sstd_quick_library)














