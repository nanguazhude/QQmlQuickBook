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
        bool construct();
        bool start();
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
        bool ppp_start_net();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Player)
    };









}/*namespace sstd*/















