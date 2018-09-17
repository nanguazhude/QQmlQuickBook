#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>

namespace sstd {
    class ChatView;
    class ChatViewPrivate : public QObject {
        Q_OBJECT
    public:
        ChatViewPrivate(ChatView *parent);
        ChatView * $m$Parent = nullptr;
    private:
        using Super = QObject;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatViewPrivate)
    };
}/*namespace sstd*/



















