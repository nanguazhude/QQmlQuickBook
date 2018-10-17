#pragma once

#if defined(QT_CORE_LIB)
#include <QtCore/qobject.h>
#endif

namespace sstd {
    namespace $p$Private {
        template<typename F, typename T, typename = void>
        class IsStaticCast : public std::false_type {};
        template<typename F, typename T>
        class IsStaticCast<F, T, std::void_t<
            decltype(static_cast<T>(std::declval<F>()))
        > > : public std::true_type {};
    }/*$p$Private*/
}/*namespace sstd*/

template<typename T, bool F = false/*force dynamic_cast?*/, typename U>
inline T sstd_dynamic_cast(U * arg) {
    if (arg == nullptr) { return nullptr; }
    using R = std::remove_cv_t< std::remove_reference_t<U> >;
    static_assert(std::is_polymorphic<R>::value, "arg is not a polymorphic class");
    if constexpr (std::is_convertible_v<R*, T>) {/*up cast ?*/
        return static_cast<T>((R*)(arg));
    }
    else if constexpr ((F == false) &&
        (sstd::$p$Private::IsStaticCast<R*, T>::value)) {/*down cast ?*/
        /*in most time this function is work ,but sometime the function run error!*/
        assert(dynamic_cast<T>((R*)(arg)) == static_cast<T>((R*)(arg)));
        return static_cast<T>((R*)(arg));
    }
#if defined(QT_CORE_LIB)
    else if constexpr (std::is_base_of_v<QObject, R>) {/*qobject cast?*/
        return qobject_cast<T>((R*)(arg));
    }
#endif
    else {/*dynamic cast*/
        return dynamic_cast<T>((R*)(arg));
    }
}

template<typename T, typename U>
inline T sstd_unsafe_cast(U * arg) {
    using R = std::remove_cv_t< std::remove_reference_t<U> >;
    if (std::is_polymorphic<R>::value) {
        return sstd_dynamic_cast<T, false, U>(arg);
    }
    else {
        return static_cast<T>((R*)(arg));
    }
}

template<typename T, typename U>
inline T sstd_safe_cast(U * arg) {
    using R = std::remove_cv_t< std::remove_reference_t<U> >;
    if (std::is_polymorphic<R>::value) {
        return sstd_dynamic_cast<T, true, U>(arg);
    }
    else {
        return static_cast<T>((R*)(arg));
    }
}













