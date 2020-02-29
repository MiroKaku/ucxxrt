/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      trnsctrl.cpp - Routines for doing control transfers
 * DATA:      2020/02/05
 *
 * PURPOSE:   Routines for doing control transfers
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 /***
 *trnsctrl.cpp -  Routines for doing control transfers
 *
 *       Copyright (c) Microsoft Corporation. All rights reserved.
 *
 *Purpose:
 *       Routines for doing control transfers; written using inline
 *       assembly in naked functions.  Contains the public routine
 *       _CxxFrameHandler, the entry point for the frame handler
 ****/


#include <eh.h>
#include "vcruntime/ehassert.h"
#include "vcruntime/ehdata.h"
#include "vcruntime/ehdata4.h"
#include "vcruntime/ehhooks.h"

#include "vcruntime/trnsctrl.h"
#include "vcruntime/ehhelpers.h"

#include <setjmp.h>


#pragma warning(disable: 4100)      // unreferenced formal parameter
#pragma warning(disable: 4731)      // ignore EBP mod in inline-asm warning
#pragma warning(disable: 4733)      // ignore unsafe FS:0 modifications
#pragma warning(disable: 4740)      // control flow in asm suppresses global optimization


namespace ucxxrt
{

    // copied from ntxcapi.h
#define EXCEPTION_UNWINDING 0x2         // Unwind is in progress
#define EXCEPTION_EXIT_UNWIND 0x4       // Exit unwind is in progress
#define EXCEPTION_STACK_INVALID 0x8     // Stack out of limits or unaligned
#define EXCEPTION_NESTED_CALL 0x10      // Nested exception handler call
#define EXCEPTION_TARGET_UNWIND 0x20    // Target unwind in progress
#define EXCEPTION_COLLIDED_UNWIND 0x40  // Collided exception handler call

#define EXCEPTION_UNWIND (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND | \
                          EXCEPTION_TARGET_UNWIND | EXCEPTION_COLLIDED_UNWIND)

#define IS_UNWINDING(Flag)      ((Flag & EXCEPTION_UNWIND) != 0)
#define IS_DISPATCHING(Flag)    ((Flag & EXCEPTION_UNWIND) == 0)
#define IS_TARGET_UNWIND(Flag)  (Flag & EXCEPTION_TARGET_UNWIND)

#define pFrameInfoChain   (*((FRAMEINFO **)&(RENAME_BASE_PTD(__ucxxrt_getptd)()->_pFrameInfoChain)))



    /////////////////////////////////////////////////////////////////////////////
    //
    // _JumpToContinuation - sets up EBP and jumps to specified code address.
    //
    // Does not return.
    //
    // NT leaves a marker registration node at the head of the list, under the
    // assumption that RtlUnwind will remove it.  As it happens, we need to keep
    // it in case of a rethrow (see below).  We only remove the current head
    // (assuming it is NT's), because there may be other nodes that we still
    // need.
    //

    void __stdcall _JumpToContinuation(
        void* target,           // The funclet to call
        EHRegistrationNode* pRN // Registration node, represents location of frame
    ) {
        EHTRACE_FMT1("Transfer to 0x%p", target);
        long targetEBP;

#if !CC_EXPLICITFRAME
        targetEBP = (long)pRN + FRAME_OFFSET;
#else
        targetEBP = pRN->frame;
#endif

        __asm {
            //
            // Transfer control to the continuation point
            //
            mov     eax, target         // Load target address
            mov     ebx, pRN
            mov     ebp, targetEBP      // Load target frame pointer
            mov     esp, [ebx - 4]      // Restore target esp
            jmp     eax                 // Call the funclet
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    //
    // _UnwindNestedFrames - Call RtlUnwind, passing the address after the call
    //                      as the continuation address.
    //
    //  Win32 assumes that after a frame has called RtlUnwind, it will never return
    // to the dispatcher.
    //
    // Let me explain:
    //  When the dispatcher calls a frame handler while searching
    // for the appropriate handler, it pushes an extra guard registration node
    // onto the list.  When the handler returns to the dispatcher, the dispatcher
    // assumes that its node is at the head of the list, restores esp from the
    // address of the head node, and then unlinks that node from the chain.
    //  However, if RtlUnwind removes ALL nodes below the specified node, including
    // the dispatcher's node, so without intervention the result is that the
    // current subject node gets poped from the list, and the stack pointer gets
    // reset to somewhere within the frame of that node, which is totally bogus
    // (this last side effect is not a problem, because esp is then immediately
    // restored from the ebp chain, which is still valid).
    //
    // So:
    //  To get arround this, WE ASSUME that the registration node at the head of
    // the list is the dispatcher's marker node (which it is in NT 1.0), and
    // we keep a handle to it when we call RtlUnwind, and then link it back in
    // after RtlUnwind has done its stuff.  That way, the dispatcher restores
    // its stack exactly as it expected to, and leave our registration node alone.
    //
    // What happens if there is an exception during the unwind?
    // We can't put a registration node here, because it will be removed
    // immediately.
    //
    // RtlUnwind:
    //  RtlUnwind is evil.  It trashes all the registers except EBP and ESP.
    // Because of that, EBX, ESI, and EDI must be preserved by this function,
    // and the compiler may not assume that any callee-save register can be used
    // across the call to RtlUnwind.  To accomplish the former, inline-asm code
    // here uses EBX, ESI, and EDI, so they will be saved in the prologue.  For
    // the latter, optimizations are disabled for the duration of this function.
    //

    BEGIN_PRAGMA_OPTIMIZE_DISABLE("g", DevDivVSO:162582, "RtlUnwind does not preserve registers (see above)")

        void __stdcall _UnwindNestedFrames(
            EHRegistrationNode* pRN,        // Unwind up to (but not including) this frame
            EHExceptionRecord* pExcept    // The exception that initiated this unwind
        ) 
    {
        PER_FLAGS(pExcept) &= ~EXCEPTION_UNWINDING; // Clear the 'Unwinding' flag
                                                    // in case exception is rethrown
    }

    END_PRAGMA_OPTIMIZE()


        //
        // This is a backwards-compatibility entry point. All new code must go to __CxxFrameHandler2
        //
        extern "C" _VCRTIMP __declspec(naked) DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler(
            /*
                EAX=FuncInfo   *pFuncInfo,          // Static information for this frame
            */
            EHExceptionRecord * pExcept,        // Information for this exception
            EHRegistrationNode * pRN,            // Dynamic information for this frame
            void* pContext,       // Context info (we don't care what's in it)
            DispatcherContext * pDC             // More dynamic info for this frame (ignored on Intel)
        ) 
    {
        FuncInfo* pFuncInfo;
        EXCEPTION_DISPOSITION result;

        __asm {
            //
            // Standard function prolog
            //
            push    ebp
            mov     ebp, esp
            sub     esp, __LOCAL_SIZE
            push    ebx
            push    esi
            push    edi
            cld             // A bit of paranoia -- Our code-gen assumes this

            //
            // Save the extra parameter
            //
            mov     pFuncInfo, eax
        }

        EHTRACE_FMT1("pRN = 0x%p", pRN);

        result = __InternalCxxFrameHandler<RENAME_EH_EXTERN(__FrameHandler3)>(pExcept, pRN, (PCONTEXT)pContext, pDC, pFuncInfo, 0, nullptr, FALSE);

        EHTRACE_HANDLER_EXIT(result);

        __asm {
            pop     edi
            pop     esi
            pop     ebx
            mov     eax, result
            mov     esp, ebp
            pop     ebp
            ret     0
        }
    }

    //
    // __CxxFrameHandler3 - Real entry point to the runtime
    //                                              __CxxFrameHandler2 is an alias for __CxxFrameHandler3
    //                                              since they are compatible in VC version of CRT
    //                      These function should be separated out if a change makes
    //                                              __CxxFrameHandler3 incompatible with __CxxFrameHandler2
    //
    extern "C" _VCRTIMP __declspec(naked) DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler3(
        /*
            EAX=FuncInfo   *pFuncInfo,          // Static information for this frame
        */
        EHExceptionRecord * pExcept,        // Information for this exception
        EHRegistrationNode * pRN,            // Dynamic information for this frame
        void* pContext,       // Context info (we don't care what's in it)
        DispatcherContext * pDC             // More dynamic info for this frame (ignored on Intel)
    ) {
        FuncInfo* pFuncInfo;
        EXCEPTION_DISPOSITION result;

        __asm {
            //
            // Standard function prolog
            //
            push    ebp
            mov     ebp, esp
            sub     esp, __LOCAL_SIZE
            push    ebx
            push    esi
            push    edi
            cld             // A bit of paranoia -- Our code-gen assumes this

            //
            // Save the extra parameter
            //
            mov     pFuncInfo, eax
        }

        EHTRACE_FMT1("pRN = 0x%p", pRN);

        result = __InternalCxxFrameHandler<RENAME_EH_EXTERN(__FrameHandler3)>(pExcept, pRN, (PCONTEXT)pContext, pDC, pFuncInfo, 0, nullptr, FALSE);

        EHTRACE_HANDLER_EXIT(result);

        __asm {
            pop     edi
            pop     esi
            pop     ebx
            mov     eax, result
            mov     esp, ebp
            pop     ebp
            ret     0
        }
    }

    //
    // __CxxFrameHandler2 - Remove after compiler is updated
    //
    extern "C" _VCRTIMP __declspec(naked) DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler2(
        /*
            EAX=FuncInfo   *pFuncInfo,          // Static information for this frame
        */
        EHExceptionRecord * pExcept,        // Information for this exception
        EHRegistrationNode * pRN,            // Dynamic information for this frame
        void* pContext,       // Context info (we don't care what's in it)
        DispatcherContext * pDC             // More dynamic info for this frame (ignored on Intel)
    ) {
        FuncInfo* pFuncInfo;
        EXCEPTION_DISPOSITION result;

        __asm {
            //
            // Standard function prolog
            //
            push    ebp
            mov     ebp, esp
            sub     esp, __LOCAL_SIZE
            push    ebx
            push    esi
            push    edi
            cld             // A bit of paranoia -- Our code-gen assumes this

            //
            // Save the extra parameter
            //
            mov     pFuncInfo, eax
        }

        EHTRACE_FMT1("pRN = 0x%p", pRN);

        result = __InternalCxxFrameHandler<RENAME_EH_EXTERN(__FrameHandler3)>(pExcept, pRN, (PCONTEXT)pContext, pDC, pFuncInfo, 0, nullptr, FALSE);

        EHTRACE_HANDLER_EXIT(result);

        __asm {
            pop     edi
            pop     esi
            pop     ebx
            mov     eax, result
            mov     esp, ebp
            pop     ebp
            ret     0
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // _CallCatchBlock2 - The nitty-gritty details to get the catch called
    //      correctly.
    //
    // We need to guard the call to the catch block with a special registration
    // node, so that if there is an exception which should be handled by a try
    // block within the catch, we handle it without unwinding the SEH node
    // in CallCatchBlock.
    //

    struct CatchGuardRN {
        EHRegistrationNode* pNext;          // Frame link
        void* pFrameHandler;  // Frame Handler
        UINT_PTR            RandomCookie;   // __security_cookie XOR node address
        FuncInfo* pFuncInfo;      // Static info for subject function
        EHRegistrationNode* pRN;            // Dynamic info for subject function
        int                 CatchDepth;     // How deeply nested are we?
    };

    extern "C" EXCEPTION_DISPOSITION __cdecl _CatchGuardHandler(EHExceptionRecord*, CatchGuardRN*, void*, void*);

    __declspec(guard(ignore)) void* _CallCatchBlock2(
        EHRegistrationNode* pRN,            // Dynamic info of function with catch
        FuncInfo* pFuncInfo,      // Static info of function with catch
        void* handlerAddress, // Code address of handler
        int                CatchDepth,      // How deeply nested in catch blocks are we?
        unsigned long      NLGCode
    ) {
        //
        // Call the catch
        //
        return _CallSettingFrame(handlerAddress, pRN, NLGCode);
    }


    /////////////////////////////////////////////////////////////////////////////
    //
    // _CatchGuardHandler - frame handler for the catch guard node.
    //
    // This function will attempt to find a handler for the exception within
    // the current catch block (ie any nested try blocks).  If none is found,
    // or the handler rethrows, returns ExceptionContinueSearch; otherwise does
    // not return.
    //
    // Does nothing on an unwind.
    //

    extern "C" EXCEPTION_DISPOSITION __cdecl _CatchGuardHandler(
        EHExceptionRecord * pExcept,        // Information for this exception
        CatchGuardRN * pRN,            // The special marker frame
        void* pContext,       // Context info (we don't care what's in it)
        void*                              // (ignored)
    ) {
        EHTRACE_FMT1("pRN = 0x%p", pRN);
        __asm cld;      // Our code-gen assumes this

        //
        // Validate our registration record, to secure against hacker attacks.
        //

        EXCEPTION_DISPOSITION result =
            __InternalCxxFrameHandler<RENAME_EH_EXTERN(__FrameHandler3)>(pExcept,
                pRN->pRN,
                (PCONTEXT)pContext,
                nullptr,
                pRN->pFuncInfo,
                pRN->CatchDepth,
                (EHRegistrationNode*)pRN,
                FALSE);
        EHTRACE_HANDLER_EXIT(result);
        return result;
    }


    /////////////////////////////////////////////////////////////////////////////
    //
    // GetRangeOfTrysToCheck - determine which try blocks are of interest, given
    //   the current catch block nesting depth.  We only check the trys at a single
    //   depth.
    //
    // Returns:
    //      Address of first try block of interest is returned
    //      pStart and pEnd get the indices of the range in question
    //
    RENAME_EH_EXTERN(__FrameHandler3)::TryBlockMap::IteratorPair RENAME_EH_EXTERN(__FrameHandler3)::GetRangeOfTrysToCheck(
        RENAME_EH_EXTERN(__FrameHandler3)::TryBlockMap& TryBlockMap,
        __ehstate_t                                    curState,
        DispatcherContext* /*pDC*/,
        FuncInfo* pFuncInfo,
        int                                            CatchDepth
    ) {
        TryBlockMapEntry* pEntry = FUNC_PTRYBLOCK(*pFuncInfo, 0);
        unsigned start = FUNC_NTRYBLOCKS(*pFuncInfo);
        unsigned end = start;
        unsigned end1 = end;

        while (CatchDepth >= 0) {
            _VCRT_VERIFY(start != -1);
            start--;
            if (TBME_HIGH(pEntry[start]) < curState && curState <= TBME_CATCHHIGH(pEntry[start])
                || (start == -1)
                ) {
                CatchDepth--;
                end = end1;
                end1 = start;
            }
        }

        ++start; // We always overshoot by 1 (we may even wrap around)
        _VCRT_VERIFY(end <= FUNC_NTRYBLOCKS(*pFuncInfo) && start <= end);
        auto iterStart = TryBlockMap::iterator(TryBlockMap, start);
        auto iterEnd = TryBlockMap::iterator(TryBlockMap, end);

        return TryBlockMap::IteratorPair(iterStart, iterEnd);
    }


    /////////////////////////////////////////////////////////////////////////////
    //
    // _CreateFrameInfo - Save the frame information for this scope just before
    //  calling the catch block.  Put it at the head of the linked list.  For
    //  x86, all we need to save is the pointer to the exception object, so we
    //  can determine when that object is no longer used by any nested catch
    //  handler and can thus be destroyed on exiting from a catch.
    //
    // Returns:
    //      Pointer to the frame info (the first input argument).
    //
    extern "C" _VCRTIMP FRAMEINFO * __cdecl _CreateFrameInfo(
        FRAMEINFO * pFrameInfo,
        PVOID       pExceptionObject
    ) {
        pFrameInfo->pExceptionObject = pExceptionObject;
        pFrameInfo->pNext = pFrameInfoChain;
        pFrameInfoChain = pFrameInfo;
        return pFrameInfo;
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // _FindAndUnlinkFrame - Remove the frame information for this scope that was
    //  inserted by _CreateFrameInfo.  This should be the first frame in the list
    //  (Ideally), but fibers deviate from ideal situation.
    //
    extern "C" _VCRTIMP void __cdecl _FindAndUnlinkFrame(
        FRAMEINFO * pFrameInfo
    ) {
        if (pFrameInfo == pFrameInfoChain) {
            pFrameInfoChain = pFrameInfo->pNext;
            return;
        }
        else {
            for (FRAMEINFO* pCurFrameInfo = pFrameInfoChain;
                pCurFrameInfo->pNext;
                pCurFrameInfo = pCurFrameInfo->pNext)
            {
                if (pFrameInfo == pCurFrameInfo->pNext) {
                    pCurFrameInfo->pNext = pFrameInfo->pNext;
                    return;
                }
            }
        }

        // Should never be reached.
        abort();
    }

}