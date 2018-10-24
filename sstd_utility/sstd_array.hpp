#pragma once

namespace sstd{

template<typename T >
class Array {
    template<typename T> using A = std::allocator<T>;
    using U1 = std::aligned_storage_t<sizeof(T), alignof(T)>;
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
    using V = std::vector < U, A< U > >;
    std::shared_ptr< V > mmm_Data;
public:
    Array(const Array &) = default;
    Array(Array &&) = default;
    Array&operator=(const Array &) = default;
    Array&operator=(Array&&) = default;
public:
    Array(std::size_t N = 1) {
        mmm_Data = std::allocate_shared< V >(A<V>{});
        mmm_Data->reserve(N);
        assert(N > 0);
        assert((N <= mmm_Data->capacity()) && "capacity should not less than N");
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
        return reinterpret_cast<T *>(const_cast<U *>((mmm_Data->data())));
    }

    inline auto size() const {
        return mmm_Data->size();
    }

    inline auto capacity() const {
        return mmm_Data->capacity();
    }

    inline iterator begin() {
        return data();
    }

    inline iterator end() {
        return data() + mmm_Data->size();
    }

    inline const_iterator cbegin() const noexcept {
        return data();
    }

    inline const_iterator cend() const noexcept {
        return begin() + mmm_Data->size();
    }

    template<typename ... K>
    inline T * push_back(K && ...args) {
        assert(size() < capacity());
        auto varMemory = &(mmm_Data->emplace_back());
        T * varAns{ nullptr };
        try {
            if constexpr (std::is_constructible_v<T, K&&...>) {
                varAns = ::new (varMemory) T(std::forward<K>(args)...);
            } else {
                varAns = ::new (varMemory) T{ std::forward<K>(args)... };
            }
        } catch (...) {
            mmm_Data->pop_back();
            throw;
        }
        return varAns;
    }

    inline ~Array() {
        if constexpr (false == std::is_trivially_destructible_v<T>) {
            if (false == mmm_Data->empty()) {
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

