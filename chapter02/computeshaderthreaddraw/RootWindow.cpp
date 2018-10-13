#include <QtGui/qcolor.h>
#include "RootWindow.hpp"
#include "RenderSize.hpp"

RootWindow::RootWindow() {
    this->setTitle(trUtf8(u8R"(Root Window)"));
    this->setMinimumSize({ getRenderWidth(),getRenderHeight() });
    this->setMaximumSize({ getRenderWidth(),getRenderHeight() });
}







