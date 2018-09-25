#include "WindowOpenGLDrawControl.hpp"
#include <QtGui/qopenglcontext.h>
#include <QtGui/qopenglfunctions.h>

namespace {
    const QString getObjectName() { return  QStringLiteral("sstd::WindowOpenGLDrawControl"); }
}

sstd::WindowOpenGLDrawControl::WindowOpenGLDrawControl(QQuickWindow * window) {
    this->setObjectName(getObjectName());
    this->setParent(window);
    window->setClearBeforeRendering(true);
    connect(window, &QQuickWindow::beforeRendering, this, &WindowOpenGLDrawControl::beforeRendering, Qt::DirectConnection);
}

void sstd::WindowOpenGLDrawControl::beforeRendering() {
    auto varWindow = qobject_cast<QQuickWindow *>(this->parent());
    if (varWindow) {
        varWindow->resetOpenGLState();
    }
}

void sstd::WindowOpenGLDrawControl::setToWindow(QQuickWindow * arg) {
    if (arg == nullptr) { return; }
    if (arg->findChild<QObject *>(getObjectName())) { return; }
    sstdNew< WindowOpenGLDrawControl >(arg);
}








