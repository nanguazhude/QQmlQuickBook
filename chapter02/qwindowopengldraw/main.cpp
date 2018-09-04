/*引入各个模块的头文件*/
#include <QtGui>
#include <QtQml>
#include <QtCore>
#include <QtQuick>
#include <QtWidgets>

#include <ctime>
#include <cstdlib>
#include <sstd_memory.hpp>

#include <ConstructQSurface.hpp>
#include "OpenGLWindow.hpp"
#include "Application.hpp"

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
    Application varApp(argc, argv);
    /*强制运行目录为程序所在目录*/
    QDir::setCurrent(varApp.applicationDirPath());
    /*设置opengl默认格式*/
    sstd::setDefaultFormat();
    /*强制加载Qt插件*/
    loadQtPlugins();
    {/*显示窗口*/
        for (int i = 0; i < 10  ; ++i) {
            auto varWindow = sstdNew<OpenGLWindow>();
            const QRect varDesktopSize = varWindow->screen()->availableGeometry();
            const auto varWidth = std::max(64, varDesktopSize.width());
            const auto varHeight = std::max(64, varDesktopSize.height());
            varWindow->setPosition(QPoint(varDesktopSize.x() + (std::rand() % varWidth) / 2 - 10,
                varDesktopSize.y() + (std::rand() % varHeight) / 2 + 20));
            varWindow->show();
        }
    }
    /*启动主线程事件循环程序*/
    return varApp.exec();
}


