#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
class QTextDocument;
class QQmlComponent;

namespace sstd {
    class ChatView;
    class ChatViewPrivate : public QObject {
        Q_OBJECT
    public:
        ChatViewPrivate(ChatView *parent);
        ChatView * $m$Parent = nullptr;
        QTextDocument * $m$Document = nullptr;
        QQuickItem * $m$TextEdit = nullptr;
        QQmlComponent * $m$TextEditDelegate = nullptr;
        QQmlComponent * $m$TextFrameDelegate = nullptr;
    private:
        using Super = QObject;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatViewPrivate)
    };
}/*namespace sstd*/



















