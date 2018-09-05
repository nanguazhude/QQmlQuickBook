#include <ConstructQSurface.hpp>
#include <QtCore>
#include <QtGui>
#include <optional>
#include <fstream>
#include <filesystem>
#include <QtCore/qtimer.h>
#include "Window.hpp"

/*仅仅用于*/
class Application : public QGuiApplication {
public:
    Application(int & a ,char ** b) :QGuiApplication(a,b) {}
    virtual ~Application() = default;
};

int main(int argc,char **argv) {
    const std::filesystem::path varAppPath = [argv=argv]() {
        std::filesystem::path varAns(argv[0]);
        return std::filesystem::absolute(std::move(varAns));
    }();
    {/*将应用程序路径设置为当前路径*/
        auto varAppDir = varAppPath;
        std::filesystem::current_path(varAppDir.remove_filename());
    }
    std::optional<std::filesystem::path> varNewImagePath;
    if (argc<2) {/*是否采用默认图片*/
        varNewImagePath = varAppPath;
        (*varNewImagePath).replace_filename("myqml/invertimage/test.png"sv);
    }
    /*初始化程序*/
    Application varApp(argc,argv);

    /*设置默认QSurfaceFormat*/
    sstd::setDefaultFormat();
   /*从文件读取Image*/
    QImage varImage;
    int varImageWidth = 0;
    int varImageHeight = 0;
    {/*从文件加载图片*/
        sstd::vector<uchar> varFileData;
        {
            const std::filesystem::path varImageFilePath(varNewImagePath?*varNewImagePath:argv[1]);
            const auto varImageFileSize = std::filesystem::file_size(varImageFilePath) + 4;
            if (varImageFileSize < 5) {
                qDebug() << "the image file is null";
                return -1;
            }
            if (varImageFileSize > (1024 * 1024 * 16)) {
                qDebug() << "the image file is too big";
                return -1;
            }
            varFileData.resize(varImageFileSize);
            std::ifstream varStream{ varImageFilePath,std::ios::binary };
            if (varStream.is_open() == false) {
                qDebug() << "can not open the image file";
                return -1;
            }
            varStream.read(reinterpret_cast<char*>(varFileData.data()), varImageFileSize);
#if !defined(NDEBUG)
            {
                const auto varStreamSize = varStream.gcount();
                assert(varStreamSize == (varImageFileSize-4));
            }
#endif
            varFileData.resize(varStream.gcount());
        }
        varImage = QImage::fromData(varFileData.data(),static_cast<int>(varFileData.size()));
        varImageHeight = varImage.height();
        varImageWidth = varImage.width();
        if ((varImageHeight < 1)||(varImageWidth<1)) { 
            qDebug() << "image file format error!";
            return -1;
        }
    }

    //QTimer::singleShot(0,[&varApp,&varImage]() {
    //    QImage varImageAns;
    //    auto openglThread = sstdNew<Thread>();
    //    openglThread->init();
    //    openglThread->openglProcessImage(varImage, &varImageAns);
    //    QObject::connect(openglThread, &QObject::destroyed,
    //        &varApp, &QCoreApplication::quit);
    //    openglThread->quit();
    //}) ;

   // return varApp.exec();

    auto window = new QWindow;
    window->setSurfaceType(QWindow::OpenGLSurface);
    window->create();
    auto thread = QThread::create([window]() {
        extern bool glewInitialize();
        QOpenGLContext $m$OpenGLContex;
        $m$OpenGLContex.create();
        $m$OpenGLContex.makeCurrent(window);
        qDebug() << glewInitialize();
    });

    thread->start();

    varApp.exec();

}











