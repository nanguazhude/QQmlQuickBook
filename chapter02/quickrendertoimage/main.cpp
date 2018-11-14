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
#include "sstd_Window.hpp"

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

int main(int argc, char ** argv) try {
    /*高分屏支持*/
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    /*重置随机数种子*/
    resetRandom();
    /*设置opengl默认格式*/
    sstd::setDefaultFormat();
    /*初始化Qt环境*/
    Application varApp{ argc,argv };

    /*分析命令行...*/
    QCommandLineParser varParser;
    varParser.setApplicationDescription(QStringLiteral("RenderQMLToImage"));
    {
        varParser.addOption({ {QStringLiteral("h"),QStringLiteral("help")},
            QStringLiteral(R"(if null input ,gui will show 
else use :
-i/--input xxxx.qml
-o/--ouput xxxx.png .)") });
        varParser.addOption({ {QStringLiteral("i"),QStringLiteral("input")},
            QStringLiteral("input qml file name ."),
            QStringLiteral("qmlFileName") });
        varParser.addOption({ {QStringLiteral("o"),QStringLiteral("output")},
            QStringLiteral("output png file name ."),
            QStringLiteral("pngFileName") });
    }

    if (!varParser.parse(varApp.arguments())) {
        qDebug() << varParser.errorText();
        return -1;
    }

    bool varInputOutPutIsSet = false;
    if ( varParser.isSet(QStringLiteral("i")) && varParser.isSet(QStringLiteral("o")) ) {
        varInputOutPutIsSet = true;
    }

    /*强制加载Qt插件*/
    loadQtPlugins();
    /*加载Qml环境*/
    auto varWindow = sstd::make_unique<sstd::Window>();
    bool isFinished = false;
    if (varInputOutPutIsSet) {
        QDir varDir;
        const auto varOutPutImageFile = varDir.absoluteFilePath(varParser.value(QStringLiteral("o")) ) ;
        varWindow->connect(varWindow.get(), &sstd::Window::renderFinished, 
            [varOutPutImageFile,&isFinished](const QImage & varSaveImage) {
            varSaveImage.save(varOutPutImageFile);
            {
                auto varQuit = sstdNew<QTimer>();
                varQuit->connect(varQuit, &QTimer::timeout, []() {
                    qApp->quit(); 
                    std::exit(0);
                });
                varQuit->start(1);
            }
            qApp->quit();
            isFinished = true;
            std::exit(0);
        });
        varWindow->startRender(varDir.absoluteFilePath(varParser.value(QStringLiteral("i"))));
    } else {
        varWindow->show();
    }

    if (isFinished) {
        return 0;
    }

    /*启动主线程事件循环程序*/
    return varApp.exec();
} catch (...) {
    qDebug() << QStringLiteral("main exception output!");
    return 0;
}











