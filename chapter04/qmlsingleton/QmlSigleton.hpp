/*QmlSigleton.hpp*/
#pragma once
#include <sstd_memory.hpp>
#include <mutex>
#include <shared_mutex>
#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>

namespace sstd{
    namespace qmlsigleton{
        template<typename T>
        class DataItem{
            T mmm_Data;
            mutable std::shared_mutex mmm_Mutex;
        public:
            DataItem() =default ;
            template<typename U>
            DataItem(U && arg):mmm_Data(std::forward<U>(arg)) {}
            inline const T & getData() const {
                std::shared_lock varReadLock{mmm_Mutex};
                return mmm_Data;
            }
            template<typename U>
            inline bool setData( U && arg ){
                T varDataTmp { std::forward<U>(arg) };
                {/*check input is same or not ...*/
                    std::shared_lock varReadLock{mmm_Mutex};
                    if(varDataTmp == mmm_Data){
                        return false;
                    }
                }
                std::unique_lock varWriteLock{mmm_Mutex};
                mmm_Data = std::move(varDataTmp);
                return true;
            }
        private:
            SSTD_MEMORY_DEFINE(DataItem)
        };
        
    }/*namespace qmlsigleton*/
}/*namespace sstd*/

class QmlSigleton : public QObject {
    Q_OBJECT
private:
    sstd::qmlsigleton::DataItem<QColor> mmm_MyColor{ QColor(128,128,128,255) };
    Q_PROPERTY(QColor myColor READ getMyColor WRITE setMyColor NOTIFY myColorChanged)
public:
    inline QColor getMyColor() const {
        return mmm_MyColor.getData();
    }
    inline void setMyColor(const QColor & arg){
        if( mmm_MyColor.setData(arg) ){
            myColorChanged();
        }
    }
    Q_SIGNAL void myColorChanged();
public:
    static QObject * instance();
public:
    QmlSigleton();
private:
    SSTD_MEMORY_QOBJECT_DEFINE(QmlSigleton)
};


















