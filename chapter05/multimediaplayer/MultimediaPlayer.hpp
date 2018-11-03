#pragma once
#include <QtCore/qobject.h>
#include <QtCore/qthread.h>
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
        bool start(double=0);
        inline const QString & getError()const;
    public:
        Player();
        Q_SIGNAL void finished();
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
        return ppp_construct();
    }

    inline const QString & Player::getError()const {
        return *mmm_ErrorString;
    }

    class PlayerThread : public QThread {
        Q_OBJECT
    public:
        PlayerThread();
    public:
        void stop();
        void startLocal(const QString &);
    protected:
        void run() override;
    private:
        Player * mmm_Player{ nullptr };
        QString mmm_LocalPath;
        QString mmm_Error;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(PlayerThread)
    };
    
    
}/*namespace sstd*/















