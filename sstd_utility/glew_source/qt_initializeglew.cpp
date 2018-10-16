#include "../sstd_glew.hpp"
#include <mutex>
#include "../sstd_memory.hpp"
#if defined(QT_WIDGETS_LIB)
#include <QtWidgets/qmessagebox.h>
#endif
#include <QtCore/qdebug.h>
#include <atomic>
#include <cstdlib>

namespace {
    /*如果一些平台不支持std::quick_exit,用std::exit代替*/
    [[noreturn]] static inline void this_file_quick_exit() {
        std::quick_exit(-1);
    }/*this_file_quick_exit()*/
}/**/

struct SimpleCallBack {

    using mutex_type = std::recursive_mutex;
    class string : public sstd::string {
        using super = sstd::string;
    public:
        string(const std::string_view &arg) :
            super(arg.data(), arg.size()) {
        }
    };


    static inline mutex_type & getMutex() {
        static auto var = new mutex_type/*never delete*/;
        return *var;
    }

    template<typename T>
    struct Print {
        static void print(const string & name, T value) {
            qDebug()
                << QString::fromUtf8(name.c_str())
                << hex << value << QStringLiteral(",")
                << dec << value;
        }
    };

    template<>
    struct Print<std::string_view> {
        static void print(const string & name, GLenum s, const std::string_view & value) {
            qDebug()
                << QString::fromUtf8(name.c_str())
                << hex << s << QStringLiteral(",")
                << QString::fromUtf8(value.data(), static_cast<int>(value.size()));

        }
    };

    static inline std::string_view severity_to_string(GLenum i) {
        switch (i) {
            case GL_DEBUG_SEVERITY_LOW:return "GL_DEBUG_SEVERITY_LOW"sv;
            case GL_DEBUG_SEVERITY_MEDIUM:return "GL_DEBUG_SEVERITY_MEDIUM"sv;
            case GL_DEBUG_SEVERITY_HIGH:return "GL_DEBUG_SEVERITY_HIGH"sv;
            case GL_DEBUG_SEVERITY_NOTIFICATION:return "GL_DEBUG_SEVERITY_NOTIFICATION"sv;
        }
        return "Unknow Severity"sv;
    }

    static inline std::string_view source_to_string(GLenum i) {
        switch (i) {
            case  GL_DEBUG_SOURCE_API:return "GL_DEBUG_SOURCE_API"sv;
            case  GL_DEBUG_SOURCE_WINDOW_SYSTEM:return "GL_DEBUG_SOURCE_WINDOW_SYSTEM"sv;
            case  GL_DEBUG_SOURCE_SHADER_COMPILER:return "GL_DEBUG_SOURCE_SHADER_COMPILER"sv;
            case  GL_DEBUG_SOURCE_THIRD_PARTY:return "GL_DEBUG_SOURCE_THIRD_PARTY"sv;
            case  GL_DEBUG_SOURCE_APPLICATION: return "GL_DEBUG_SOURCE_APPLICATION"sv;
            case  GL_DEBUG_SOURCE_OTHER:return "GL_DEBUG_SOURCE_OTHER"sv;
        }
        return "Unknow Source"sv;
    }

    static inline std::string_view type_to_string(GLenum i) {
        switch (i) {
            case  GL_DEBUG_TYPE_ERROR:return "GL_DEBUG_TYPE_ERROR"sv;
            case  GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"sv;
            case  GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"sv;
            case  GL_DEBUG_TYPE_PORTABILITY:return "GL_DEBUG_TYPE_PORTABILITY"sv;
            case  GL_DEBUG_TYPE_PERFORMANCE:return "GL_DEBUG_TYPE_PERFORMANCE"sv;
            case  GL_DEBUG_TYPE_MARKER:return "GL_DEBUG_TYPE_MARKER"sv;
            case  GL_DEBUG_TYPE_PUSH_GROUP:return "GL_DEBUG_TYPE_PUSH_GROUP"sv;
            case  GL_DEBUG_TYPE_POP_GROUP:return "GL_DEBUG_TYPE_POP_GROUP"sv;
            case  GL_DEBUG_TYPE_OTHER:return "GL_DEBUG_TYPE_OTHER"sv;
        }
        return "Unknow Type"sv;
    }

    static std::atomic<std::int64_t>& getDebugGroupCount() {
        static auto * ans = new std::atomic<std::int64_t>{ 0 }/*never delete*/;
        return *ans;
    }

    static void GLAPIENTRY callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* /*userParam*/
    ) {

        {/*加入或移出当前调试组*/
            if ((source == GL_DEBUG_SOURCE_APPLICATION) &&
                (id == sstd::$p$sstdGlewDebugGroupIndex())) {
                if ((type == GL_DEBUG_TYPE_PUSH_GROUP)) {
                    ++getDebugGroupCount();
                    return;
                } else if (type == GL_DEBUG_TYPE_POP_GROUP) {
                    --getDebugGroupCount();
                    return;
                }
            }
        }

        /*不在当前调试组则退出*/
        if (getDebugGroupCount().load() < 1) {
            return;
        }

        /*忽略不需要的调试信息*/
        if (((severity == GL_DEBUG_SEVERITY_NOTIFICATION) ||
#if 0
            (severity == GL_DEBUG_SEVERITY_LOW))
            && (source == GL_DEBUG_SOURCE_API)
#else
            false)
#endif
            ) {
            /*忽略来自显卡的提示信息*/
            return;
        }

        std::unique_lock varLock{ getMutex() }/*防止多线程乱码*/;
        static std::uint64_t varDebugIndex = 0;
        Print<std::string_view>::print("source:"sv, source, source_to_string(source));
        Print<std::string_view>::print("type:"sv, type, type_to_string(type));
        Print<GLuint>::print("id:"sv, id);
        Print<std::string_view>::print("severity:"sv, severity, severity_to_string(severity));
        std::string mes(message, length);

        {
            QString varDebugText;
            {
                QTextStream varStream{ &varDebugText };
                varStream << QStringLiteral("message: ")
                    << QString::fromUtf8(mes.c_str())
                    << endl
                    << "log index : "
                    << ++varDebugIndex
                    << endl
                    << QStringLiteral("--------------------------------------");
            }
            qDebug() << varDebugText;
        }

    }

};

static inline void setSimpleCallbackFunction() {

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    glDebugMessageControl(
        GL_DONT_CARE/*source*/,
        GL_DONT_CARE/*type*/,
        GL_DONT_CARE/*severity*/,
        0/*count*/,
        nullptr/*ids*/,
        true);

    glDebugMessageCallback(
        &(SimpleCallBack::callback),
        0);

    GLint v = 0x01;
    glGetIntegerv(GL_CONTEXT_FLAGS, &v);
    if (GL_CONTEXT_FLAG_DEBUG_BIT&v) {
        qDebug() << "simple debug function set!";
    } else {
        qDebug() << "debug function set failed!";
    }
}

static inline bool __qWindowInitializeGlew() {
    glewExperimental = GL_TRUE;
    int glewErr = glewInit();
    if (glewErr != GLEW_OK) {

#if defined(QT_WIDGETS_LIB)
        QMessageBox box;
        box.setText("opengl/glew init error!");
        box.exec();
#else
        qDebug() << "opengl/glew init error!";
#endif

        this_file_quick_exit();
        return false;
    }/****/
    return true;
}

static inline void __run_once_wrap(bool * ans) {
    *ans = __qWindowInitializeGlew();
}

static inline std::once_flag & __call_once_flag() {
    static std::once_flag ans;
    return ans;
}

static inline void initializeOpenGLDebug() {
    /****************************************/
    //OpengGL Debug Function
#if defined(ENABLE_GL_DEBUG)
    setSimpleCallbackFunction();
#else
    glDisable(GL_DEBUG_OUTPUT);
    glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
    /****************************************/
}

extern bool glewInitialize() {

    if constexpr (true) {
        /*只初始化一次就可...*/
        static bool ans = false;
        std::call_once(__call_once_flag(), &__run_once_wrap, &ans);
        initializeOpenGLDebug();
        return ans;
    } else {
        /*每个线程初仅始化opengl一次*/
        thread_local const bool varAns = __qWindowInitializeGlew();
        initializeOpenGLDebug();
        return varAns;
    }

}

/**
glPushDebugGroup pushes a debug group described by the string message​
into the command stream, and emits a message as if glDebugMessageInsert
were called. The value of id​ specifies the ID of messages generated.
The parameter length​ contains the number of characters in message​.
If length​ is negative, it is implied that message​ contains a null
terminated string. The value of source​ must be
GL_DEBUG_SOURCE_APPLICATION or DEBUG_SOURCE_THIRD_PARTY.
The generated message will have the same source​ and id​;
the value of type​ will be GL_DEBUG_TYPE_PUSH_GROUP and the value
of severity​ will be GL_DEBUG_SEVERITY_NOTIFICATION.
The GL will put a new debug group on top of the debug group
stack which inherits the control of the volume of debug output
of the debug group previously residing on the top of the debug
group stack. Because debug groups are strictly hierarchical,
any additional control of the debug output volume will only apply
within the active debug group and the debug groups pushed on top
of the active debug group.
**/

