#pragma once
#include <QObject>
#include <sstd_memory.hpp>

namespace sstd {



    class Player :
        public QObject,
        public virtual sstd::memory_lock::VirtualClassBasic {
        Q_OBJECT
    public:

        void setLocalFile(const QString &);
        bool start();

    public:
        Player();
    private:
        bool * mmm_IsStart;
        bool * mmm_ISLocalFile;
        QString * mmm_LocalFileName;
    private:
        bool ppp_start_local();
        bool ppp_start_net();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(Player)
    };









}/*namespace sstd*/















