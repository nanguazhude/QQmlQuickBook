#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>

namespace sstd {

    class PlayerItem : public QQuickItem {
        Q_OBJECT
    public:
        PlayerItem();
    protected:
        QSGNode * updatePaintNode(QSGNode *, QQuickItem::UpdatePaintNodeData *)override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(PlayerItem)
    };

}/*******/







