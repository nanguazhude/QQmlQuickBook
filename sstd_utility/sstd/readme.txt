1. 命令行进入下载的boost包的根目录
2. bootstrap.bat，得到bjam.exe
3. bjam tools/bcp，在dist\bin目录下会得到bcp.exe
4. boost/config/user.hpp

/*************************/

#ifndef _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#endif

#ifndef _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#endif

#ifndef BOOST_HAS_TR1
#define BOOST_HAS_TR1
#endif

#ifndef BOOST_SP_USE_STD_ATOMIC
#define BOOST_SP_USE_STD_ATOMIC
#endif

#ifndef BOOST_AC_USE_STD_ATOMIC
#define BOOST_AC_USE_STD_ATOMIC
#endif

#ifndef BOOST_ALL_NO_LIB
#define BOOST_ALL_NO_LIB
#endif

/*************************/

5.replace

<boost/ to <sstd/boost/
"boost/ to "sstd/boost/

















