#pragma once

#include <sstd_memory.hpp>
#include "qml_chat_qtextdocumentlayout_p.h"

namespace sstd {
    class ChatView;
    class ChatDocumentLayout : public QTextDocumentLayout {
        Q_OBJECT
    public:
        ChatDocumentLayout(ChatView *,QTextDocument *doc);
    private:
        using Super = QTextDocumentLayout;
        ChatView * $m$ChatView = nullptr;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatDocumentLayout)
    };
}







