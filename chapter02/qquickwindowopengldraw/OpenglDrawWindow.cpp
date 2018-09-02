#include "OpenglDrawWindow.hpp"
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickwindow.h>

extern bool glewInitialize();

OpenglDrawWindow::OpenglDrawWindow(QQuickItem *parent) :Super(parent) {
    this->setFlag(QQuickItem::ItemHasContents, true);
    connect(this, &QQuickItem::windowChanged, this, &OpenglDrawWindow::handleWindowChanged);
}

OpenglDrawWindowRender::OpenglDrawWindowRender() {}
OpenglDrawWindowRender::~OpenglDrawWindowRender() {}

void OpenglDrawWindow::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &OpenglDrawWindow::sync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &OpenglDrawWindow::cleanup, Qt::DirectConnection);
        window->setClearBeforeRendering(false);
    }

}

void OpenglDrawWindow::sync() {

}

void OpenglDrawWindow::cleanup() {

}

