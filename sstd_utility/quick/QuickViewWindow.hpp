#pragma once
#include <QtQuick/qquickview.h>
#include "sstd_quick_library_global.hpp"

namespace sstd {

    class _1_SSTD_QUICK_LIBRARY_EXPORT QuickViewWindow : public QQuickView {
        Q_OBJECT
    public:
        QuickViewWindow();
        QQuickWindow * getQuickWindow() const;
    private:
        using Super = QQuickView;
    };

}/*namespace sstd*/





















