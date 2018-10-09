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
#include "RootWindow.hpp"

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

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

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
    /*强制加载Qt插件*/
    loadQtPlugins();

    debug_call_once([]() {std::cout << "main thread id : " << std::this_thread::get_id() << std::endl; });

    auto varWindow = sstdNew<RootWindow>();
    {
        /*main.qml完整目录*/
        const auto varMainQmlFileName = sstd::getLocalFileFullPath(
                    QStringLiteral(R"(myqml/qsgdrawuserender/main.qml)"));
        /*加载main.qml*/
        varWindow->load(varMainQmlFileName);
        /*检查并报错*/
        if (varWindow->status() != sstd::LoadState::Ready) {
            qDebug() << "can not load : " << varMainQmlFileName;
            return -1;
        }
        else {
            varWindow->show();
        }
    }
    /*启动主线程事件循环程序*/
    return varApp.exec();
}

/*
 * https://zhuanlan.zhihu.com/p/25769561
*/










