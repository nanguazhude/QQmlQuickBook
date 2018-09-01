#include <GL/glew.h>
#include <QtWidgets/qmessagebox.h>
#include <mutex>

static inline bool __qWindowInitializeGlew() {
    glewExperimental = GL_TRUE;
    int glewErr = glewInit();
    if (glewErr != GLEW_OK){
        QMessageBox box;
        box.setText("opengl/glew init error!");
        box.exec();
        return false;
    }/****/
    return true;
}

static inline void __run_once_wrap(bool * ans){
    *ans = __qWindowInitializeGlew();
}

static inline std::once_flag & __call_once_flag(){
    static std::once_flag ans;
    return ans;
}

extern bool glewInitialize() {
    static bool ans = false;
    std::call_once( __call_once_flag() , &__run_once_wrap , &ans );
    return ans;
}

