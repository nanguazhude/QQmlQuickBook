#pragma once

#include "TextFrameItem.hpp"

namespace sstd {

    class ChatSimpleReply : public TextFrameItem {
        Q_OBJECT
    public:
        QByteArray getQmlBackgroundDelegateData() const override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatSimpleReply)
    };

}











