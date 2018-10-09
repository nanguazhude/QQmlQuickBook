#include <ConstructQSurface.hpp>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Window.hpp"
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

int main(int argc,char ** argv){
    /*高分屏支持*/
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    /*重置随机数种子*/
    resetRandom();

    /*设置opengl默认格式*/
    sstd::setDefaultFormat();

    Application varApp{ argc,argv };

    /*强制加载Qt插件*/
    loadQtPlugins();

    auto window = sstdNew<Window>();
    window->show();

    return varApp.exec();

}






