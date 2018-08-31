#include "ApplicationEngine.hpp"
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>
#include <QtCore/qcoreapplication.h>
#include "SimpleDraw.hpp"

/*this function should be just run once*/
ApplicationEngine::ApplicationEngine(QObject *parent) : Super(parent) {
    try {
    {/*检查是否第一次加载*/
        static bool varHasRun = false;
        if (varHasRun) { 
            throw "this function should just runonce!!!" ; 
        }
        varHasRun = true;
    }
    {/*强制运行目录为程序所在目录*/
        QDir::setCurrent(qApp->applicationDirPath());
    }
    {/*注册类型*/
        qmlRegisterType<SimpleDraw>("myqml.simpledraw", 1, 0, "SimpleDraw");
    }
    /*main.qml完整目录*/
    const static auto varMainQmlFileName = QDir(qApp->applicationDirPath())
        .absoluteFilePath(QStringLiteral("myqml/simpledraw/main.qml"));
    /*加载main.qml*/
    this->load(varMainQmlFileName);
    /*检查并报错*/
    if (this->rootObjects().isEmpty()) {
        qDebug() << "can not load : " << varMainQmlFileName;
        throw "load error!!!";
    }
   }
    catch (const char * err) {
        qDebug() << err;
    }
    catch (...) {
        qDebug() << "unknow error";
    }
   

}


