/***
*align.c - Aligned allocation, reallocation or freeing of memory in the heap
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the _aligned_malloc(),
*                   _aligned_realloc(),
*                   _aligned_recalloc(),
*                   _aligned_offset_malloc(),
*                   _aligned_offset_realloc(),
*                   _aligned_offset_recalloc(),
*                   _aligned_free(),
*                   _aligned_msize() functions.
*
*******************************************************************************/

#include <corecrt_internal.h>
#include <malloc.h>



#define IS_2_POW_N(X)   ((X) != 0 && ((X) & ((X) - 1)) == 0)
#define PTR_SZ          sizeof(void *)

/***
*
* |1|___6___|2|3|4|_________5__________|_6_|
*
* 1 -> Pointer to start of the block allocated by malloc.
* 2 -> Value of 1.
* 3 -> Gap used to get 1 aligned on sizeof(void *).
* 4 -> Pointer to the start of data block.
* 4+5 -> Data block.
* 6 -> Wasted memory at rear of data block.
* 6 -> Wasted memory.
*
*******************************************************************************/

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_offset_malloc(
    size_t const size,
    size_t const alignment,
    size_t const offset
);

// These functions are patchable and therefore must be marked noinline.
// Their *_base implementations are marked __forceinline in order to
// ensure each export is separated from each other and that there is
// enough space in each export to host a patch.

extern "C" __declspec(noinline) void __cdecl _aligned_free(void* const block)
{
    uintptr_t ptr;

    if (block == nullptr)
        return;

    ptr = (uintptr_t)block;

    /* ptr points to the pointer to starting of the memory block */
    ptr = (ptr & ~(PTR_SZ - 1)) - PTR_SZ;

    /* ptr is the pointer to the start of memory block*/
    ptr = *((uintptr_t*)ptr);
    free((void*)ptr);
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_malloc(
    size_t const size,
    size_t const alignment
    )
{
    return _aligned_offset_malloc(size, alignment, 0);
}

extern "C" __declspec(noinline) size_t __cdecl _aligned_msize(
    void*  const block,
    size_t const alignment,
    size_t const offset)
{
    size_t align = alignment;
    size_t header_size = 0; /* Size of the header block */
    size_t footer_size = 0; /* Size of the footer block */
    size_t total_size  = 0; /* total size of the allocated block */
    size_t user_size   = 0; /* size of the user block*/
    uintptr_t gap      = 0; /* keep the alignment of the data block */
                             /* after the sizeof(void*) aligned pointer */
                             /* to the beginning of the allocated block */
    uintptr_t ptr      = 0; /* computes the beginning of the allocated block */

    _VALIDATE_RETURN(block != nullptr, EINVAL, static_cast<size_t>(-1));

    /* HEADER SIZE + FOOTER SIZE = GAP + ALIGN + SIZE OF A POINTER*/
    /* HEADER SIZE + USER SIZE + FOOTER SIZE = TOTAL SIZE */

    ptr = (uintptr_t)block;            /* ptr points to the start of the aligned memory block */
    ptr = (ptr & ~(PTR_SZ - 1)) - PTR_SZ; /* ptr is one position behind memblock */
                                          /* the value in ptr is the start of the real allocated block */
    ptr = *((uintptr_t *)ptr);            /* after dereference ptr points to the beginning of the allocated block */

    total_size  = _msize((void*)ptr);
    header_size = (uintptr_t) block - ptr;
    gap         = (0 - offset) & (PTR_SZ - 1);

    /* Alignment cannot be less than sizeof(void*) */
    align       = (align > PTR_SZ ? align : PTR_SZ) -1;
    footer_size = gap + align + PTR_SZ - header_size;
    user_size   = total_size - header_size - footer_size;

    return user_size;
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_offset_malloc(
    size_t const size,
    size_t const alignment,
    size_t const offset
    )
{
    uintptr_t ptr, retptr, gap;
    size_t nonuser_size,block_size;
    size_t align = alignment;

    /* validation section */
    _VALIDATE_RETURN(IS_2_POW_N(align), EINVAL, nullptr);
    _VALIDATE_RETURN(offset == 0 || offset < size, EINVAL, nullptr);

    align = (align > PTR_SZ ? align : PTR_SZ) -1;

    /* gap = number of bytes needed to round up offset to align with PTR_SZ*/
    gap = (0 - offset)&(PTR_SZ -1);

    nonuser_size = PTR_SZ +gap +align;
    block_size = nonuser_size + size;
    _VALIDATE_RETURN_NOEXC(size <= block_size, ENOMEM, nullptr)

    if ( (ptr =(uintptr_t)malloc(block_size)) == (uintptr_t)nullptr)
        return nullptr;

    retptr =((ptr +nonuser_size+offset)&~align)- offset;
    ((uintptr_t *)(retptr - gap))[-1] = ptr;

    return (void *)retptr;
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_offset_realloc(
    void*  const block,
    size_t const size,
    size_t const alignment,
    size_t const offset
    )
{
    uintptr_t ptr, retptr, gap, stptr, diff;
    uintptr_t movsz, reqsz;
    size_t align = alignment;
    int bFree = 0;

    /* special cases */
    if (block == nullptr)
    {
        return _aligned_offset_malloc(size, align, offset);
    }
    if (size == 0)
    {
        _aligned_free(block);
        return nullptr;
    }

    /* validation section */
    _VALIDATE_RETURN(IS_2_POW_N(align), EINVAL, nullptr);
    _VALIDATE_RETURN(offset == 0 || offset < size, EINVAL, nullptr);

    stptr = (uintptr_t)block;

    /* ptr points to the pointer to starting of the memory block */
    stptr = (stptr & ~(PTR_SZ -1)) - PTR_SZ;

    /* ptr is the pointer to the start of memory block*/
    stptr = *((uintptr_t *)stptr);

    align = (align > PTR_SZ ? align : PTR_SZ) -1;
    /* gap = number of bytes needed to round up offset to align with PTR_SZ*/
    gap = (0 -offset)&(PTR_SZ -1);

    diff = (uintptr_t)block - stptr;
    /* Mov size is min of the size of data available and sizw requested.
     */
    #pragma warning(push)
    #pragma warning(disable: 22018) // Silence prefast about overflow/underflow
    movsz = _msize((void *)stptr) - ((uintptr_t)block - stptr);
    #pragma warning(pop)

    movsz = movsz > size ? size : movsz;
    reqsz = PTR_SZ + gap + align + size;

    _VALIDATE_RETURN_NOEXC(size <= reqsz, ENOMEM, nullptr);

    /* First check if we can expand(reducing or expanding using expand) data
     * safely, ie no data is lost. eg, reducing alignment and keeping size
     * same might result in loss of data at the tail of data block while
     * expanding.
     *
     * If no, use malloc to allocate the new data and move data.
     *
     * If yes, expand and then check if we need to move the data.
     */
    if ((stptr +align +PTR_SZ +gap)<(uintptr_t)block)
    {
        if ((ptr = (uintptr_t)malloc(reqsz)) == (uintptr_t) nullptr)
            return nullptr;
        bFree = 1;
    }
    else
    {
        /* we need to save errno, which can be modified by _expand */
        errno_t save_errno = errno;
        if ((ptr = (uintptr_t)_expand((void *)stptr, reqsz)) == (uintptr_t)nullptr)
        {
            errno = save_errno;
            if ((ptr = (uintptr_t)malloc(reqsz)) == (uintptr_t) nullptr)
                return nullptr;
            bFree = 1;
        }
        else
            stptr = ptr;
    }


    if ( ptr == ((uintptr_t)block - diff)
         && !( ((size_t)block + gap +offset) & ~(align) ))
    {
        return block;
    }

    retptr =((ptr +PTR_SZ +gap +align +offset)&~align)- offset;
    memmove((void *)retptr, (void *)(stptr + diff), movsz);
    if ( bFree)
        free ((void *)stptr);

    ((uintptr_t *)(retptr - gap))[-1] = ptr;
    return (void *)retptr;
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_offset_recalloc(
    void*  const block,
    size_t const count,
    size_t const size,
    size_t const alignment,
    size_t const offset
    )
{
    size_t align        = alignment;
    size_t user_size    = 0;    /* wanted size, passed to aligned realoc */
    size_t start_fill   = 0;    /* location where aligned recalloc starts to fill with 0 */
    /* filling must start from the end of the previous user block */
    void* retptr = nullptr; /* result of aligned recalloc*/

    /* ensure that (size * num) does not overflow */
    if (count > 0)
    {
        _VALIDATE_RETURN_NOEXC((_HEAP_MAXREQ / count) >= size, ENOMEM, nullptr);
    }

    user_size = size * count;

    if (block != nullptr)
    {
        start_fill = _aligned_msize(block, align, offset);
    }

    retptr = _aligned_offset_realloc(block, user_size, align, offset);

    if (retptr != nullptr)
    {
        if (start_fill < user_size)
        {
            memset((char*)retptr + start_fill, 0, user_size - start_fill);
        }
    }
    return retptr;
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_realloc(
    void*  const block,
    size_t const size,
    size_t const alignment
    )
{
    return _aligned_offset_realloc(block, size, alignment, 0);
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _aligned_recalloc(
    void*  const block,
    size_t const count,
    size_t const size,
    size_t const alignment
    )
{
    return _aligned_offset_recalloc(block, count, size, alignment, 0);
}
