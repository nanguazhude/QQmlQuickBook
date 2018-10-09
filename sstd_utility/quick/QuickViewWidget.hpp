#pragma once
#include <QtQuickWidgets/qquickwidget.h>

namespace sstd {
    class QuickViewWidget : public QQuickWidget {
        Q_OBJECT
    public:
        QuickViewWidget();
        QQuickWindow * getQuickWindow() const;
    private:
        using Super = QQuickWidget;
    };
}/*namespace sstd*/




















