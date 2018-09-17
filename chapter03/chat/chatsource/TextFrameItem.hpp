#pragma once

#include <sstd_memory.hpp>
#include <QtCore/qobject.h>

namespace sstd{

    class TextFrameItem : public QObject {
        Q_OBJECT
    public:
        TextFrameItem(QObject * /**/=nullptr);
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(TextFrameItem)
    private:
        using Super = QObject;
    };

}/*namespace sstd*/















