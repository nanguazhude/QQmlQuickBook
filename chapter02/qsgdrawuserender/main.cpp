/*引入各个模块的头文件*/
#include <QtGui>
#include <QtQml>
#include <QtCore>
#include <QtQuick>
#include <QtWidgets>

#include <ctime>
#include <cstdlib>
#include <sstd_memory.hpp>

#include "Application.hpp"
#include <ConstructQSurface.hpp>
#include "QmlApplicationEngine.hpp"

#include <iostream>
#include "ThreadPrint.hpp"

namespace {

    inline void resetRandom() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    inline void loadQtPlugins() {
        QImage varImagePluginForceLoad{ QStringLiteral(":/qqmlquickglobal/image/load.png") };
        (void)varImagePluginForceLoad;
    }

}/*namespace*/

#include <cstdlib>

int main(int argc, char ** argv) {

#if defined(NDEBUG)
#else
    if constexpr (true) {
        //qputenv("QSG_RENDERER_DEBUG", "render");
        //qputenv("QSG_RENDERER_DEBUG", "dump");
        //qputenv("QML_IMPORT_TRACE","1");
        //qputenv("QML_DISABLE_DISK_CACHE","true");
    }
#endif

    /*高分屏支持*/
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    /*重置随机数种子*/
    resetRandom();
    /*设置opengl默认格式*/
    sstd::setDefaultFormat();
    /*初始化Qt环境*/
    Application varApp{ argc,argv };
    /*强制运行目录为程序所在目录*/
    QDir::setCurrent(varApp.applicationDirPath());
    /*强制加载Qt插件*/
    loadQtPlugins();
    /*加载Qml环境*/
    QmlApplicationEngine varQmlApplicationEngine;

    debug_call_once([]() {std::cout << "main thread id : " << std::this_thread::get_id() << std::endl; });

    /*检查Qml是否加载成功*/
    if (varQmlApplicationEngine.rootObjects().empty()) {
        return -1;
    }
    /*启动主线程事件循环程序*/
    return varApp.exec();
}

/*
 * https://zhuanlan.zhihu.com/p/25769561
*/










