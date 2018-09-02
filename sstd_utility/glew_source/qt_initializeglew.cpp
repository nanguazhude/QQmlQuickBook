#include <GL/glew.h>/*glew必须被第一个包含*/
#include <mutex>
#include <QtWidgets/qmessagebox.h>

static inline bool __qWindowInitializeGlew() {
    glewExperimental = GL_TRUE;
    int glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        QMessageBox box;
        box.setText("opengl/glew init error!");
        box.exec();
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

extern bool glewInitialize() {
    if constexpr (0) {
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

