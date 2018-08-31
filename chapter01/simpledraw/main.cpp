/*引入各个模块的头文件*/
#include <QtGui>
#include <QtQml>
#include <QtCore>
#include <QtQuick>
#include <QtWidgets>

#include <ctime>
#include <cstdlib>
#include <sstd_memory.hpp>

#include "SimpleDraw.hpp"
#include "ApplicationEngine.hpp"

namespace {

    inline void resetRandom() {
        std::srand(static_cast<int>(std::time(nullptr)));
    }

    inline void loadQtPlugins() {
        QImage varImagePluginForceLoad{ QStringLiteral(":/qqmlquickglobal/image/load.png") };
        (void)varImagePluginForceLoad;
    }

}/*namespace*/

int main(int argc, char ** argv) {
    /*高分屏支持*/
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    /*重置随机数种子*/
    resetRandom();
    /*初始化Qt环境*/
    QApplication varApp(argc, argv);
    /*强制加载Qt插件*/
    loadQtPlugins();
    /*初始化Qml/Quick引擎*/
    ApplicationEngine varEngine;
    /*启动主线程事件循环程序*/
    return varApp.exec();
}


