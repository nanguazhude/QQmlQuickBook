
#set cplusplus version
win32-msvc*{
    QMAKE_CXXFLAGS += /std:c++latest
    QMAKE_CXXFLAGS += /await
    CONFIG+=suppress_vcproj_warnings
}else{
    linux-g++*{
        LIBS += -lstdc++fs
        CONFIG(debug,debug|release){
        }else{
            QMAKE_CXXFLAGS += -Wno-cpp
        }
    }
    QMAKE_CXXFLAGS += -std=c++17
    #set c version
    QMAKE_CFLAGS += -std=c11
}

#set root install dir
CONFIG += console
include($$PWD/OutDirPath.pri)

INCLUDEPATH += $$PWD/sstd_utility
HEADERS +=     $$PWD/sstd_utility/sstd_memory.hpp
#####################################################$
HEADERS +=     $$PWD/sstd_utility/sstd_memory_lock.hpp
HEADERS +=     $$PWD/sstd_utility/sstd_array.hpp
HEADERS +=     $$PWD/sstd_utility/sstd_array_view.hpp
#####################################################$
HEADERS +=     $$PWD/sstd_utility/sstd_function.hpp
#####################################################$
HEADERS +=     $$PWD/sstd_utility/ConstructQSurface.hpp

include($$PWD/sstd_utility/sstd_quick_debug.pri)
include($$PWD/sstd_utility/time/time.pri)
include($$PWD/sstd_utility/sstd_boost/sstd_boost.pri)
win32{
    QMAKE_LIBDIR += $$RootDestDir
}else{
    LIBS += -lboost_context
    LIBS += -lboost_system
}

DEFINES *= BOOST_THREAD_PROVIDES_FUTURE_CTOR_ALLOCATORS
DEFINES *= BOOST_THREAD_VERSION=4

