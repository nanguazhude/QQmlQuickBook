#pragma once

#include <array>
#include <sstd_memory.hpp>
#include <QtQuick/qquickimageprovider.h>

namespace sstd {

    class QuickImageProvider : public QQuickImageProvider {
    public:
        QuickImageProvider();
        QImage requestImage(const QString &id, QSize *size, const QSize& requestedSize) override;
    public:
        static QString getIndexHeader();
        static QString getNextIndexHeader();
        static void addImage(const QString &, const QImage &);
        static QuickImageProvider * instance();
    private:
        using Super = QQuickImageProvider;
    private:
        SSTD_MEMORY_DEFINE(QuickImageProvider)
    };

} // namespace sstd


