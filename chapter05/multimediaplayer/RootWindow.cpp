#include <QtGui/qcolor.h>
#include "RootWindow.hpp"
#include "MultimediaPlayer.hpp"
#include <QtCore/qtimer.h>

RootWindow::RootWindow() {
    setTitle(trUtf8(u8R"(简单视频播放器)"));
    this->setMinimumSize({ 256,256 });
    this->setClearColor(QColor(0, 0, 0, 1));
}







