
#pragma once

#include <sstd_memory.hpp>
#include <mutex>
#include <shared_mutex>

namespace sstd {
    namespace qmlsigleton {

        template<typename T>
        class DataItem {
            T mmm_Data;
            mutable std::shared_mutex mmm_Mutex;
        public:
            DataItem() = default;
            template<typename U>
            DataItem(U && arg) :mmm_Data(std::forward<U>(arg)) {}

            inline T getData() const {
                std::shared_lock<std::shared_mutex> varReadLock{ mmm_Mutex };
                return mmm_Data;
            }

            template<typename U>
            inline bool setData(U && arg) {
                using Input = std::remove_cv_t<
                std::remove_reference_t<U> >;
                using ThisDataType = std::remove_cv_t< T >;
                if constexpr ( std::is_same_v<Input, ThisDataType > ){
                    {/*check input is same or not ...*/
                        std::shared_lock varReadLock{ mmm_Mutex };
                        if (arg == mmm_Data) {
                            return false;
                        }
                    }
                    std::unique_lock varWriteLock{ mmm_Mutex };
                    mmm_Data = std::forward<U>(arg);
                    return true;
                }else{
                    ThisDataType varDataTmp{ std::forward<U>(arg) };
                    {/*check input is same or not ...*/
                        std::shared_lock varReadLock{ mmm_Mutex };
                        if (varDataTmp == mmm_Data) {
                            return false;
                        }
                    }
                    std::unique_lock varWriteLock{ mmm_Mutex };
                    mmm_Data = std::move(varDataTmp);
                    return true;
                }
            }

        private:
            SSTD_MEMORY_DEFINE(DataItem)
        };

    }/*namespace qmlsigleton*/
}/*namespace sstd*/












