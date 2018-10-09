#pragma once
#include <QtQuick/qquickview.h>

namespace sstd {

    class QuickViewWindow : public QQuickView {
        Q_OBJECT
    public:
        QuickViewWindow();
        QQuickWindow * getQuickWindow() const;
    private:
        using Super = QQuickView;
    };

}/*namespace sstd*/





















