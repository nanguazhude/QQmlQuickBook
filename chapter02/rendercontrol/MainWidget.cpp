#include "MainWidget.hpp"
#include "sstd_Render.hpp"
#include "sstd_RenderThread.hpp"

MainWidget::MainWidget() {
    this->setMinimumSize(128,128);
    this->resize(512,512);

    auto varRenderThread = sstdNew<sstd::RenderThread<sstd::Render>>();
    auto varRender = varRenderThread->getRender();

}













