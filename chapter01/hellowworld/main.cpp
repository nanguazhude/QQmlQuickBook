#include <QtGui>
#include <QtQml>
#include <QtCore>
#include <QtQuick>
#include <QtWidgets>

int main(int argc , char ** argv){
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QDir::setCurrent(app.applicationDirPath());

    QQmlApplicationEngine engine;
    const static auto varMainQmlFileName = QDir(app.applicationDirPath()).
            absoluteFilePath(QStringLiteral("myqml/hellowword/main.qml"));
    engine.load(varMainQmlFileName);
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "can not load : "<< varMainQmlFileName;
        return -1;
    }

    return app.exec();
}

