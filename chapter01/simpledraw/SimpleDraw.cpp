#include "SimpleDraw.hpp"
#include <QtGui/qopenglcontext.h>
#include <QtQuick/qquickwindow.h>

extern bool glewInitialize();

SimpleDraw::SimpleDraw(QQuickItem *parent) :Super(parent) {
    this->setFlag(QQuickItem::ItemHasContents, true);
    connect(this, &QQuickItem::windowChanged, this, &SimpleDraw::handleWindowChanged);
}

SimpleDrawOpenglRender::SimpleDrawOpenglRender() {}
SimpleDrawOpenglRender::~SimpleDrawOpenglRender() {}

void SimpleDraw::handleWindowChanged(QQuickWindow * window) {
    if (window) {
        connect(window, &QQuickWindow::beforeSynchronizing, this, &SimpleDraw::sync, Qt::DirectConnection);
        connect(window, &QQuickWindow::sceneGraphInvalidated, this, &SimpleDraw::cleanup, Qt::DirectConnection);
        window->setClearBeforeRendering(false);
    }

}

void SimpleDraw::sync() {

}

void SimpleDraw::cleanup() {

}

