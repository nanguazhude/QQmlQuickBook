#include "QuickViewWindow.hpp"

sstd::QuickViewWindow::QuickViewWindow(){

}

QQuickWindow * sstd::QuickViewWindow::getQuickWindow() const{
    return const_cast<QuickViewWindow*>(this);
}
















