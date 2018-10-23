QT += qml
QT += quick
QT += widgets
QT += quickwidgets

INCLUDEPATH += $$PWD

include($$PWD/../OutDirPath.pri)
CONFIG(debug,debug|release){
    LIBS += -L$$RootDestDir -lsstd_quick_libraryd
}else{
    LIBS += -L$$RootDestDir -lsstd_quick_library
}
















