#pragma once
#include <QtQuickWidgets/qquickwidget.h>

namespace sstd {
    class QuickViewWidget : public QQuickWidget {
        Q_OBJECT
    public:
        QuickViewWidget();
    private:
        using Super = QQuickWidget;
    };
}/*namespace sstd*/




















