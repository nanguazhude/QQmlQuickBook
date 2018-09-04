#include <GL/glew.h>/*glew必须被第一个包含*/
#include <mutex>
#include "../sstd_memory.hpp"
#include <QtWidgets/qmessagebox.h>
#include <QtCore/qdebug.h>

struct SimpleCallBack {

    using mutex_type = std::recursive_mutex;
    class string : public sstd::string {
        using super = sstd::string;
    public:
        string(const std::string_view &arg) :
            super(arg.data(), arg.size()) {}
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

    static void GLAPIENTRY callback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* /*userParam*/
    ) {

        if ((severity == GL_DEBUG_SEVERITY_NOTIFICATION)
            && (source == GL_DEBUG_SOURCE_API)) {
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
        qDebug()
            << QStringLiteral("message: ")
            << QString::fromUtf8(mes.c_str())
            << endl
            << "log index : "
            << ++varDebugIndex
            << endl
            << QStringLiteral("--------------------------------------");
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
    }
    else {
        qDebug() << "debug function set failed!";
    }
}

static inline bool __qWindowInitializeGlew() {
    glewExperimental = GL_TRUE;
    int glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        QMessageBox box;
        box.setText("opengl/glew init error!");
        box.exec();
        return false;
    }/****/
    /****************************************/
    //OpengGL Debug Function
#if defined(ENABLE_GL_DEBUG)
    setSimpleCallbackFunction();
#endif
    /****************************************/
    return true;
}

static inline void __run_once_wrap(bool * ans) {
    *ans = __qWindowInitializeGlew();
}

static inline std::once_flag & __call_once_flag() {
    static std::once_flag ans;
    return ans;
}

extern bool glewInitialize() {
    if constexpr (true) {
        /*如果使用者确信只有一个线程使用opengl，则采用此函数初始化opengl*/
        static bool ans = false;
        std::call_once(__call_once_flag(), &__run_once_wrap, &ans);
        return ans;
    }
    else {
        /*每个线程初仅始化opengl一次*/
        thread_local const bool varAns = __qWindowInitializeGlew();
        return varAns;
    }
}

/*
"--------------------------------------"
"source:" 8246 "," "GL_DEBUG_SOURCE_API"
"type:" 824c "," "GL_DEBUG_TYPE_ERROR"
"id:" 500 "," 1280
"severity:" 9146 "," "GL_DEBUG_SEVERITY_HIGH"
"message: " "GL_INVALID_ENUM error generated. <pname> requires feature(s) disabled in the current profile."
"--------------------------------------"
*/

