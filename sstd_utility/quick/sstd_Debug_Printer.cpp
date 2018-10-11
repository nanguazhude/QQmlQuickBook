#if defined(_DEBUG)

#if defined(QT_CORE_LIB)

/*这里应当有预编译头文件支持*/
#include <QtCore/QtCore>
#include <sstd_memory.hpp>
#include <shared_mutex>
#include <charconv>
#include <iostream>

#define OUT_STREAM std::cout

static inline std::shared_mutex & getMutex() {
    static std::shared_mutex varMutex;
    return varMutex;
}

static inline void messageOutput(
    QtMsgType type,
    const QMessageLogContext &context,
    const QString &msg) {

    sstd::string varDebugInfo;

    {
        const static std::string_view varSP = "---------------------------------------\n"sv;
        std::string_view varFileName = context.file;
        std::string_view varFunctionName = context.function;
        const auto varLineNumber = context.line;

        {
            varDebugInfo += varSP;
        }
        {
            varDebugInfo += "fileName : "sv;
            varDebugInfo += varFileName;
            varDebugInfo += "\n"sv;
        }
        {
            varDebugInfo += "functionName : "sv;
            varDebugInfo += varFunctionName;
            varDebugInfo += "\n"sv;
        }
        {
            std::array<char, 128> varTmpData;
            auto[p, ec] = std::to_chars(
                varTmpData.data(),
                varTmpData.data() + varTmpData.size(),
                varLineNumber);
            varDebugInfo += "lineNumber : "sv;
            varDebugInfo += std::string_view(varTmpData.data(), p - varTmpData.data());
            varDebugInfo += "\n"sv;
        }

        const auto varMessage = msg.toLocal8Bit();
        varDebugInfo += std::string_view(varMessage.data(), varMessage.size());
    }

    std::unique_lock varLock{ getMutex() };

    switch (type) {
    case QtDebugMsg:
    {
        OUT_STREAM << varDebugInfo << std::endl;
        break;
    }
    case QtInfoMsg:
    {
        OUT_STREAM << varDebugInfo << std::endl;
        break;
    }
    case QtWarningMsg:
    {
        OUT_STREAM << varDebugInfo << std::endl;
        break;
    }
    case QtCriticalMsg:
    {
        OUT_STREAM << varDebugInfo << std::endl;
        break;
    }
    case QtFatalMsg:
    {
        OUT_STREAM << varDebugInfo << std::endl;
        break;
    }
    }

}

inline static void registerThis() {
    qInstallMessageHandler(&messageOutput);
}
Q_COREAPP_STARTUP_FUNCTION(registerThis)

#endif

#else/*Release*/


#endif

/*重定向qDebug输出*/
/**
Debugging QML Applications
//QT_NO_DEBUG_OUTPUT, QT_NO_INFO_OUTPUT, or QT_NO_WARNING_OUTPUT 
//QLoggingCategory
**/












