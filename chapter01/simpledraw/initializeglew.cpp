#include <GL/glew.h>
#include <QtWidgets/qmessagebox.h>

static inline bool __qWindowInitializeGlew() {
    glewExperimental = GL_TRUE;
    int glewErr = glewInit();
    if (glewErr != GLEW_OK){
        QMessageBox box;
        box.setText("opengl/glew init error!");
        box.exec();
        return false;
    }//
    return true;
}

extern bool glewInitialize() {
    return  __qWindowInitializeGlew();
}

