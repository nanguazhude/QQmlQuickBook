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

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

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
    /*创建窗口*/
    std::unique_ptr varWindow = sstd::make_unique<RootWindow>();
    varWindow->show();
    /*启动主线程事件循环程序*/
    return varApp.exec();
}











