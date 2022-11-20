/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      kallocator.cpp
 * DATA:      2022/11/19
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once
#include <yvals.h>
#include "knew.h"
#include <type_traits>

_STD_BEGIN

#ifndef _HAS_DEPRECATED_ALLOCATOR_MEMBERS
#define _HAS_DEPRECATED_ALLOCATOR_MEMBERS 1
#endif

#ifndef _CXX20_DEPRECATE_IS_ALWAYS_EQUAL
#define _CXX20_DEPRECATE_IS_ALWAYS_EQUAL
#endif

#ifndef _NODISCARD_RAW_PTR_ALLOC
#define _NODISCARD_RAW_PTR_ALLOC _NODISCARD
#endif


template <class T>
_NODISCARD constexpr void* ___voidify_iter(T iter) noexcept {
    if constexpr (::std::is_pointer_v<T>) {
        return const_cast<void*>(static_cast<const volatile void*>(iter));
    }
    else {
        return const_cast<void*>(static_cast<const volatile void*>(::std::addressof(*iter)));
    }
}

template <typename T, POOL_TYPE _Pool = PagedPool, unsigned long _Tag = 'trcu'>
class kallocator {
public:
    static_assert(!::std::is_const_v<T>, "The C++ Standard forbids containers of const elements "
        "because kallocator<const T> is ill-formed.");

    using value_type        = T;
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using propagate_on_container_move_assignment = true_type;

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    using pointer           _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS  = T*;
    using const_pointer     _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS  = const T*;
    using reference         _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS  = T&;
    using const_reference   _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS  = const T&;
    using is_always_equal   _CXX20_DEPRECATE_IS_ALWAYS_EQUAL        = true_type;

    template <typename N>
    struct _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS rebind {
        using other = kallocator<N, _Pool, _Tag>;
    };

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD T* address(T& value) const noexcept {
        return ::std::addressof(value);
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD const T* address(const T& value) const noexcept {
        return ::std::addressof(value);
    }
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

    constexpr kallocator() noexcept {}
    constexpr kallocator(const kallocator&) noexcept = default;

    template <typename N>
    constexpr kallocator(const kallocator<N, _Pool, _Tag>&) noexcept {}
    _CONSTEXPR20 ~kallocator() = default;
    _CONSTEXPR20 kallocator& operator=(const kallocator&) = default;

    _CONSTEXPR20 void deallocate(T* const ptr, const size_t count) {
        _STL_ASSERT(ptr != nullptr || count == 0, "null pointer cannot point to a block of non-zero size");
        __noop(count); // ignore

        // no overflow check on the following multiply; we assume allocate did that check
        ::operator delete(ptr, _Pool, _Tag);
    }

    _NODISCARD_RAW_PTR_ALLOC _CONSTEXPR20 __declspec(allocator) T* allocate(const size_t count) {
        static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
        return static_cast<T*>(::operator new(sizeof(T) * count, _Pool, _Tag));
    }

#if _HAS_CXX23
    _NODISCARD_RAW_PTR_ALLOC constexpr allocation_result<T*> allocate_at_least(
        _CRT_GUARDOVERFLOW const size_t count) {
        return { allocate(count), count };
    }
#endif // _HAS_CXX23

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD_RAW_PTR_ALLOC __declspec(allocator) T* allocate(
        _CRT_GUARDOVERFLOW const size_t count, const void*) {
        return allocate(count);
    }

    template <typename N, typename... Ns>
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS void construct(N* const ptr, Ns&&... args) {
        ::new (___voidify_iter(ptr)) N(::std::forward<Ns>(args)...);
    }

    template <typename N>
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS void destroy(N* const object) {
        object->~N();
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD size_t max_size() const noexcept {
        return static_cast<size_t>(-1) / sizeof(T);
    }
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS
};


#if _HAS_DEPRECATED_ALLOCATOR_VOID || _HAS_DEPRECATED_ALLOCATOR_MEMBERS
template <>
class kallocator<void> {
public:
    using value_type = void;

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    using pointer       _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = void*;
    using const_pointer _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = const void*;

    template <class N>
    struct _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS rebind {
        using other = kallocator<N>;
    };
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

#if _HAS_CXX20
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;

    using propagate_on_container_move_assignment            = true_type;
    using is_always_equal _CXX20_DEPRECATE_IS_ALWAYS_EQUAL  = true_type;
#endif // _HAS_CXX20
};
#endif // _HAS_DEPRECATED_ALLOCATOR_VOID || _HAS_DEPRECATED_ALLOCATOR_MEMBERS


_STD_END
