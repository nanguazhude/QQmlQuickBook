CONFIG(debug,debug|release){
    RootDestDir = $$PWD/bin
    DEFINES *= _DEBUG
}else{#release
    RootDestDir = $$PWD/bin
    DEFINES *= NDEBUG #assert
    DEFINES *= QT_NO_DEBUG #Q_ASSERT,Q_ASSERT_X
}

















