#include <QtGui/qcolor.h>
#include "RootWindow.hpp"
#include "MultimediaPlayer.hpp"
#include <QtCore/qtimer.h>

RootWindow::RootWindow(){
    setTitle( trUtf8( u8R"(物体与坐标关联)" ) );
    this->setMinimumSize({256,256});
    this->setClearColor(QColor(0,0,0,1));
    /*********************************************/
    //auto p = new sstd::Player;
    //p->setLocalFile(QString::fromUtf8(u8R"(G:\九品芝麻官\九品芝麻官.Hail.the.Judge.1994.BD1080P.X264.AAC.Cantonese&Mandarin.CHS.Mp4Ba.mp4)"));
    //p->open();
    //p->start();

    auto pt = new sstd::PlayerThread;
    auto p = new sstd::Player;
    //p->setLocalFile(QString::fromUtf8(u8R"(G:\阿拉蕾\阿拉蕾1981(辽艺)\001.rmvb)"));
    p->setLocalFile(QString::fromUtf8(u8R"(G:\九品芝麻官\九品芝麻官.Hail.the.Judge.1994.BD1080P.X264.AAC.Cantonese&Mandarin.CHS.Mp4Ba.mp4)"));
    p->open();
    pt->start(p);
    //QTimer::singleShot(100s, [pt]() { pt->stop(); });

}







