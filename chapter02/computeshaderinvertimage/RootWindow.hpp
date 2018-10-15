#pragma once

#include <sstd_memory.hpp>
#include <quick/QuickView.hpp>

class QQuickItem;

class RootWindow : public sstd::DefaultRoowWindow {
    Q_OBJECT
public:
    RootWindow();
private:
    QQuickItem * mmm_ImageSource{ nullptr };
    QQuickItem * mmm_ImageTarget{ nullptr };
private:
    using Super = sstd::DefaultRoowWindow;
    SSTD_MEMORY_QOBJECT_DEFINE(RootWindow)
};


