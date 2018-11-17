#pragma once

#include <map>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

template<typename T>
class sstd_register_virtual_basic;
class sstd_virtual_basic {
protected:
    using type_index_t = std::type_index;
public:
    using sstd_type_index = std::pair<std::size_t, type_index_t>;
    template<typename T>
    inline static const sstd_type_index & get_sstd_type_index() noexcept {
        using sstd_this_type_ = std::remove_cv_t<
            std::remove_reference_t<T> >;
        const static sstd_type_index varAns = []() {
            type_index_t varIndex_{ typeid(sstd_this_type_) };
            std::hash< type_index_t > varHash_;
            return sstd_type_index{ varHash_(varIndex_),varIndex_ };
        }();
        return varAns;
    }
private:
    std::map< sstd_type_index, void * > mmm_objects;
public:
    virtual ~sstd_virtual_basic();
    virtual const sstd_type_index & sstd_get_type_index() const;
    inline sstd_virtual_basic * sstd_get_virtual_basic() const {
        return const_cast<sstd_virtual_basic *>(this);
    }
    inline sstd_virtual_basic() {
        sstd_add_object_cast(this->sstd_get_type_index(), this);
    }
    void * sstd_find_object(const sstd_type_index & k) const;
protected:
    void sstd_add_object_cast(const sstd_type_index & k, void * v);
public:
    sstd_virtual_basic(const sstd_virtual_basic &) = default;
    sstd_virtual_basic(sstd_virtual_basic&&) = default;
    sstd_virtual_basic&operator=(const sstd_virtual_basic &) = default;
    sstd_virtual_basic&operator=(sstd_virtual_basic&&) = default;
private:
    template<typename T>
    friend class sstd_register_virtual_basic;
};

/*typeid*/
template<typename T>
inline static const sstd_virtual_basic::sstd_type_index & get_sstd_type_index() noexcept {
    return sstd_virtual_basic::get_sstd_type_index<T>();
}

/*dynamic_cast*/
template<typename T,typename U = sstd_virtual_basic*>
inline static T * sstd_find_object(const U & arg) {
    if (arg) {
        return reinterpret_cast<T *>(
            arg->sstd_find_object(::get_sstd_type_index<T>()));
    }
    return nullptr;
}

const sstd_virtual_basic::sstd_type_index & sstd_virtual_basic::sstd_get_type_index() const {
    using sstd_this_type_ = std::remove_cv_t<
        std::remove_reference_t< decltype(*this) > >;
    return ::get_sstd_type_index<sstd_this_type_>();
}

sstd_virtual_basic::~sstd_virtual_basic() {
}

void sstd_virtual_basic::sstd_add_object_cast(const sstd_type_index & k, void * v) {
    mmm_objects.emplace(k, v);
}

void * sstd_virtual_basic::sstd_find_object(const sstd_type_index & k) const {
    auto varPos = (std::as_const(mmm_objects).find(k));
    if (varPos == (std::as_const(mmm_objects).end())) {
        return nullptr;
    }
    return varPos->second;
}

template<typename T>
class sstd_register_virtual_basic {
    using sstd_type_index = sstd_virtual_basic::sstd_type_index;
protected:
    inline static const sstd_type_index & sstd_get_type_index() {
        using sstd_this_type_ = std::remove_cv_t< std::remove_reference_t< T > >;
        return sstd_virtual_basic::get_sstd_type_index<sstd_this_type_>();
    }
    inline sstd_register_virtual_basic() {
        using sstd_this_type_ = std::remove_cv_t< std::remove_reference_t< T > >;
        this->sstd_add_object_cast(this->sstd_get_type_index(), static_cast<sstd_this_type_ *>(this));
    }
    inline sstd_virtual_basic * sstd_get_virtual_basic() const {
        using sstd_this_type_ = std::remove_cv_t< std::remove_reference_t< T > >;
        return static_cast<const sstd_virtual_basic *>( 
            static_cast<const sstd_this_type_ *>(this))->sstd_get_virtual_basic();
    }
    inline void * sstd_find_object(const sstd_type_index & k) const {
        return this->sstd_get_virtual_basic()->sstd_find_object(k);
    }
    inline void sstd_add_object_cast(const sstd_type_index & k, void * v) {
        return this->sstd_get_virtual_basic()->sstd_add_object_cast(k,v);
    }
};

#ifndef SSTD_REGISTER_VIRTUAL_BASIC
#define SSTD_REGISTER_VIRTUAL_BASIC(_B_) private sstd_register_virtual_basic<_B_>,\
    public virtual sstd_virtual_basic
#endif

#ifndef SSTD_VIRTUAL_CLASS
#define SSTD_VIRTUAL_CLASS public: \
using sstd_type_index = sstd_virtual_basic::sstd_type_index; \
inline const sstd_type_index & sstd_get_type_index() const override { \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>::sstd_get_type_index(); \
} \
inline void * sstd_find_object(const sstd_type_index & k) const { \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>::sstd_find_object(k); \
} \
inline sstd_virtual_basic * sstd_get_virtual_basic() const { \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>::sstd_get_virtual_basic(); \
} \
protected: \
inline void sstd_add_object_cast(const sstd_type_index & k, void * v) { \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>::sstd_add_object_cast(k, v); \
}
/***************************************************************/
#endif
