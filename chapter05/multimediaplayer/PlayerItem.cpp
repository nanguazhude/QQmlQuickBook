#include "PlayerItem.hpp"
#include "MultimediaPlayer.hpp"
#include <QtCore/qcoreapplication.h>
#include <QtQml/QtQml>
#include <QtQuick/QtQuick>

namespace sstd {

    PlayerItem::PlayerItem() {

    }

    QSGNode * PlayerItem::updatePaintNode(QSGNode * argOldNode, QQuickItem::UpdatePaintNodeData *) {

        if (argOldNode == nullptr) {

        }

        return nullptr;

    }

    QString PlayerItem::getLocalFile() const {
        return mmm_LocalFile;
    }

    void PlayerItem::setLocalFile(const QString & arg) {
        if (arg == mmm_LocalFile) {
            return;
        }
        mmm_LocalFile = arg;
        localFileChanged();
    }

    bool PlayerItem::open() {

        if (mmm_LocalFile.isEmpty()) {

        } else {
            this->stop();
            mmm_Player = sstdNew< sstd::Player >();
            mmm_Player->setLocalFile(mmm_LocalFile);
            auto varAns = mmm_Player->open();
            if (varAns == false) {
                delete mmm_Player;
                mmm_Player = nullptr;
                return false;
            }
            mmm_PlayerThread = sstdNew< sstd::PlayerThread >();
            return true;
        }
        return false;
    }

    bool PlayerItem::start() {
        if (mmm_Player == nullptr) {
            return false;
        }
        if (false == mmm_Player->isOpen()) {
            return false;
        }
        mmm_PlayerThread->start(mmm_Player, 0);
        return true;
    }

    void PlayerItem::stop() {
        if (mmm_PlayerThread) {
            if (false == mmm_PlayerThread->isPlayerSet()) {
                delete mmm_Player;
            }
            mmm_PlayerThread->stop();
            mmm_PlayerThread = nullptr;
            mmm_Player = nullptr;
        }
    }

}/*namespace sstd*/


namespace {

    inline void start_this() {
        qmlRegisterType<sstd::PlayerItem>("myqml.multimediaplayer", 1, 0, "PlayerItem");
    }
    Q_COREAPP_STARTUP_FUNCTION(start_this)

}/*namespace*/












