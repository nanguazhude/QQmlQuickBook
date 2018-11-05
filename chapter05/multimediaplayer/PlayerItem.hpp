#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>

namespace sstd {

    class Player;
    class PlayerThread;
    class PlayerItem : public QQuickItem {
        Q_OBJECT
    private:
        Q_PROPERTY(QString localFile READ getLocalFile WRITE setLocalFile NOTIFY localFileChanged)
    public:
        PlayerItem();
    public:
        Q_SIGNAL void localFileChanged();
        QString getLocalFile() const;
        void setLocalFile(const QString &);
    public:
        Q_SLOT bool open();
        Q_SLOT bool start();
        Q_SLOT void stop();
    private:
        QString mmm_LocalFile;
        Player * mmm_Player{nullptr};
        PlayerThread * mmm_PlayerThread{nullptr};
    protected:
        QSGNode * updatePaintNode(QSGNode *, QQuickItem::UpdatePaintNodeData *)override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(PlayerItem)
    };

}/*******/







