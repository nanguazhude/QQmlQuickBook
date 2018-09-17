#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>

namespace sstd {
    class ChatViewPrivate;
    class ChatView : public QQuickItem {
        Q_OBJECT
    public:
        ChatView(QQuickItem *parent = nullptr);
    private:
        using Super = QQuickItem;
        ChatViewPrivate * thisp = nullptr;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatView)
    };
}/*namespace sstd*/



















