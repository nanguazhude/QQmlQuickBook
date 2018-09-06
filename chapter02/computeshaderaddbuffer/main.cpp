#include <ConstructQSurface.hpp>
#include <QtCore>
#include <QtGui>
#include "Window.hpp"

class Application : public QGuiApplication {
public:
    Application(int & a ,char ** b) :QGuiApplication(a,b) {}
    virtual ~Application() = default;
};

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

    Application varApp{argc,argv};

    {/*强制运行目录为程序所在目录*/
        QDir::setCurrent(qApp->applicationDirPath());
    }

    /*设置opengl默认格式*/
    sstd::setDefaultFormat();

    /*强制加载Qt插件*/
    loadQtPlugins();

    Window window;
    window.show();

    return varApp.exec();
}






