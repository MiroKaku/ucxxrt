#pragma once
#include <xmemory>
#include "knew.h"


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

#ifndef _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS
#define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS
#endif


_EXPORT_STD template <class _Ty, pool_t _Pool = PagedPool, unsigned long _Tag = 'trcu'>
class kallocator {
public:
    static_assert(!is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
        "because kallocator<const T> is ill-formed.");
    static_assert(!is_function_v<_Ty>, "The C++ Standard forbids allocators for function elements "
        "because of [kallocator.requirements].");
    static_assert(!is_reference_v<_Ty>, "The C++ Standard forbids allocators for reference elements "
        "because of [kallocator.requirements].");

    using _From_primary = kallocator;

    using value_type = _Ty;

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    using pointer _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = _Ty*;
    using const_pointer _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = const _Ty*;

    using reference _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = _Ty&;
    using const_reference _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = const _Ty&;
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using propagate_on_container_move_assignment = true_type;
    using is_always_equal _CXX20_DEPRECATE_IS_ALWAYS_EQUAL = true_type;

    template <class _Other>
    struct _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS rebind {
        using other = kallocator<_Other>;
    };

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD _Ty* address(_Ty& _Val) const noexcept
    {
        return _STD addressof(_Val);
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD const _Ty* address(const _Ty& _Val) const noexcept
    {
        return _STD addressof(_Val);
    }
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

    constexpr kallocator() noexcept
    {}

    constexpr kallocator(const kallocator&) noexcept = default;

    template <class _Other>
    constexpr kallocator(const kallocator<_Other>&) noexcept
    {}

    _CONSTEXPR20 ~kallocator() = default;
    _CONSTEXPR20 kallocator& operator=(const kallocator&) = default;

    _CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count)
    {
        _STL_ASSERT(_Ptr != nullptr || _Count == 0, "null pointer cannot point to a block of non-zero size");
        // no overflow check on the following multiply; we assume _Allocate did that check
        // _Deallocate<_New_alignof<_Ty>>(_Ptr, sizeof(_Ty) * _Count);

        UNREFERENCED_PARAMETER(_Count);
        ::operator delete(_Ptr, _Pool, _Tag);
    }

    _NODISCARD_RAW_PTR_ALLOC _CONSTEXPR20 __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t _Count)
    {
        static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
        // return static_cast<_Ty*>(_Allocate<_New_alignof<_Ty>>(_Get_size_of_n<sizeof(_Ty)>(_Count)));

        return static_cast<_Ty*>(::operator new(_Get_size_of_n<sizeof(_Ty)>(_Count), _Pool, _Tag));
    }

#if _HAS_CXX23
    _NODISCARD_RAW_PTR_ALLOC constexpr allocation_result<_Ty*> allocate_at_least(
        _CRT_GUARDOVERFLOW const size_t _Count)
    {
        return {allocate(_Count), _Count};
    }
#endif // _HAS_CXX23

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD_RAW_PTR_ALLOC __declspec(kallocator) _Ty* allocate(
        _CRT_GUARDOVERFLOW const size_t _Count, const void*)
    {
        return allocate(_Count);
    }

    template <class _Objty, class... _Types>
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS void construct(_Objty* const _Ptr, _Types&&... _Args)
    {
        ::new (const_cast<void*>(static_cast<const volatile void*>(_Ptr))) _Objty(_STD forward<_Types>(_Args)...);
    }

    template <class _Uty>
    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS void destroy(_Uty* const _Ptr)
    {
        _Ptr->~_Uty();
    }

    _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS _NODISCARD size_t max_size() const noexcept
    {
        return static_cast<size_t>(-1) / sizeof(_Ty);
    }
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

    static constexpr size_t _Minimum_asan_allocation_alignment = _Asan_granularity;
};

#if _HAS_DEPRECATED_ALLOCATOR_VOID || _HAS_DEPRECATED_ALLOCATOR_MEMBERS
template <>
class kallocator<void> {
public:
    using value_type = void;

    template <class _Other>
    struct _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS rebind {
        using other = kallocator<_Other>;
    };

#if _HAS_DEPRECATED_ALLOCATOR_MEMBERS
    using pointer _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = void*;
    using const_pointer _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS = const void*;
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS

#if _HAS_CXX20
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using propagate_on_container_move_assignment = true_type;
    using is_always_equal _CXX20_DEPRECATE_IS_ALWAYS_EQUAL = true_type;
#endif // _HAS_CXX20
};
#endif // _HAS_DEPRECATED_ALLOCATOR_VOID || _HAS_DEPRECATED_ALLOCATOR_MEMBERS

_EXPORT_STD template <class _Ty, class _Other>
_NODISCARD _CONSTEXPR20 bool operator==(const kallocator<_Ty>&, const kallocator<_Other>&) noexcept
{
    return true;
}

#if !_HAS_CXX20
template <class _Ty, class _Other>
_NODISCARD bool operator!=(const kallocator<_Ty>&, const kallocator<_Other>&) noexcept
{
    return false;
}
#endif // !_HAS_CXX20


_STD_END
