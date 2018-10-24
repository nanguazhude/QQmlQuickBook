#pragma once

namespace sstd{

template<typename T>
class Array : public std::enable_shared_from_this<Array<T>> {
    using U = std::aligned_storage_t<sizeof(T), alignof(T)>;
    std::vector<U> mmm_Data;
    Array(const Array &) = delete;
    Array(Array &&) = delete;
    Array&operator=(const Array &) = delete;
    Array&operator=(Array&&) = delete;
public:

    inline static std::shared_ptr<Array<T>> make_array(std::size_t N) {
        return std::make_shared<Array<T>>(N);
    }

protected:
    Array(std::size_t N = 1) {
        mmm_Data.reserve(N);
        assert((N <= mmm_Data.capacity()) && "capacity should not less than N");
    }
public:

    using element_type = T;
    using value_type = std::remove_cv_t<element_type>;
    using index_type = std::ptrdiff_t;
    using difference_type = index_type;
    using pointer = T * ;
    using reference = T & ;
    using const_reference = const T &;
    using iterator = T * ;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    inline pointer data() const {
        return static_cast<T *>(mmm_Data.data());
    }

    inline auto size() const {
        return mmm_Data.size();
    }

    inline auto capacity() const {
        return mmm_Data.capacity();
    }

    inline iterator begin() {
        return data();
    }

    inline iterator end() {
        return data() + mmm_Data.size();
    }

    inline const_iterator cbegin() const noexcept {
        return data();
    }

    inline const_iterator cend() const noexcept {
        return begin() + mmm_Data.size();
    }

    template<typename ... K>
    inline T * push_back(K && ...args) {
        assert(size() < capacity());
        auto varMemory = &(mmm_Data.emplace_back());
        T * varAns{ nullptr };
        try {
            if constexpr (std::is_constructible_v<T, K&&...>) {
                varAns = ::new (varMemory) T(std::forward<K>(args)...);
            } else {
                varAns = ::new (varMemory) T{ std::forward<K>(args)... };
            }
        } catch (...) {
            mmm_Data.pop_back();
            throw;
        }
        return varAns;
    }

    inline ~Array() {
        if (false == mmm_Data.empty()) {
            std::destroy(begin(), end());
        }
    }

public:
    inline reverse_iterator rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }

    inline reverse_iterator rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }

    inline const_reverse_iterator crbegin() const noexcept {
        return std::make_reverse_iterator(cend());
    }

    inline const_reverse_iterator crend() const noexcept {
        return std::make_reverse_iterator(cbegin());
    }

};

    
}/*namespace sstd*/

