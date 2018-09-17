#pragma once

#include <sstd_memory.hpp>
#include "qml_chat_qtextdocumentlayout_p.h"

namespace sstd {
    class ChatDocumentLayout : public QTextDocumentLayout {
        Q_OBJECT
    public:
        ChatDocumentLayout(QTextDocument *doc);
    private:
        using Super = QTextDocumentLayout;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatDocumentLayout)
    };
}







