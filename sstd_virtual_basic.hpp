#pragma once

#include <map>
#include <mutex>
#include <atomic>
#include <string>
#include <utility>
#include <cstddef>
#include <cstdint>
#include <typeinfo>
#include <typeindex>
#include <type_traits>
#include <string_view>
#include <forward_list>

namespace _01_00_private_sstd_virtual_basic {
    using string = std::string;
    template<typename T>
    using forward_list = std::forward_list<T>;
    template<typename K,typename T>
    using map = std::map<K,T>;
    template<typename T, typename U>
    class alignas((alignof(T) > alignof(void *)) ? alignof(T) : alignof(void *)) object_wrap final : public U{
        T mmm_data;
        static_assert(false == std::is_reference_v<T>);
        static_assert(false == std::is_array_v<T>);
    public:
        inline void * get_data() const override {
            return get_type_data();
        }
        inline T * get_type_data() const {
            return const_cast<T *>(&mmm_data);
        }
        template<typename A0, typename ... A,
            typename = std::enable_if_t<
            true == std::is_constructible_v<T, A0&&, A&&...>> >
            inline object_wrap(A0&&a0, A && ... args) : mmm_data(std::forward<A0>(a0),
                std::forward<A>(args)...) {
        }
        template<typename A0, typename ... A,
            typename = decltype(nullptr),
            typename = std::enable_if_t<
            false == std::is_constructible_v<T, A0&&, A&&...>> >
            inline object_wrap(A0&&a0, A && ... args) : mmm_data{ std::forward<A0>(a0),
                std::forward<A>(args)... } {
        }
        inline object_wrap() : mmm_data{} {
        }
    };
    class object {
    public:
        virtual void * get_data() const = 0;
        virtual ~object() noexcept;
        object(const object &) = delete;
        object(object &&) = delete;
        object&operator=(const object &) = delete;
        object&operator=(object&&) = delete;
        object() noexcept;
    };
    class named_object : public object {
        using string_type = object_wrap<_01_00_private_sstd_virtual_basic::string, object>;
        string_type * mmm_name{ nullptr };
    public:
        named_object() noexcept;
        virtual ~named_object() noexcept;
        void set_name(const std::string_view &);
        std::string_view get_name() const;
    };
}/*namespace*/

template<typename T>
class sstd_register_virtual_basic;
class sstd_virtual_basic {
protected:
    using type_index_t = std::type_index;
    using private_object_t = _01_00_private_sstd_virtual_basic::object;
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
    std::shared_ptr< std::recursive_mutex > sstd_get_class_mutex() const noexcept /*此函数线程安全*/;
private:
    using mutex_t_t = _01_00_private_sstd_virtual_basic::object_wrap<std::shared_ptr< std::recursive_mutex >, private_object_t>;
    using maped_named_objects_t = _01_00_private_sstd_virtual_basic::map< std::string_view, private_object_t * >;
    using mmm_objects_in_this_t = _01_00_private_sstd_virtual_basic::forward_list< std::unique_ptr< private_object_t > >;
    using maped_named_objects_t_t = _01_00_private_sstd_virtual_basic::object_wrap<maped_named_objects_t, private_object_t>;
    using mmm_objects_in_this_t_t = _01_00_private_sstd_virtual_basic::object_wrap<mmm_objects_in_this_t, private_object_t>;
    _01_00_private_sstd_virtual_basic::map< sstd_type_index, void * > * mmm_objects;
    maped_named_objects_t_t * mmm_named_objects{ nullptr };
    mmm_objects_in_this_t_t * mmm_objects_in_this{ nullptr };
    mutable std::atomic< mutex_t_t * > mmm_mutex{ nullptr };
public:
    virtual ~sstd_virtual_basic();
    virtual const sstd_type_index & sstd_get_type_index() const;
    inline sstd_virtual_basic * sstd_get_virtual_basic() const {
        return const_cast<sstd_virtual_basic *>(this);
    }
    sstd_virtual_basic() noexcept;
    void * sstd_find_object(const sstd_type_index & k) const;
    template<typename T, typename ... U>
    inline T * sstd_create_object_in_this_class(U && ...);
    template<typename T, typename ... U>
    inline T * sstd_create_named_object_in_this_class(std::string_view, U && ...);
    void * sstd_find_named_object(const std::string_view &) const;
protected:
    void sstd_add_object_cast(const sstd_type_index & k, void * v);
public:
    sstd_virtual_basic(const sstd_virtual_basic &) = delete;
    sstd_virtual_basic(sstd_virtual_basic&&) = delete;
    sstd_virtual_basic&operator=(const sstd_virtual_basic &) = delete;
    sstd_virtual_basic&operator=(sstd_virtual_basic&&) = delete;
private:
    template<typename T>
    friend class sstd_register_virtual_basic;
    maped_named_objects_t * get_named_objects() noexcept;
    mmm_objects_in_this_t * get_objects_list() noexcept;
    void destory_objects_in_this() noexcept;
    void insert_object(std::unique_ptr< private_object_t > &&) noexcept;
    void insert_named_object(const std::string_view &, std::unique_ptr< private_object_t > &&) noexcept;
};

template<typename T, typename ... U>
inline T * sstd_virtual_basic::sstd_create_object_in_this_class(U && ...args) {
    using sstd_this_type_1 = std::remove_cv_t< T >;
    using sstd_this_object_1 = _01_00_private_sstd_virtual_basic::object;
    using sstd_this_wrap_1 = _01_00_private_sstd_virtual_basic::object_wrap<sstd_this_type_1, sstd_this_object_1>;
    std::unique_ptr varAnsUnique = std::make_unique<sstd_this_wrap_1>(std::forward<U>(args)...);
    auto varAns = reinterpret_cast<T *>(varAnsUnique->get_data());
    this->insert_object(std::move(varAnsUnique));
    return varAns;
}

template<typename T, typename ... U>
inline T * sstd_virtual_basic::sstd_create_named_object_in_this_class(std::string_view name, U && ... args) {
    using sstd_this_type_1 = std::remove_cv_t< T >;
    using sstd_this_object_1 = _01_00_private_sstd_virtual_basic::named_object;
    using sstd_this_wrap_1 = _01_00_private_sstd_virtual_basic::object_wrap<sstd_this_type_1, sstd_this_object_1>;
    std::unique_ptr varAnsUnique = std::make_unique<sstd_this_wrap_1>(std::forward<U>(args)...);
    auto varAns = reinterpret_cast<T *>(varAnsUnique->get_data());
    varAnsUnique->set_name(name);
    name = varAnsUnique->get_name();
    this->insert_named_object(name, std::move(varAnsUnique));
    return varAns;
}

/*typeid*/
template<typename T>
inline static const sstd_virtual_basic::sstd_type_index & sstd_get_type_index() noexcept {
    return sstd_virtual_basic::get_sstd_type_index<T>();
}

/*dynamic_cast*/
template<typename T, typename U = sstd_virtual_basic * >
inline static T * sstd_find_object(const U & arg) {
    if (arg) {
        return reinterpret_cast<T *>(
            arg->sstd_find_object(::sstd_get_type_index<T>()));
    }
    return nullptr;
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
        return this->sstd_get_virtual_basic()->sstd_add_object_cast(k, v);
    }
    template<typename T111, typename ... U>
    inline T111 * sstd_create_object_in_this_class(U && ...args) {
        return this->sstd_get_virtual_basic()
            ->template sstd_create_object_in_this_class<T111>(std::forward<U>(args)...);
    }
    template<typename T111, typename ... U>
    inline T111 * sstd_create_named_object_in_this_class(const std::string_view & name, U && ...args) {
        return this->sstd_get_virtual_basic()
            ->template sstd_create_named_object_in_this_class<T111>(name, std::forward<U>(args)...);
    }
    inline void * sstd_find_named_object(const std::string_view & name) const {
        return this->sstd_get_virtual_basic()->sstd_find_named_object(name);
    }
    std::shared_ptr< std::recursive_mutex > sstd_get_class_mutex() const noexcept/*线程安全*/ {
        return this->sstd_get_virtual_basic()
            ->sstd_get_class_mutex();
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
} \
public: \
inline void * sstd_find_named_object(const std::string_view & name) const { \
using sstd_this_type_ = std::remove_cv_t< \
    std::remove_reference_t< decltype(*this) > >; \
return sstd_register_virtual_basic<sstd_this_type_>::sstd_find_named_object(name); \
} \
template<typename T111, typename ... U> \
inline T111 * sstd_create_named_object_in_this_class(const std::string_view & name, U && ...args) { \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>:: \
    template sstd_create_named_object_in_this_class<T111>(name, std::forward<U>(args)...); \
} \
template<typename T111, typename ... U> \
inline T111 * sstd_create_object_in_this_class(U && ...args) { \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>:: \
    template sstd_create_object_in_this_class<T111>(std::forward<U>(args)...); \
} \
std::shared_ptr< std::recursive_mutex > sstd_get_class_mutex() const noexcept{ \
    using sstd_this_type_ = std::remove_cv_t< \
        std::remove_reference_t< decltype(*this) > >; \
    return sstd_register_virtual_basic<sstd_this_type_>:: \
        sstd_get_class_mutex(); \
}
/***************************************************************/
#endif
