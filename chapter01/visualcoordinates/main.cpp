/*引入各个模块的头文件*/
#include <QtGui>
#include <QtQml>
#include <QtCore>
#include <QtQuick>
#include <QtWidgets>

#include <ctime>
#include <cstdlib>

#include "RootWindow.hpp"
#include "Application.hpp"

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

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
    /*强制加载Qt插件*/
    loadQtPlugins();
    /*初始化Qml/Quick引擎*/
    RootWindow varWindow;
    {
        /*main.qml完整目录*/
        const auto varMainQmlFileName =
            sstd::getLocalFileFullPath(QStringLiteral("myqml/visualcoordinates/main.qml"));
        /*加载main.qml*/
        varWindow.load(varMainQmlFileName);
        /*检查并报错*/
        if (varWindow.status() != sstd::LoadState::Ready) {
            qDebug() << "can not load : " << varMainQmlFileName;
            return -1;
        } else {
            varWindow.show();
            /***************************/
            auto varRootObject = varWindow.getRootObject();
            auto varTimer = sstdNew<QTimer>(varRootObject);
            varTimer->connect(varTimer, &QTimer::timeout, varRootObject, [varRootObject]() {
                qDebug() << QStringLiteral("global") << varRootObject->mapToGlobal({ 0,0 });
                qDebug() << QStringLiteral("scene") << varRootObject->mapToScene({ 0,0 });
            });
            varTimer->start(1000);
            /***************************/
        }
    }
    /*启动主线程事件循环程序*/
    return varApp.exec();
}

