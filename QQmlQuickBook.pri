
#set cplusplus version
win32-msvc*{
    QMAKE_CXXFLAGS += /std:c++latest
    QMAKE_CXXFLAGS += /await
}else{
    CONFIG+=c++17
}

#set root install dir
CONFIG(debug,debug|release){
    RootDestDir = $$PWD/bin
    CONFIG += console
}else{#release
    RootDestDir = $$PWD/bin
    CONFIG += console
    DEFINES += NDEBUG #assert
    DEFINES += QT_NO_DEBUG #Q_ASSERT,Q_ASSERT_X
}


