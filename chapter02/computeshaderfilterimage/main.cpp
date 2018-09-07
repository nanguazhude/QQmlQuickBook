#include <ConstructQSurface.hpp>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Window.hpp"

class Application : public QApplication {
public:
    Application(int & a ,char ** b) :QApplication(a,b) {}
    virtual ~Application() = default;
};

int main(int argc,char ** argv){
    Application varApp{argc,argv};

    {/*强制运行目录为程序所在目录*/
        QDir::setCurrent(qApp->applicationDirPath());
    }
    sstd::setDefaultFormat();

    auto window = sstdNew<Window>();
    window->show();

    return varApp.exec();
}






