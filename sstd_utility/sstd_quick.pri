QT += qml
QT += quick
QT += widgets
QT += quickwidgets

INCLUDEPATH += $$PWD

LIBS += -L$$PWD/../bin -l$$qtLibraryTarget(sstd_quick_library)














