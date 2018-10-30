#pragma once

#include <array>
#include <memory>
#include <utility>
#include <type_traits>
#include <forward_list>
#include "sstd_memory.hpp"

namespace sstd{
    template<typename T, std::size_t NEveryRowSize = 64>
class MemoryLock {
private:
    static_assert(false == std::is_reference_v<T>);
    static_assert(false == std::is_array_v<T>);
    static_assert(NEveryRowSize > 0);
    using _U0 = std::remove_reference_t<T>;
    using _U = std::remove_cv_t<_U0>;
    class alignas(_U) ItemWrap {
        _U mmm_Data;
    public:
        template<typename ... K,
            typename = std::enable_if_t<((sizeof...(K)) != 0) &&
            (true == std::is_constructible_v<_U, K &&...>)
        > >
            ItemWrap(K && ... arg) :mmm_Data(std::forward<K>(arg)...) {
        }
        template<typename ... K,
            typename = int,
            typename = std::enable_if_t<((sizeof...(K)) == 0) ||
            (false == std::is_constructible_v<_U, K &&...>)
        > >
            ItemWrap(K && ... arg) :mmm_Data{ std::forward<K>(arg)... } {
        }
    public:
        ItemWrap(const ItemWrap &) = delete;
        ItemWrap(ItemWrap &&) = delete;
        ItemWrap&operator=(const ItemWrap &) = delete;
        ItemWrap&operator=(ItemWrap &&) = delete;
    public:
        _U * getData() {
            return &mmm_Data;
        }
    private:
        SSTD_MEMORY_DEFINE(ItemWrap)
    };
    using _RawData = std::aligned_storage_t<sizeof(ItemWrap), alignof(ItemWrap)>;
    constexpr const static std::size_t _row_size = NEveryRowSize;
    using _RowData = std::array<_RawData, _row_size>;
    class RowData : public _RowData {
    public:
        static constexpr std::size_t nopos_() {
            return 0;
        }
        std::size_t current = nopos_();
        bool isFull() const {
            return  current >= (this->size());
        }
        ItemWrap * begin_() const {
            return reinterpret_cast<ItemWrap *>(
                const_cast<_RawData *>(this->data()));
        }
        ItemWrap * end_() const {
            return  begin_() + current;
        }
        ItemWrap * next_() {
            if (isFull()) {
                return nullptr;
            }
            auto varAns = begin_() + current;
            ++current;
            return varAns;
        }
        void back_() {
            --current;
        }
        ~RowData() {
            if constexpr (false == std::is_trivially_destructible_v<_U>) {
                if (current == nopos_()) {
                    return;
                }
                std::destroy(
                    std::make_reverse_iterator(end_()),
                    std::make_reverse_iterator(begin_()));
            }
        }
        RowData() = default;
    public:
        RowData(const RowData &) = delete;
        RowData(RowData &&) = delete;
        RowData&operator=(const RowData &) = delete;
        RowData&operator=(RowData &&) = delete;
        SSTD_MEMORY_DEFINE(RowData)
    };
    sstd::forward_list<RowData> mmm_Data;
public:
    MemoryLock() = default;
    template<typename ... K>
    T * emplace_back(K && ... arg) {
        if (mmm_Data.empty()) {
            mmm_Data.emplace_front();
        }
        ItemWrap * varData = mmm_Data.front().next_();
        if (varData == nullptr) {
            mmm_Data.emplace_front();
            varData = mmm_Data.front().next_();
        }
        try {
            ::new (varData) ItemWrap(std::forward<K>(arg)...);
        } catch (...) {
            mmm_Data.front().back_();
            throw;
        }
        return varData->getData();
    }
    MemoryLock(MemoryLock&&) = default;
    MemoryLock&operator=(MemoryLock&&) = default;
private:
    MemoryLock&operator=(const MemoryLock&) = delete;
    MemoryLock(const MemoryLock&) = delete;
    SSTD_MEMORY_DEFINE(MemoryLock)
};

    namespace memory_lock {
        namespace _0_private {
            class _1_SSTD_CORE_EXPORT VirtualBasic {
                SSTD_DELETE_COPY_ASSIGN(VirtualBasic);
            private:
            protected:
                VirtualBasic();
            public:
                virtual ~VirtualBasic();
            public:
                SSTD_MEMORY_DEFINE(VirtualBasic)
            };

            template<typename U>
            class ItemWrap : public VirtualBasic {
                U mmm_Data;
            public:
                ItemWrap() :mmm_Data{} {
                }
                template<typename T1, typename ... TN,
                    typename = std::enable_if_t<true == std::is_constructible_v<U, T1&&, TN&&...>>>
                    ItemWrap(T1 && arg0, TN && ... args) : mmm_Data(std::forward<T1>(arg0),
                        std::forward<TN>(args)...) {
                }
                template<typename T1, typename ... TN,
                    typename = int,
                    typename = std::enable_if_t<false == std::is_constructible_v<U, T1&&, TN&&...>>>
                    ItemWrap(T1 && arg0, TN && ... args) : mmm_Data{ std::forward<T1>(arg0),
                        std::forward<TN>(args)... } {
                }
                U * get_this_data() {
                    return &mmm_Data;
                }
            };

            template<typename U>
            class NamedItemWrap : public ItemWrap<U> {
                using Super = ItemWrap<U>;
                sstd::string mmm_Name;
            public:
                template<typename ... Args>
                NamedItemWrap(Args && ... args) : Super( std::forward<Args>(args)... ) {
                }
                void setName(std::string_view n) {
                    mmm_Name = n;
                }
                std::string_view getName()const {
                    return mmm_Name;
                }
            };

        }/*_0_private*/

        class _1_SSTD_CORE_EXPORT VirtualClassBasic {
            SSTD_DELETE_COPY_ASSIGN(VirtualClassBasic);
        private:
            class DataType : 
                public MemoryLock< std::unique_ptr< _0_private::VirtualBasic >, 15 > {
            public:

                class NamedObject {
                public:
                    void * data;
                    NamedObject(void * d) :data(d) {
                    }
                    NamedObject(const NamedObject &) = default;
                    NamedObject(NamedObject&&) = default;
                    NamedObject&operator=(const NamedObject &) = default;
                    NamedObject&operator=(NamedObject&&) = default;
                };

                using NamedObjectMap = sstd::map< std::string_view, NamedObject >;
                NamedObjectMap * indexed_object{nullptr};
            };
            DataType * mmm_data;
        protected:
            VirtualClassBasic() noexcept;
        public:
            virtual ~VirtualClassBasic();
        public:
            template<typename T,typename ... Args>
            inline T * create_object_in_this_class(Args && ...);
            template<typename T, typename ... Args>
            inline T * create_named_object_in_this_class(std::string_view,Args && ...);
            template<typename T>
            inline T * find_named_object_in_this_class(const std::string_view &) const;
        public:
            SSTD_MEMORY_DEFINE(VirtualClassBasic)
        };

        template<typename T, typename ... Args>
        inline T * VirtualClassBasic::create_object_in_this_class(Args && ... args) {
            static_assert(false==std::is_array_v<T>);
            static_assert(false==std::is_reference_v<T>);
            using U0 = std::remove_reference_t<T>;
            using U1 = std::remove_cv_t<U0>;
            using U = U1;
            using ItemWrap = _0_private::ItemWrap<U>;
            auto varAnsUnique = sstd::make_unique<ItemWrap>( std::forward<Args>(args)... );
            auto varAns = varAnsUnique->get_this_data();
            this->mmm_data->emplace_back( std::move(varAnsUnique) );
            return varAns;
        }

        template<typename T, typename ... Args>
        inline T * VirtualClassBasic::create_named_object_in_this_class(std::string_view name, Args && ... args) {
            
            if ( name.empty() ) {
                return nullptr;
            }

           {
                auto varAns = find_named_object_in_this_class<T>(name);
                if (varAns) {
                    return varAns;
                }
           }
            
           static_assert(false == std::is_array_v<T>);
           static_assert(false == std::is_reference_v<T>);
           using U0 = std::remove_reference_t<T>;
           using U1 = std::remove_cv_t<U0>;
           using U = U1;
           using ItemWrap = _0_private::NamedItemWrap<U>;
           auto varAnsUnique = sstd::make_unique<ItemWrap>(std::forward<Args>(args)...);
           auto varAns = varAnsUnique->get_this_data();
           varAnsUnique->setName(name);
           name = varAnsUnique->getName();
           this->mmm_data->emplace_back(std::move(varAnsUnique));
           this->mmm_data->indexed_object->emplace(name, 
               DataType::NamedObject{ (void*)(varAns) });
           return varAns;

        }

        template<typename T>
        inline T * VirtualClassBasic::find_named_object_in_this_class(const std::string_view & name) const {
            auto varPos = mmm_data->indexed_object->find(name);
            if (varPos != mmm_data->indexed_object->end()) {
                return (T *)(varPos->second.data);
            }
            return nullptr;
        }
       
    }/*namespace memroy*/

}/*namespace sstd*/






