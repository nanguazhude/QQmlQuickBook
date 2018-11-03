#pragma once
#include <QObject>
#include <sstd_memory.hpp>

namespace sstd {

    class PlayerPrivate;
    class Player :
        public QObject,
        public virtual sstd::memory_lock::VirtualClassBasic {
        Q_OBJECT
    public:
        void setLocalFile(const QString &);
        inline bool open();
        inline bool start(double=0);
    public:
        Player();
    private:
        bool * mmm_IsStart;
        bool * mmm_ISLocalFile;
        bool * mmm_IsConstruct;
        QString * mmm_LocalFileName;
        PlayerPrivate * mmm_Private;
        QString * mmm_ErrorString;
    private:
        bool ppp_construct_local();
        bool ppp_construct_net();
        bool ppp_construct();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Player)
    };

    inline bool Player::open() {
        ppp_construct();
    }







}/*namespace sstd*/















