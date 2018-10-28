#pragma once

#include <array>
#include <memory>
#include <utility>
#include <type_traits>
#include <forward_list>

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
    };
    std::forward_list<RowData> mmm_Data;
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
};
}/*namespace sstd*/






