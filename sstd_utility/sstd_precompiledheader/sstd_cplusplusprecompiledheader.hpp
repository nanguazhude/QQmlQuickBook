#pragma once

#if defined(__cplusplus)
/*cplusplus file only*/

/*cplusplus预编译头文件*/
/**cplusplusprecompiledheader.hpp**/

#if defined(_DEBUG)/*_DEBUG*/
/*Debug模式下的预编译头文件*/
/*Debug模式是经常被修改的模式，为了硬盘寿命，少加预编译头文件*/
#include <cassert>

#else/*_DEBUG*/

#include "../sstd_memory.hpp"

#if defined(QT_CORE_LIB)
#include <QtCore/QtCore>
#endif

#if defined(QT_GUI_LIB)
#include <QtGui/QtGuiDepends>
#include <QtGui/qimage.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qbrush.h>
#include <QtGui/qpen.h>
#include <QtGui/qcolor.h>
#endif

#if defined(QT_QML_LIB)
#include <QtQml/QtQml>
#endif

#endif/*_DEBUG*/

#endif

















