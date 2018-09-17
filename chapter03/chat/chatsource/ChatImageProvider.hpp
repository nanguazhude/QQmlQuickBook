#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickimageprovider.h>

namespace sstd{
    class ChatImageProvider
            : public QQuickImageProvider{
    public:
        ChatImageProvider();
        QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize)override;
    private:
        SSTD_MEMORY_DEFINE(ChatImageProvider)
    private:
        using Super = QQuickImageProvider;
    };
}/*namespace sstd*/








