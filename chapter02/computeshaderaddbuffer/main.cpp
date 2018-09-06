#include <ConstructQSurface.hpp>
#include <QtCore>
#include <QtGui>
#include <optional>
#include <fstream>
#include <filesystem>
#include <QtCore/qtimer.h>
#include "Window.hpp"
#include <future>
#include <thread>
#include <iostream>

class Application : public QGuiApplication {
public:
    Application(int & a ,char ** b) :QGuiApplication(a,b) {}
    virtual ~Application() = default;
};

int main(int argc,char ** argv){
    Application varApp{argc,argv};

    sstd::setDefaultFormat();

    Window window;
    window.show();

    return varApp.exec();
}






