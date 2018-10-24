#pragma once

namespace sstd{

template<typename T >
class Array {
    template<typename T> using A = std::allocator<T>;
    using U1 = std::aligned_storage_t<sizeof(T), alignof(T)>;
#if defined(_DEBUG)
    union U {
        T  real_;
        U1 virtual_;
        ~U() {/*delete destruct ...*/
        }
        U() {/*delete construct ... */
        }
        U(const U & a) :virtual_(a.virtual_) {/*this should never call ... */
        }
        U(U && a) :virtual_(std::move(a.virtual_)) {/*this should never call ... */
        }
        U & operator=(const U & a) {/*this should never call ...*/
            if (this == &a) {
                return *this;
            }
            virtual_ = a.virtual_;
        }
        U & operator=(U && a) {/*this should never call ...*/
            if (this == &a) {
                return *this;
            }
            virtual_ = std::move(a.virtual_);
        }
    };
#else
    using U = U1;
#endif
    using V = std::vector < U, A< U > >;
    V mmm_Data;
    inline const V * _0_get_this_value() const {
        return &mmm_Data;
    }
    inline V * _0_get_this_value() {
        return &mmm_Data;
    }
public:
    Array(const Array &) = delete;
    Array(Array &&) = default;
    Array&operator=(const Array &) = delete;
    Array&operator=(Array&&) = default;
public:
    template<typename ... R>
    Array(std::size_t N = 1, R && ... args) {
        _0_get_this_value()->reserve(N);
        assert(N > 0);
        assert((N <= _0_get_this_value()->capacity()) && "capacity should not less than N");
        (((void)push_back(std::forward<R>(args))), ...);
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

    operator bool() const {
        return size() > 0;
    }

    inline pointer data() const {
        return reinterpret_cast<T *>(const_cast<U *>((_0_get_this_value()->data())));
    }

    inline auto size() const {
        return _0_get_this_value()->size();
    }

    inline auto capacity() const {
        return _0_get_this_value()->capacity();
    }

    inline iterator begin() {
        return data();
    }

    inline iterator end() {
        return data() + _0_get_this_value()->size();
    }

    inline const_iterator cbegin() const noexcept {
        return data();
    }

    inline const_iterator cend() const noexcept {
        return begin() + _0_get_this_value()->size();
    }

    template<typename ... K>
    inline T * push_back(K && ...args) {
        assert(size() < capacity());
        auto varMemory = &(_0_get_this_value()->emplace_back());
        T * varAns{ nullptr };
        try {
            if constexpr (std::is_constructible_v<T, K&&...>) {
                varAns = ::new (varMemory) T(std::forward<K>(args)...);
            } else {
                varAns = ::new (varMemory) T{ std::forward<K>(args)... };
            }
        } catch (...) {
            _0_get_this_value()->pop_back();
            throw;
        }
        return varAns;
    }

    inline ~Array() {
        if constexpr (false == std::is_trivially_destructible_v<T>) {
            if (false == _0_get_this_value()->empty()) {
                std::destroy(begin(), end());
            }
        }
    }

    reference operator[](std::size_t N) {
        return data()[N];
    }

    const_reference operator[](std::size_t N) const {
        return data()[N];
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

