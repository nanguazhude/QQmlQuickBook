#pragma once

#if defined(_MSC_VER)
#pragma warning (disable: 4291)
#endif

#include "sstd_core_library/global_sstd_core_library.hpp"

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <charconv>

#include <array>
#include <cmath>
#include <memory>
#include <limits>
#include <utility>
#include <type_traits>

#if __has_include(<memory_resource>)
#include <memory_resource>
#else
#define DO_NOT_HAVE_MEMORY_RESOURCE
#endif

#include <string>
#include <chrono>
#include <charconv>
#include <filesystem>
#include <string_view>
using namespace std::chrono_literals;
using namespace std::string_view_literals;

#include <map>
#include <set>
#include <list>
#include <vector>
#include <random>
#include <iterator>
#include <algorithm>
#include <forward_list>

namespace sstd {
    class _1_SSTD_CORE_EXPORT SSTDMemory {
    public:
        static constexpr inline void* operator new  (std::size_t, void* ptr) noexcept {
            return ptr;
        }
        static constexpr inline void* operator new[](std::size_t, void* ptr) noexcept {
            return ptr;
        }
            static constexpr inline void operator delete  (void *, void *) noexcept {
        }
        static constexpr inline void operator delete[](void *, void *) noexcept {
        }
    public:
        static void* operator new (std::size_t count);
        static void operator delete(void * ptr);
    public:
        static void* operator new[](std::size_t count);
        static void operator delete[](void * ptr);
    public:
        static void* operator new(std::size_t count, std::align_val_t al);
        static void operator delete(void* ptr, std::align_val_t al);
    public:
        static void* operator new[](std::size_t count, std::align_val_t al);
        static void operator delete[](void* ptr, std::align_val_t al);
    };
}/*namespace sstd*/

#ifndef ____SSTD_MEMORY_DEFINE
#define ____SSTD_MEMORY_DEFINE($M$) public : static constexpr inline std::size_t ___$p$class_size() noexcept {/*check __VA_ARGS__ : typename is right*/ return sizeof($M$); } \
static inline constexpr void* operator new  (std::size_t count, void* ptr) { return sstd::SSTDMemory::operator new(count,ptr); } \
static inline constexpr void* operator new[](std::size_t count, void* ptr) { return sstd::SSTDMemory::operator new[](count,ptr); } \
static constexpr inline void operator delete  (void * a, void * b) noexcept{ return sstd::SSTDMemory::operator delete(a,b); } \
static constexpr inline void operator delete[](void * a, void * b) noexcept { return sstd::SSTDMemory::operator delete[](a,b); } \
static inline void* operator new (std::size_t count) { return sstd::SSTDMemory::operator new(count); } \
static inline void operator delete(void * ptr) { return sstd::SSTDMemory::operator delete(ptr); } \
static inline void* operator new[](std::size_t count) { return sstd::SSTDMemory::operator new(count); } \
static inline void operator delete[](void * ptr) { return sstd::SSTDMemory::operator delete(ptr); } \
static inline void* operator new(std::size_t count, std::align_val_t al) { return sstd::SSTDMemory::operator new(count, al); } \
static inline void operator delete(void* ptr, std::align_val_t al) { return sstd::SSTDMemory::operator delete(ptr, al); } \
static inline void* operator new[](std::size_t count, std::align_val_t al) { return sstd::SSTDMemory::operator new(count, al); } \
static inline void operator delete[](void* ptr, std::align_val_t al) { return sstd::SSTDMemory::operator delete(ptr, al); }
#endif

#if defined(QT_CORE_LIB)/*defined(QT_CORE_LIB)*/

#include <QtCore/qobject.h>

#ifndef SSTD_MEMORY_QOBJECT_DEFINE
#define SSTD_MEMORY_QOBJECT_DEFINE($M$) /**/    private: static inline  void ___$p$test_is_qobject() \
                                                { static_assert( std::is_base_of_v<QObject,$M$>, \
                                                "please use SSTD_MEMORY_DEFINE" ); }/**/ \
                                                ____SSTD_MEMORY_DEFINE($M$)
#endif

#ifndef SSTD_MEMORY_DEFIN
#define SSTD_MEMORY_DEFINE($M$) /**/            private : static inline void ___$p$test_is_qobject() \
                                                { static_assert( !std::is_base_of_v<QObject,$M$>, \
                                                "please use SSTD_MEMORY_QOBJECT_DEFINE" ); }/**/ \
                                                ____SSTD_MEMORY_DEFINE($M$)
#endif

#else/*defined(QT_CORE_LIB)*/

#ifndef SSTD_MEMORY_DEFINE
#define SSTD_MEMORY_DEFINE($M$) ____SSTD_MEMORY_DEFINE($M$)
#endif

#endif/*defined(QT_CORE_LIB)*/

namespace sstd {

    namespace _private {

        template<typename T, typename = void>
        class HasOperatorNew_0 : public std::false_type {
        };

        template<typename T>
        class HasOperatorNew_0<T, std::void_t<decltype(
            std::declval<T>().operator new(1))>/**/> : public std::true_type {
        };

        template<typename $T$, bool = (true == std::is_class_v<$T$>) &&
            (false == HasOperatorNew_0<std::remove_cv_t<$T$>/**/>::value) &&
            (false == std::is_final_v<$T$>) &&
            (true == std::has_virtual_destructor_v<$T$>)>
            class TypeSelect;

        template<typename $T$>
        class TypeSelect<$T$, true> {
        public:
            class alignas($T$) $T$Destory final : public $T${
            public:

                template<typename A0_,typename = std::enable_if_t< std::is_constructible_v<$T$,A0_&&>/**/>/**/>
                $T$Destory(A0_&&a0) :$T$(std::forward<A0_>(a0)) {
}
template<typename A0_, typename = void *,typename = std::enable_if_t<false == std::is_constructible_v<$T$, A0_&&>/**/>/**/>
$T$Destory(A0_&&a0) : $T${ std::forward<A0_>(a0) } {
}

template<typename A0_,typename A1_,typename ... Args,typename = void *,typename = void **, typename = std::enable_if_t< std::is_constructible_v<$T$, A0_&& , A1_ &&,Args && ...>/**/>/**/>
$T$Destory(A0_&&a0,A1_&&a1,Args && ... args) : $T$(std::forward<A0_>(a0),std::forward<A1_>(a1),std::forward<Args>(args)...) {
}
template<typename A0_,typename A1_,typename ... Args, typename = void *, typename = void **,typename = void *** ,typename = std::enable_if_t<false == std::is_constructible_v<$T$,  A0_&&, A1_ &&, Args && ...>/**/>/**/>
$T$Destory(A0_&&a0, A1_&&a1, Args && ... args) : $T${ std::forward<A0_>(a0),std::forward<A1_>(a1),std::forward<Args>(args)... } {
}

$T$Destory() :$T${} {
}
public:
    SSTD_MEMORY_DEFINE($T$Destory)
            };
            using type = $T$Destory;
            constexpr const static bool value = true;
        };
        template<typename $T$>
        class TypeSelect<$T$, false> {
        public:
            using type = $T$;
            constexpr const static bool value = false;
        };

    }/*_private*/

    template<typename $T$, typename ... $T$Args>
    inline $T$ * sstdNew($T$Args && ... args) {
        static_assert(std::is_reference_v<$T$> == false, "can not new reference");
        static_assert(std::is_array_v <$T$> == false, "can not new array");
        using $T$ObjectSelect = _private::TypeSelect<std::remove_cv_t<$T$>/**/>;
        using $T$Object = typename $T$ObjectSelect::type;
        if constexpr ($T$ObjectSelect::value) {
            return new $T$Object(std::forward<$T$Args>(args)...);
        } else {
            constexpr const static bool $m$_is_constructible_v = std::is_constructible_v<$T$Object, $T$Args&&...>;
            if constexpr (($m$_is_constructible_v == false) || ((sizeof...(args)) == 0)) {
                return new $T$Object{ std::forward<$T$Args>(args)... };
            } else {
                return new $T$Object(std::forward<$T$Args>(args)...);
            }
        }
    }

}/*namespace sstd*/

namespace sstd {
    template<class _Ty>
    class allocator {	// generic allocator for objects of class _Ty
    public:
        static_assert(!std::is_const_v<_Ty>,
            "The C++ Standard forbids containers of const elements "
            "because allocator<const T> is ill-formed.");

        using value_type = _Ty;

        typedef _Ty * pointer;
        typedef const _Ty * const_pointer;

        typedef _Ty& reference;
        typedef const _Ty& const_reference;

        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        template<class _Other>
        struct rebind {	// convert this type to allocator<_Other>
            using other = allocator<_Other>;
        };

        _Ty * address(_Ty& _Val) const noexcept {	// return address of mutable _Val
            return (std::addressof(_Val));
        }

        const _Ty * address(const _Ty& _Val) const noexcept {	// return address of nonmutable _Val
            return (std::addressof(_Val));
        }

        constexpr allocator() noexcept {	// construct default allocator (do nothing)
        }

        constexpr allocator(const allocator&) noexcept = default;
        template<class _Other>
        constexpr allocator(const allocator<_Other>&) noexcept {	// construct from a related allocator (do nothing)
        }

        void deallocate(_Ty * const _Ptr, const size_t _Count) {	// deallocate object at _Ptr
        // no overflow check on the following multiply; we assume _Allocate did that check
            SSTDMemory::operator delete(_Ptr, static_cast<std::align_val_t>(alignof(_Ty)));
            (void)_Count;
        }

        _Ty * allocate(const size_t _Count) {	// allocate array of _Count elements
            return reinterpret_cast<_Ty *>(SSTDMemory::operator new((_Count * sizeof(_Ty)),
                static_cast<std::align_val_t>(alignof(_Ty))));
        }

        _Ty * allocate(const size_t _Count, const void *) {	// allocate array of _Count elements, ignore hint
            return (this->allocate(_Count));
        }

        template<class _Objty, class... _Types>
        void construct(_Objty * const _Ptr, _Types&&... _Args) {	// construct _Objty(_Types...) at _Ptr
            if constexpr ((std::is_constructible_v < _Objty, _Types&&... > /**/ == false) ||
                (sizeof...(_Args) == 0)) {
                ::new (const_cast<void *>(static_cast<const volatile void *>(_Ptr)))
                    _Objty{ std::forward<_Types>(_Args)... };
            } else {
                ::new (const_cast<void *>(static_cast<const volatile void *>(_Ptr)))
                    _Objty(std::forward<_Types>(_Args)...);
            }
        }

        template<class _Uty>
        void destroy(_Uty * const _Ptr) {	// destroy object at _Ptr
            _Ptr->~_Uty();
        }

        size_t max_size() const noexcept {	// estimate maximum array size
            return (static_cast<size_t>(-1) / sizeof(_Ty));
        }

        /*msvc*/
        using propagate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

    };

    template<>
    class allocator<void> {	// generic allocator for type void
    public:
        using value_type = void;
        using pointer = void *;
        using const_pointer = const void *;

        template<class _Other>
        struct rebind {	// convert this type to an allocator<_Other>
            using other = allocator<_Other>;
        };
    };

    template<class _Ty, class _Other>
    inline bool operator==(const allocator<_Ty>&,
        const allocator<_Other>&) noexcept {	// test for allocator equality
        return (true);
    }

    template<class _Ty, class _Other>
    inline bool operator!=(const allocator<_Ty>&,
        const allocator<_Other>&) noexcept {	// test for allocator inequality
        return (false);
    }

}/**/

namespace sstd {

    template<class _Ty, class... _Types>
    inline std::unique_ptr<_Ty> make_unique(_Types&&... _Args) {	// make a unique_ptr
        static_assert(false == std::is_reference_v<_Ty>, "can not new reference");
        static_assert(false == std::is_array_v<_Ty>, "can not new array");
        return (std::unique_ptr<_Ty>(sstdNew<_Ty>(std::forward<_Types>(_Args)...)));
    }

}/*namespace sstd*/

namespace sstd {

    template<typename _T, typename ... Args>
    std::shared_ptr<_T> make_shared(Args && ... args) {
        static_assert(false == std::is_reference_v<_T >, "can not new reference");
        static_assert(false == std::is_array_v<_T >, "can not new array");
        using Obj_ = std::remove_cv_t<_T>;
        using A_ = sstd::allocator<Obj_>;
        return std::allocate_shared<Obj_, A_>(A_{}, std::forward<Args>(args)...);
    }

}/*namespace sstd*/

namespace sstd {
    using string = std::basic_string<char, std::char_traits<char>, sstd::allocator<char>/**/>;
    template<typename T_>using vector = std::vector<T_, sstd::allocator<T_>/**/>;
    template<typename T_>using list = std::list<T_, sstd::allocator<T_>/**/>;
    /*typedef int is_transparent;*/
    template<typename T_>using set = std::set<T_, std::less<void>, sstd::allocator<T_>/**/>;
    template<typename K_, typename T_> using map
        = std::map<K_, T_, std::less<void>, std::pair<const K_, T_>/**/>;
    template<typename T_>using forward_list = std::forward_list<T_, sstd::allocator<T_>/**/>;
}/*namespace sstd*/

using sstd::sstdNew;

#ifndef SSTD_MEMORY_FRIND
#define SSTD_MEMORY_FRIND private:template<typename $T$T,bool $T$V> friend class sstd::_private::TypeSelect; \
template<typename $T$, typename ... $T$Args> friend $T$ * sstd::sstdNew($T$Args && ...);
#endif

/**************************************************/
#include "sstd_state_stack.hpp"
#include "sstd_dynamic_cast.hpp"
#include "time/sstd_time.hpp"
#include "NumberWrapType.hpp"

#ifndef SSTD_DELETE_COPY_ASSIGN
#define SSTD_DELETE_COPY_ASSIGN(ARG_MMM_CLASS_NAME) \
public : \
ARG_MMM_CLASS_NAME(const ARG_MMM_CLASS_NAME &)=delete ;\
ARG_MMM_CLASS_NAME(ARG_MMM_CLASS_NAME &&)=delete ;\
ARG_MMM_CLASS_NAME&operator=(const ARG_MMM_CLASS_NAME &)=delete ;\
ARG_MMM_CLASS_NAME&operator=(ARG_MMM_CLASS_NAME &&)=delete \
/*Endl SSTD_DELETE_COPY_ASSIGN*/
#endif

#ifndef SSTD_RMCVR
#define SSTD_RMCVR(...) std::remove_cv_t< std::remove_reference_t< __VA_ARGS__ > >/**/
#endif

#ifndef SSTD_DECLTYPE
#define SSTD_DECLTYPE(...) SSTD_RMCVR( decltype(__VA_ARGS__) ) /**/
#endif

#ifndef SSTD_THIS_TYPE
#define SSTD_THIS_TYPE std::remove_reference_t< decltype(*this) >/**/
#endif

namespace sstd {
    namespace unique {
        class _1_SSTD_CORE_EXPORT VirtualBasic {
        public:
            virtual ~VirtualBasic() ;
        private:
            SSTD_MEMORY_DEFINE(VirtualBasic)
        };

        template< typename T >
        class alignas(alignof(T) < alignof(int *) ? alignof(int *) : alignof(T))
            PublicTypedVirtualBasic :
            public T,
            public VirtualBasic {
            static_assert(false == std::is_reference_v < T >);
            static_assert(false == std::is_array_v < T >);
        public:
            PublicTypedVirtualBasic() : T{} {
            }
            template<typename T0, typename ... TN,
                typename = std::enable_if_t<true == std::is_constructible_v<T, T0&&, TN&&...> >
            >
                PublicTypedVirtualBasic(T0 && arg0, TN && ... argN) :
                T(std::forward<T0>(arg0), std::forward<TN>(argN)...) {
            }
            template<typename T0, typename ... TN,
                typename = int,
                typename = std::enable_if_t<false == std::is_constructible_v<T, T0&&, TN&&...> >
            >
                PublicTypedVirtualBasic(T0 && arg0, TN && ... argN) :
                T{ std::forward<T0>(arg0), std::forward<TN>(argN)... } {
            }
        private:
            ____SSTD_MEMORY_DEFINE(PublicTypedVirtualBasic)
        };

        template<typename T>
        class alignas(alignof(T) < alignof(int *) ? alignof(int *) : alignof(T))
            TypedVirtualBasic : public VirtualBasic {
            static_assert(false == std::is_reference_v < T >);
            static_assert(false == std::is_array_v < T >);
        public:
#if defined(_DEBUG)
            /*if delete T * is a error !*/
            std::array<int*, 4> _0_delete_check_{ nullptr,nullptr,nullptr,nullptr };
#endif
            T mmm_RawData;
            TypedVirtualBasic() : mmm_RawData{} {
            }
            template<typename T0, typename ... TN,
                typename = std::enable_if_t<true == std::is_constructible_v<T, T0&&, TN&&...> >
            >
                TypedVirtualBasic(T0 && arg0, TN && ... argN) :
                mmm_RawData(std::forward<T0>(arg0), std::forward<TN>(argN)...) {
            }
            template<typename T0, typename ... TN,
                typename = int,
                typename = std::enable_if_t<false == std::is_constructible_v<T, T0&&, TN&&...> >
            >
                TypedVirtualBasic(T0 && arg0, TN && ... argN) :
                mmm_RawData{ std::forward<T0>(arg0), std::forward<TN>(argN)... } {
            }

        public:
            TypedVirtualBasic(const TypedVirtualBasic &) = delete;
            TypedVirtualBasic(TypedVirtualBasic&&) = delete;
            TypedVirtualBasic&operator=(TypedVirtualBasic&&) = delete;
            TypedVirtualBasic&operator=(const TypedVirtualBasic&) = delete;
        };

        class VirtualBasicDelete {
            VirtualBasic * mmm_Data;
        public:

            constexpr inline VirtualBasicDelete(VirtualBasic * arg) noexcept :mmm_Data(arg) {
            }

            constexpr inline VirtualBasicDelete() noexcept : mmm_Data{ nullptr } {
            }

            inline void operator()(void *arg) const {
                assert(arg);
                delete mmm_Data;
                return;
                (void)arg;
            }

            constexpr inline VirtualBasic * get() const noexcept {
                return mmm_Data;
            }

        public:
            inline VirtualBasicDelete(const VirtualBasicDelete &) = default;
            inline VirtualBasicDelete(VirtualBasicDelete &&) = default;
            inline VirtualBasicDelete&operator=(const VirtualBasicDelete &) = default;
            inline VirtualBasicDelete&operator=(VirtualBasicDelete &&) = default;
        };


        template<typename T, typename = void>
        class is_public : public std::false_type {
        };

        template<typename T>
        class is_public<T, std::void_t<PublicTypedVirtualBasic<T>>> : public std::true_type {
        };

    }/*namespace unique*/

    template<typename T>
    using unique_ptr = std::unique_ptr<T, unique::VirtualBasicDelete>;

    template<typename T>
    inline constexpr bool is_unique_release() {
        using U = std::remove_cv_t<T>;
        return std::has_virtual_destructor_v<U>&&unique::is_public<U>::value;
    }

    template<typename T>
    inline auto * release(T && arg) {
        using U = std::remove_reference_t<T>;
        using E = typename U::element_type;
        using D = typename U::deleter_type;
        static_assert(true == std::has_virtual_destructor_v<E>,
            "you should not release data,it was not safe!");
        static_assert((false == std::is_same_v<D, unique::VirtualBasicDelete>) || (true == is_unique_release<E>()),
            "you should not release data,it was not safe!");
        return std::forward<T>(arg).release();
    }

    /*design to not release data ...*/
    template<typename T, typename ... Args>
    unique_ptr<T> create_unique(Args &&...args) {
        using U = std::remove_cv_t<T>;
        using D = unique::VirtualBasicDelete;
        if constexpr (is_unique_release<T>()) {
            auto varWrapedPointer =
                new unique::PublicTypedVirtualBasic<U>(std::forward<Args>(args)...);
            return unique_ptr<T>{ varWrapedPointer, D(varWrapedPointer) };
        } else {
            auto varWrapedPointer =
                new unique::TypedVirtualBasic<U>(std::forward<Args>(args)...);
            return unique_ptr<T>{
                &(varWrapedPointer->mmm_RawData), D(varWrapedPointer) };
        }
    }

}/*namespace sstd*/

namespace sstd {

    template<typename T, typename D>
    inline std::shared_ptr<T> unique_to_shared(std::unique_ptr<T, D> &&arg) noexcept {
        if (false == bool(arg)) {
            return{};
        }
        auto varAns = std::shared_ptr<T>{ arg.get(),
            std::move(std::move(arg).get_deleter()),
            sstd::allocator<T>{} };
        arg.release();
        return std::move(varAns);
    }

}/*namespace sstd*/

#include <sstd_array.hpp>
#include <sstd_array_view.hpp>

