#include "ChatView.hpp"
#include "ChatViewPrivate.hpp"
#include "qml_chat_qtextdocumentlayout_p.h"

namespace sstd {

    ChatView::ChatView(QQuickItem *parent):Super(parent){
        thisp = sstdNew<ChatViewPrivate>(this);
    }

}/*namespace sstd*/












