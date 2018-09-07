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

    sstd::setDefaultFormat();

    Window window;
    window.show();

    return varApp.exec();
}






