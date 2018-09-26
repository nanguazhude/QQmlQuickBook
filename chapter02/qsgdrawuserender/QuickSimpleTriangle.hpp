#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qsgrendernode.h>

namespace sstd{

class SimpleTriangleNode : public QSGRenderNode {
private:
    SSTD_MEMORY_DEFINE(SimpleTriangleNode)
};

class SimpleTriangle : public QQuickItem {
    Q_OBJECT
private:
    SSTD_MEMORY_QOBJECT_DEFINE(SimpleTriangle)
};


}/*namespace sstd*/














