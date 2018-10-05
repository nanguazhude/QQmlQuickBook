#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace sstd {

    template<typename T>
    using get_type = typename T::type;

#define FINAL_CLASS_TYPE(_TypeName_,...) \
    class _TypeName_ final : public __VA_ARGS__ { \
    public: \
    using _Super_ = __VA_ARGS__; \
    using _Super_::_Super_; \
    inline _TypeName_(const _TypeName_ &) = default; \
    inline _TypeName_(_TypeName_ &&) = default; \
    inline _TypeName_()=default; \
} \
    /*FINAL_CLASS_TYPE*/

    template<typename>class NumberWrapType;
    template<typename A, typename B, typename >
    inline constexpr bool operator<(const A &, const B &);
    template<typename A, typename B, typename >
    inline constexpr bool operator<=(const A &, const B &);
    template<typename A, typename B, typename >
    inline constexpr bool operator>(const A &, const B &);
    template<typename A, typename B, typename >
    inline constexpr bool operator>=(const A &, const B &);
    template<typename A, typename B, typename >
    inline constexpr bool operator==(const A &, const B &);
    template<typename A, typename B, typename >
    inline constexpr bool operator!=(const A &, const B &);

    namespace private_wrap_helper {
        template <typename T>
        using rmcvr = std::remove_cv_t< std::remove_reference_t<T> >;
        template<typename T>
        class _is_static_wrap_type :public std::false_type {};
        template<typename T>
        class _is_static_wrap_type< NumberWrapType<T> > :public std::true_type {
        public:
            using static_type = typename NumberWrapType<T>::type;
        };
        template<typename T>
        class is_static_wrap_type : public _is_static_wrap_type<rmcvr<T>> {};
    }/*namespace private_wrap_helper*/

    template<typename Tt>
    class NumberWrapType {
        static_assert (false == std::is_reference_v<Tt>, "Tt can not be reference");
        static_assert (std::is_arithmetic_v<Tt>, "Tt must be number");
        template <typename T>
        using rmcvr = private_wrap_helper::rmcvr<T>;
    public:
        using type = Tt;
    public:
        inline constexpr NumberWrapType() :type{ /*init to zero*/ } {}
        inline constexpr NumberWrapType(const NumberWrapType&) = default;
        inline constexpr NumberWrapType(NumberWrapType&&) = default;
        inline constexpr NumberWrapType&operator=(const NumberWrapType&) = default;
        inline constexpr NumberWrapType&operator=(NumberWrapType&&) = default;
        inline ~NumberWrapType() = default;
    public:
        /*init by a number*/
        template<typename U,
            typename = std::enable_if_t<std::is_arithmetic_v<rmcvr<U>>>>
            inline constexpr NumberWrapType(U && arg) :mmm_value(static_cast<type>(std::forward<U>(arg))) {}
        /*init by value*/
        template<typename U,
            typename = std::enable_if_t<false == std::is_same_v<rmcvr<U>, NumberWrapType>>,
            typename = std::enable_if_t<false == std::is_arithmetic_v<rmcvr<U>>>>
            inline constexpr NumberWrapType(U && arg) : mmm_value(static_cast<type>(std::forward<U>(arg))) {}
    public:
        /*copy from a number*/
        template<typename U,
            typename = std::enable_if_t<std::is_arithmetic_v<rmcvr<U>>>>
            inline constexpr NumberWrapType & operator=(U && arg) {
            mmm_value = static_cast<type>(std::forward<U>(arg));
            return *this;
        }
        /*copy from a value*/
        template<typename U,
            typename = std::enable_if_t<false == std::is_same_v<rmcvr<U>, NumberWrapType>>,
            typename = std::enable_if_t<false == std::is_arithmetic_v<rmcvr<U>>>>
            inline constexpr NumberWrapType & operator=(U && arg) {
            mmm_value = static_cast<type>(std::forward<U>(arg));
            return *this;
        }
    public:
        inline constexpr operator const type & () const { return mmm_value; }
        inline constexpr operator type & () { return mmm_value; }
    public:
        explicit inline constexpr  operator bool() const { return mmm_value; }
    public:
        inline constexpr NumberWrapType & operator++() { ++mmm_value; return *this; }
        inline constexpr NumberWrapType operator++(int) {
            NumberWrapType varAns{ mmm_value++ };
            return varAns;
        }
    private:
        type mmm_value;
    private:
        template<typename A, typename B, typename >
        inline friend constexpr bool operator<(const A &, const B &);
        template<typename A, typename B, typename >
        inline friend constexpr bool operator<=(const A &, const B &);
        template<typename A, typename B, typename >
        inline friend constexpr bool operator>(const A &, const B &);
        template<typename A, typename B, typename >
        inline friend constexpr bool operator>=(const A &, const B &);
        template<typename A, typename B, typename >
        inline friend constexpr bool operator==(const A &, const B &);
        template<typename A, typename B, typename >
        inline friend constexpr bool operator!=(const A &, const B &);
    };

#define _1_WRAP_TYLE_LESS(_OP_)                                                         \
    template<typename LeftType,typename RightType,                                      \
    typename =std::enable_if_t<                                                         \
    private_wrap_helper::is_static_wrap_type<LeftType>::value||                         \
    private_wrap_helper::is_static_wrap_type<RightType>::value>>                        \
    inline constexpr bool operator _OP_ (const LeftType & l,const RightType & r){       \
    using namespace private_wrap_helper;                                                \
    constexpr const static bool varIsLeftWrap= is_static_wrap_type<LeftType>::value;    \
    constexpr const static bool varIsRightWrap= is_static_wrap_type<RightType>::value;  \
    if constexpr( varIsLeftWrap && varIsRightWrap ){                                    \
    return  l.mmm_value  _OP_  r.mmm_value ;                                            \
}else if constexpr(varIsLeftWrap){                                                      \
    return l.mmm_value _OP_ r;                                                          \
}else if constexpr(varIsRightWrap){                                                     \
    return l _OP_ r.mmm_value;                                                          \
}                                                                                       \
}                                                                                       \
    /**_1_WRAP_TYLE_LESS**/

    _1_WRAP_TYLE_LESS(< )_1_WRAP_TYLE_LESS(<= )_1_WRAP_TYLE_LESS(> )_1_WRAP_TYLE_LESS(>= )_1_WRAP_TYLE_LESS(== )_1_WRAP_TYLE_LESS(!= )

}/*namespace sstd*/

namespace std {

    template<typename T>
    class hash< sstd::NumberWrapType<T> > {
    public:
        inline size_t operator()(const sstd::NumberWrapType<T> & arg) const noexcept {
            return std::hash_value(static_cast<const T &>(arg));
        }
    };

}/*namespace std*/













