
#set cplusplus version
win32-msvc*{
    QMAKE_CXXFLAGS += /std:c++latest
    QMAKE_CXXFLAGS += /await
    CONFIG+=suppress_vcproj_warnings
}else{
    CONFIG+=c++17
}

#set c version
QMAKE_CFLAGS += -std=c11

#set root install dir
CONFIG += console
CONFIG(debug,debug|release){
    RootDestDir = $$PWD/bin
    DEFINES *= _DEBUG
}else{#release
    RootDestDir = $$PWD/bin
    DEFINES += NDEBUG #assert
    DEFINES += QT_NO_DEBUG #Q_ASSERT,Q_ASSERT_X
}

INCLUDEPATH += $$PWD/sstd_utility
HEADERS += $$PWD/sstd_utility/sstd_memory.hpp
HEADERS += $$PWD/sstd_utility/ConstructQSurface.hpp

include($$PWD/sstd_utility/sstd_quick_debug.pri)
include($$PWD/sstd_utility/time/time.pri)


