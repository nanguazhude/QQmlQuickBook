#pragma once
#include <QtQuick/qquickview.h>

namespace sstd {

    class QuickViewWindow : public QQuickView {
        Q_OBJECT
    public:
        QuickViewWindow();
    private:
        using Super = QQuickView;
    };

}/*namespace sstd*/





















