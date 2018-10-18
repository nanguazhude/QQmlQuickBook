#pragma once
#include <QtQuickWidgets/qquickwidget.h>
#include "sstd_quick_library_global.hpp"

namespace sstd {
    class _1_SSTD_QUICK_LIBRARY_EXPORT QuickViewWidget : public QQuickWidget {
        Q_OBJECT
    public:
        QuickViewWidget();
        QQuickWindow * getQuickWindow() const;
    private:
        using Super = QQuickWidget;
    };
}/*namespace sstd*/




















