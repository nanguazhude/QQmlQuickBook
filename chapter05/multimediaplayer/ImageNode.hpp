#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>

namespace sstd {

class ImageNode : public QQuickItem {
Q_OBJECT
public:
    ImageNode();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(ImageNode)
};

}/*******/







