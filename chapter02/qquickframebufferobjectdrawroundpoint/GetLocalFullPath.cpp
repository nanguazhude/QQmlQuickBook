#include <QtCore/qurl.h>
#include <QtCore/qdir.h>
#include <QtCore/qstring.h>
#include <QtCore/qcoreapplication.h>

namespace sstd {
    extern QUrl getLocalFileFullPath(const QString & arg);
}

namespace sstd {

#if defined(_DEBUG)
        const QDir varRootDir{ CURRENT_DEBUG_PATH };
#else
        const QDir varRootDir{ qApp->applicationDirPath() };
#endif

        const auto varAns = varRootDir.absoluteFilePath(arg);
        if( varAns.startsWith(QChar('/')) ){
            return QStringLiteral(R"(file://)")+varAns;
        }else{
            return QStringLiteral(R"(file:///)")+varAns;
        }
    }
}/*namespace sstd*/



