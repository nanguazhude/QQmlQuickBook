#define  _WIN32_WINNT 0x0501
#include <boost/asio.hpp>
/*引入各个模块的头文件*/
#include <QtGui>
#include <QtQml>
#include <QtCore>
#include <QtQuick>
#include <QtWidgets>

#include <ctime>
#include <cstdlib>

#include <boost/context/fiber.hpp>

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

constexpr const static std::size_t varMainStackSize = 1024 * 1024 * 10;
class MainPack : public QObject {
public:
    int argc;
    char ** argv;
    int ans{ 0 };
    bool isQuit{ false };
} varMainPack;
using Fiber = boost::context::fiber;

void mainSwapFunction(Fiber * argFiber) {
    if (*argFiber) {
        /*处理完当前Qt事件队列中的所有事件*/
        qApp->processEvents();
        /*切换到主调度函数*/
        *argFiber = std::move(*argFiber).resume();
    }
    /*1ms之后再次运行此函数*/
    QTimer::singleShot(1, &varMainPack, [argFiber]() {
        if (varMainPack.isQuit) {
            return;
        }
        mainSwapFunction(argFiber); }
    );
};


class IOService : public boost::asio::execution_context::service {
    using Super = boost::asio::execution_context::service;
public:
    IOService(boost::asio::execution_context & arg) : Super(arg) {
    }
    void shutdown() {
    }
    void notify_fork(boost::asio::execution_context::fork_event ) override {
        qDebug() << "????";
    }

    

};

 

class IOContext : public boost::asio::io_context {
    using Super = boost::asio::io_context;
public:
    IOContext() {
        boost::asio::add_service(*this,sstdNew<IOService>(*this));
    }
    
};


int main(int argc, char ** argv) {
    {
        varMainPack.argc = argc;
        varMainPack.argv = argv;
    }

    auto varMainFunction = [](Fiber && argFiber) ->Fiber {
        /*高分屏支持*/
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        /*重置随机数种子*/
        resetRandom();
        /*初始化Qt环境*/
        Application varApp(varMainPack.argc, varMainPack.argv);
        /*此协程退出，标记为主函数退出*/
        class MainLock {
        public:
            MainLock() = default;
            ~MainLock() {
                varMainPack.isQuit = true;
            }
        } varMainLock{};
        /*强制加载Qt插件*/
        loadQtPlugins();
        /*初始化Qml/Quick引擎*/
        RootWindow varWindow;
        {
            /*main.qml完整目录*/
            const auto varMainQmlFileName =
                sstd::getLocalFileFullPath(QStringLiteral("myqml/runcontrol/main.qml"));
            /*加载main.qml*/
            varWindow.load(varMainQmlFileName);
            /*检查并报错*/
            if (varWindow.status() != sstd::LoadState::Ready) {
                qDebug() << "can not load : " << varMainQmlFileName;
                varMainPack.ans = -1;
                return std::move(argFiber);
            } else {
                varWindow.show();
            }
        }
        /*启动轮询切换到主调度函数*/
        mainSwapFunction(&argFiber);
        /*启动主线程事件循环程序*/
        varMainPack.ans = varApp.exec();
        return std::move(argFiber);
    };

    auto varNetWorkFunction = [](Fiber && argFiber)->Fiber {
        IOContext varNetworkContext;
        while (varMainPack.isQuit == false) {
            while (varNetworkContext.poll_one()) {
                 
            }
            boost::asio::post(varNetworkContext, 
                []() {qDebug() << "do something about network!"; });
            argFiber = std::move(argFiber).resume() /*切换到主调度函数*/;
        }
        return std::move(argFiber);
    };

    Fiber varMain(
        std::allocator_arg,
        boost::context::fixedsize_stack(varMainStackSize),
        varMainFunction);

    Fiber varNetWork(
        std::allocator_arg,
        boost::context::fixedsize_stack(varMainStackSize),
        varNetWorkFunction
    );

    /*主调度函数*/
    while (varMainPack.isQuit == false) {
        varMain = std::move(varMain).resume();
        varNetWork = std::move(varNetWork).resume();
    }

    return varMainPack.ans;
}

