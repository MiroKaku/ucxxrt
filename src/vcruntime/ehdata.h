/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ehdata.h
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// Declares internal C++ exception handling data structures.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//

#ifndef _INC_EHDATA
#define _INC_EHDATA
#pragma once

#include <ehdata_values.h>
#include <excpt.h>

#pragma pack(push, ehdata, 4)

#define TD_HASH(td)		((td).hash)
#define TD_NAME(td)		((td).name)

#define TD_IS_TYPE_ELLIPSIS(td) ((td == NULL) || (TD_NAME(*td)[0] == '\0'))

#define CT_PROPERTIES(ct)	((ct).properties)
#if _EH_RELATIVE_TYPEINFO
#define CT_PTD_IB(ct,ib)		((TypeDescriptor *)((ib) + (ct).pType))
#define CT_COPYFUNC_IB(ct,ib)	((void (*)(void*))((ib) + (ct).copyFunction))
#else
#define CT_PTD(ct)			((ct).pType)
#define CT_COPYFUNC(ct)		((ct).copyFunction)
#endif
#define CT_THISDISP(ct)		((ct).thisDisplacement)
#define CT_SIZE(ct)			((ct).sizeOrOffset)
#define CT_OFFSET(ct)		((ct).sizeOrOffset)
#define CT_HASH(ct)			(TD_HASH(*CT_PTD(ct)))
#define CT_NAME(ct)			(TD_NAME(*CT_PTD(ct)))

#if _EH_RELATIVE_TYPEINFO
#define CT_NAME_IB(ct,ib)       (TD_NAME(*CT_PTD_IB(ct, ib)))
#endif

#define SET_CT_ISSIMPLETYPE(ct)		(CT_PROPERTIES(ct) |= CT_IsSimpleType)
#define SET_CT_BYREFONLY(ct)		(CT_PROPERTIES(ct) |= CT_ByReferenceOnly)
#define SET_CT_HASVB(ct)			(CT_PROPERTIES(ct) |= CT_HasVirtualBase)
#define SET_CT_ISWINRTHANDLE(ct)	(CT_PROPERTIES(ct) |= CT_IsWinRTHandle)
#define SET_CT_ISSTDBADALLOC(ct)	(CT_PROPERTIES(ct) |= CT_IsStdBadAlloc)

#define CT_ISSIMPLETYPE(ct)			(CT_PROPERTIES(ct) & CT_IsSimpleType)		// Is it a simple type?
#define CT_BYREFONLY(ct)			(CT_PROPERTIES(ct) & CT_ByReferenceOnly)	// Must it be caught by reference?
#define CT_HASVB(ct)				(CT_PROPERTIES(ct) & CT_HasVirtualBase)		// Is this type a class with virtual bases?
#define CT_ISWINRTHANDLE(ct)		(CT_PROPERTIES(ct) & CT_IsWinRTHandle)		// Is it a winrt handle?
#define CT_ISSTDBADALLOC(ct)		(CT_PROPERTIES(ct) & CT_IsStdBadAlloc)		// Is it a std::bad_alloc?

#define THROW_ATTRS(t)			((t).attributes)
#if _EH_RELATIVE_TYPEINFO
#define THROW_UNWINDFUNC_IB(t,ib)		((void (*)(void*))((ib) + (t).pmfnUnwind))
#define THROW_FORWARDCOMPAT_IB(t,ib)	((int(__cdecl *)(...))((ib) + (t).pForwardCompat))
#define THROW_CTARRAY_IB(t,ib)			((CatchableTypeArray*)((ib) + (t).pCatchableTypeArray))
#define THROW_COUNT_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->nCatchableTypes)
#define THROW_CTLIST_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->arrayOfCatchableTypes)
#else
#define THROW_FORWARDCOMPAT(t)	((t).pForwardCompat)
#define THROW_COUNT(t)			((t).pCatchableTypeArray->nCatchableTypes)
#define THROW_CTLIST(t)			((t).pCatchableTypeArray->arrayOfCatchableTypes)
#endif
#define THROW_UNWINDFUNC(t)		((t).pmfnUnwind)
#define THROW_PCTLIST(t)		(&THROW_CTLIST(t))
#define THROW_CT(t, n)			(*THROW_CTLIST(t)[n])
#define THROW_PCT(t, n)			(THROW_CTLIST(t)[n])

#define THROW_ISCONST(t)		(THROW_ATTRS(t) & TI_IsConst)
#define THROW_ISVOLATILE(t)		(THROW_ATTRS(t) & TI_IsVolatile)
#define THROW_ISUNALIGNED(t)	(THROW_ATTRS(t) & TI_IsUnaligned)
#define THROW_ISPURE(t)			(THROW_ATTRS(t) & TI_IsPure)
#define THROW_ISWINRT(t)		(THROW_ATTRS(t) & TI_IsWinRT)

/////////////////////////////////////////////////////////////////////////////
//
// The following data structures describe 'try/catch' blocks:
//
//---------------------------------------------------------------------------

//
// Current state of a function.
// -1 is the 'blank' state, i.e. there is nothing to unwind, no try blocks active.
//

typedef int __ehstate_t;		// The type of a state index


//
// HandlerType - description of a single 'catch'
//
typedef const struct _s_HandlerType {
    unsigned int	adjectives;			// Handler Type adjectives (bitfield)
#if _EH_RELATIVE_FUNCINFO
    int				dispType;			// Image relative offset of the corresponding type descriptor
    int				dispCatchObj;		// Displacement of catch object from base
    int				dispOfHandler;		// Image relative offset of 'catch' code
#if defined(_WIN64) || defined(_CHPE_X86_ARM64_EH_)
    int				dispFrame;			// displacement of address of function frame wrt establisher frame
#endif
#else   // _EH_RELATIVE_FUNCINFO
    TypeDescriptor*	pType;				// Pointer to the corresponding type descriptor
    ptrdiff_t		dispCatchObj;		// Displacement of catch object from base
    void *			addressOfHandler;	// Address of 'catch' code
#endif // _EH_RELATIVE_FUNCINFO
} HandlerType;

#define HT_ADJECTIVES(ht)		((ht).adjectives)
#if _EH_RELATIVE_FUNCINFO
#define HT_PTD_IB(ht,ib)		((TypeDescriptor*)((ib) + (ht).dispType))
#define HT_HANDLER_IB(ht,ib)	((void*)((ib) + (ht).dispOfHandler))
#else
#define HT_PTD(ht)				((ht).pType)
#define HT_HANDLER(ht)			((ht).addressOfHandler)
#endif
#define HT_DISPCATCH(ht)		((ht).dispCatchObj)
#define HT_NAME(ht)				(TD_NAME(*HT_PTD(ht)))
#define HT_HASH(ht)				(TD_HASH(*HT_PTD(ht)))
#define HT_IS_TYPE_ELLIPSIS(ht)	TD_IS_TYPE_ELLIPSIS(HT_PTD(ht))

#define HT_ISCONST(ht)			(HT_ADJECTIVES(ht) & HT_IsConst)		// Is the type referenced 'const' qualified
#define HT_ISVOLATILE(ht)		(HT_ADJECTIVES(ht) & HT_IsVolatile)		// Is the type referenced 'volatile' qualified
#define HT_ISUNALIGNED(ht)		(HT_ADJECTIVES(ht) & HT_IsUnaligned)	// Is the type referenced 'unaligned' qualified
#define HT_ISREFERENCE(ht)		(HT_ADJECTIVES(ht) & HT_IsReference)	// Is the catch type by reference
#define HT_ISRESUMABLE(ht)		(HT_ADJECTIVES(ht) & HT_IsResumable)	// Might the catch choose to resume (Reserved)
#define HT_ISCOMPLUSEH(ht)      (HT_ADJECTIVES(ht) & HT_IsComplusEh)

#define HT_ISBADALLOCCOMPAT(ht) (HT_ADJECTIVES(ht) & HT_IsBadAllocCompat)

#define HT_IS_STD_DOTDOT(ht)    (HT_ADJECTIVES(ht) & HT_IsStdDotDot)

#define SET_HT_ISCONST(ht)		(HT_ADJECTIVES(ht) |= HT_IsConst)
#define SET_HT_ISVOLATILE(ht)	(HT_ADJECTIVES(ht) |= HT_IsVolatile)
#define SET_HT_ISUNALIGNED(ht)	(HT_ADJECTIVES(ht) |= HT_IsUnaligned)
#define SET_HT_ISREFERENCE(ht)	(HT_ADJECTIVES(ht) |= HT_IsReference)
#define SET_HT_ISRESUMABLE(ht)	(HT_ADJECTIVES(ht) |= HT_IsResumable)
#define SET_HT_ISCOMPLUSEH(ht)	(HT_ADJECTIVES(ht) |= HT_IsComplusEh)

//
// HandlerMapEntry - associates a handler list (sequence of catches) with a
//	range of eh-states.
//
typedef const struct _s_TryBlockMapEntry {
    __ehstate_t		tryLow;				// Lowest state index of try
    __ehstate_t		tryHigh;			// Highest state index of try
    __ehstate_t		catchHigh;			// Highest state index of any associated catch
    int				nCatches;			// Number of entries in array
#if _EH_RELATIVE_FUNCINFO
    int				dispHandlerArray;	// Image relative offset of list of handlers for this try
#else
    HandlerType* pHandlerArray;	// List of handlers for this try
#endif
} TryBlockMapEntry;

#define TBME_LOW(hm)		((hm).tryLow)
#define TBME_HIGH(hm)		((hm).tryHigh)
#define TBME_CATCHHIGH(hm)	((hm).catchHigh)
#define TBME_NCATCHES(hm)	((hm).nCatches)
#if _EH_RELATIVE_FUNCINFO
#define TBME_PLIST(hm,ib)	((HandlerType*)((ib) + (hm).dispHandlerArray))
#define TBME_CATCH(hm,n,ib)	(TBME_PLIST(hm,ib)[n])
#define TBME_PCATCH(hm,n,ib)(&(TBME_PLIST(hm,ib)[n]))
#else
#define TBME_PLIST(hm)		((hm).pHandlerArray)
#define TBME_CATCH(hm, n)	(TBME_PLIST(hm)[n])
#define TBME_PCATCH(hm, n)	(&(TBME_PLIST(hm)[n]))
#endif


/////////////////////////////////////////////////////////////////////////////
//
// The following data structures describe function layout to the EH runtime
//
//---------------------------------------------------------------------------

//
// UnwindMapEntry - Description of each state transition for unwinding
//	the stack (i.e. calling destructors).
//
// The unwind map is an array, indexed by current state.  Each entry specifies
// the state to go to during unwind, and the action required to get there.
// Note that states are represented by a signed integer, and that the 'blank'
// state is -1 so that the array remains 0-based (because by definition there
// is never any unwind action to be performed from state -1).  It is also
// assumed that state indices will be dense, i.e. that there will be no gaps of
// unused state indices in a function.
//

typedef const struct _s_UnwindMapEntry {
    __ehstate_t	toState;					// State this action takes us to
#if _EH_RELATIVE_FUNCINFO
    int			action;						// Image relative offset of funclet
#else
    void		(__cdecl * action)(void);	// Funclet to call to effect state change
#endif
} UnwindMapEntry;

#define UWE_TOSTATE(uwe)	((uwe).toState)
#if _EH_RELATIVE_FUNCINFO
#define UWE_ACTION_IB(uwe,ib)	((void (__cdecl *)(void))((ib) + (uwe).action))
#else
#define UWE_ACTION(uwe)			((uwe).action)
#endif

#if defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)
typedef struct IptoStateMapEntry {
    unsigned int	Ip;		// Image relative offset of IP
    __ehstate_t		State;
} IptoStateMapEntry;
#endif

typedef const struct _s_ESTypeList
{
    int 			nCount;			// how many types are there
#if _EH_RELATIVE_TYPEINFO
    int				dispTypeArray;	// offset of list of types in exception specification
#else
    HandlerType*	pTypeArray;		// List of types in exception specification
#endif
}ESTypeList;

#define EST_COUNT(x)    ((x)->nCount)
#if _EH_RELATIVE_TYPEINFO
#define EST_ARRAY_IB(estl, ib, n)  (&(((HandlerType*)((ib) + (estl)->dispTypeArray))[n]))
#else
#define EST_ARRAY(x,n)  (&((x)->pTypeArray[n]))
#endif

//
// FuncInfo - all the information that describes a function with exception
//	handling information.
//

// bbtFlags values
#define BBT_UNIQUE_FUNCINFO 1

/*
 * The magicNumber here is incremented with every compiler change that does not
 * break backwards compatibility. If for some reason backward compatibility
 * should be broken, then we will add new handler. What this means is that
 * current handler functions can assume that the structure layout that they
 * know about will remain the same and so even if magicNumber > my_magicNumber,
 * the handler can assume that what all it needs is there. The magicNumber will
 * be revised every time new data is added at the end of this structure.
 */
typedef const struct _s_FuncInfo
{
    unsigned int		magicNumber:29;		// Identifies version of compiler
    unsigned int		bbtFlags:3;			// flags that may be set by BBT processing
    __ehstate_t			maxState;			// Highest state number plus one (thus
                                            // number of entries in unwind map)
#if _EH_RELATIVE_FUNCINFO
    int					dispUnwindMap;		// Image relative offset of the unwind map
    unsigned int		nTryBlocks;			// Number of 'try' blocks in this function
    int					dispTryBlockMap;	// Image relative offset of the handler map
    unsigned int		nIPMapEntries;		// # entries in the IP-to-state map. NYI (reserved)
    int					dispIPtoStateMap;	// Image relative offset of the IP to state map
    int					dispUwindHelp;		// Displacement of unwind helpers from base
    int					dispESTypeList;		// Image relative list of types for exception specifications
#else
    UnwindMapEntry*		pUnwindMap;			// Where the unwind map is
    unsigned int		nTryBlocks;			// Number of 'try' blocks in this function
    TryBlockMapEntry*	pTryBlockMap;		// Where the handler map is
    unsigned int		nIPMapEntries;		// # entries in the IP-to-state map. NYI (reserved)
    void*				pIPtoStateMap;		// An IP to state map.  NYI (reserved).
    ESTypeList*			pESTypeList;		// List of types for exception specifications
#endif
    int					EHFlags;			// Flags for some features.
} FuncInfo;

#define FUNC_MAGICNUM(fi)		((fi).magicNumber)
#define FUNC_MAXSTATE(fi)		((fi).maxState)
#define FUNC_NTRYBLOCKS(fi)		((fi).nTryBlocks)
#define FUNC_NIPMAPENT(fi)		((fi).nIPMapEntries)
#define FUNC_FLAGS(fi)			((fi).EHFlags)
#if _EH_RELATIVE_FUNCINFO
#define FUNC_PUNWINDMAP(fi,ib)	((UnwindMapEntry*)((ib) + (fi).dispUnwindMap))
#define FUNC_PHANDLERMAP(fi,ib)	((TryBlockMapEntry*)((ib) + (fi).dispTryBlockMap))
#define FUNC_IPMAP(fi,ib)		((IptoStateMapEntry*)((ib) + (fi).dispIPtoStateMap))
#define FUNC_UNWIND(fi,st,ib)	(FUNC_PUNWINDMAP(fi,ib)[st])
#define FUNC_PUNWIND(fi,st,ib)	(&FUNC_UNWIND(fi,st,ib))
#define FUNC_TRYBLOCK(fi,n,ib)	(FUNC_PHANDLERMAP(fi,ib)[n])
#define FUNC_PTRYBLOCK(fi,n,ib)	(&FUNC_TRYBLOCK(fi,n,ib))
#define FUNC_ESTYPES_IB(fi, ib)	((ESTypeList*)((ib) + (fi).dispESTypeList))
#define FUNC_PESTYPES_IB(fi, ib)	FUNC_ESTYPES_IB((*fi), ib)
#else
#define FUNC_PUNWINDMAP(fi)		((fi).pUnwindMap)
#define FUNC_PHANDLERMAP(fi)	((fi).pTryBlockMap)
#define FUNC_IPMAP(fi)			((fi).pIPtoStateMap)
#define FUNC_UNWIND(fi, st)		((fi).pUnwindMap[st])
#define FUNC_PUNWIND(fi, st)	(&FUNC_UNWIND(fi, st))
#define FUNC_TRYBLOCK(fi,n)		((fi).pTryBlockMap[n])
#define FUNC_PTRYBLOCK(fi,n)	(&FUNC_TRYBLOCK(fi, n))
#define FUNC_ESTYPES(fi)		((fi).pESTypeList)
#define FUNC_PESTYPES(fi)		(FUNC_ESTYPES(*fi))
#endif
#if _EH_RELATIVE_FUNCINFO
#define FUNC_IPTOSTATE(fi,n,ib)	(FUNC_IPMAP(fi,ib)[n])
#define FUNC_PIPTOSTATE(fi,n,ib)(&FUNC_IPTOSTATE(fi,n,ib))
#define FUNC_DISPUNWINDHELP(fi)	((fi).dispUwindHelp)
#else
#define FUNC_IPTOSTATE(fi,n) 	__ERROR_NYI__
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Data types that are variants of data used by NT (and Chicago) to manage
// exception handling.
//
//---------------------------------------------------------------------------

#define RUNTIME_FUNCTION_INDIRECT 0x1

/////////////////////////////////////////////////////////////////////////////
//
// A stack registration node (i386 only)
//

#if defined(_M_IX86) && !defined(_CHPE_X86_ARM64_EH_)

struct EHRegistrationNode;
typedef struct EHRegistrationNode EHRegistrationNode;

struct EHRegistrationNode {
    /* void *			stackPtr */		// Stack ptr at entry to try (below address point)
    EHRegistrationNode*	pNext;			// Next node in the chain
    void * 				frameHandler;	// The handler function for this frame
    __ehstate_t			state;			// The current state of this function
};

# define FRAME_OFFSET	sizeof(EHRegistrationNode)

#define PRN_NEXT(prn)		((prn)->pNext)
#define PRN_HANDLER(prn)	((prn)->frameHandler)
#define PRN_STATE(prn)		((prn)->state)
#define PRN_STACK(prn)		(((void**)(prn))[-1])
# define PRN_FRAME(prn)		((void*)(((char*)prn) + FRAME_OFFSET))

typedef void DispatcherContext;		// Meaningless on x86

#elif defined(_M_ARM)

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0

typedef struct _UNWIND_INFO {
    unsigned short Version;
    unsigned short Flags;
    unsigned int DataLength;
} UNWIND_INFO, * PUNWIND_INFO;

typedef struct _xDISPATCHER_CONTEXT {
    ULONG ControlPc;
    ULONG ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
    ULONG EstablisherFrame;
    ULONG TargetPc;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PVOID HistoryTable;
    ULONG ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    PUCHAR NonVolatileRegisters;
} DispatcherContext;					// changed the case of the name to conform to EH conventions


//
// Define unwind information flags.
//

#define UNW_FLAG_NHANDLER               0x0             /* any handler */
#define UNW_FLAG_EHANDLER               0x1             /* filter handler */
#define UNW_FLAG_UHANDLER               0x2             /* unwind handler */

//
// Define unwind history table structure.
//

#define UNWIND_HISTORY_TABLE_SIZE 12

typedef struct _UNWIND_HISTORY_TABLE_ENTRY {
    DWORD ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
} UNWIND_HISTORY_TABLE_ENTRY, * PUNWIND_HISTORY_TABLE_ENTRY;

typedef struct _UNWIND_HISTORY_TABLE {
    DWORD Count;
    BYTE  LocalHint;
    BYTE  GlobalHint;
    BYTE  Search;
    BYTE  Once;
    DWORD LowAddress;
    DWORD HighAddress;
    UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
} UNWIND_HISTORY_TABLE, * PUNWIND_HISTORY_TABLE;

//
// Define exception dispatch context structure.
//

typedef struct _DISPATCHER_CONTEXT {
    DWORD ControlPc;
    DWORD ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
    DWORD EstablisherFrame;
    DWORD TargetPc;
    PCONTEXT ContextRecord;
    PEXCEPTION_ROUTINE LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    DWORD ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    PBYTE  NonVolatileRegisters;
    DWORD Reserved;
} DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

//
// Define exception filter and termination handler function types.
// N.B. These functions use a custom calling convention.
//

struct _EXCEPTION_POINTERS;
typedef
LONG
(*PEXCEPTION_FILTER) (
    struct _EXCEPTION_POINTERS* ExceptionPointers,
    DWORD EstablisherFrame
    );

typedef
VOID
(*PTERMINATION_HANDLER) (
    BOOLEAN AbnormalTermination,
    DWORD EstablisherFrame
    );

//
// Define dynamic function table entry.
//

typedef
_Function_class_(GET_RUNTIME_FUNCTION_CALLBACK)
PIMAGE_RUNTIME_FUNCTION_ENTRY
GET_RUNTIME_FUNCTION_CALLBACK(
    _In_ DWORD ControlPc,
    _In_opt_ PVOID Context
);
typedef GET_RUNTIME_FUNCTION_CALLBACK* PGET_RUNTIME_FUNCTION_CALLBACK;

typedef
_Function_class_(OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK)
DWORD
OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK(
    _In_ HANDLE Process,
    _In_ PVOID TableAddress,
    _Out_ PDWORD Entries,
    _Out_ PIMAGE_RUNTIME_FUNCTION_ENTRY* Functions
);
typedef OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK* POUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK;

#define OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK_EXPORT_NAME \
    "OutOfProcessFunctionTableCallback"

//
// Nonvolatile context pointer record.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS {

    PDWORD R4;
    PDWORD R5;
    PDWORD R6;
    PDWORD R7;
    PDWORD R8;
    PDWORD R9;
    PDWORD R10;
    PDWORD R11;
    PDWORD Lr;

    PULONGLONG D8;
    PULONGLONG D9;
    PULONGLONG D10;
    PULONGLONG D11;
    PULONGLONG D12;
    PULONGLONG D13;
    PULONGLONG D14;
    PULONGLONG D15;

} KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;


//
// On ARM we don't have a registration node, just a pointer to the stack frame base
//

typedef ULONG EHRegistrationNode;

#elif defined(_M_X64)

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0

#pragma warning (push)
#pragma warning (disable: 4201)

typedef union _UNWIND_CODE {
    struct {
        unsigned char CodeOffset;
        unsigned char UnwindOp : 4;
        unsigned char OpInfo : 4;
    };
    unsigned short FrameOffset;
} UNWIND_CODE, *PUNWIND_CODE;
typedef struct _UNWIND_INFO {
    unsigned char Version : 3;
    unsigned char Flags : 5;
    unsigned char SizeOfProlog;
    unsigned char CountOfCodes;
    unsigned char FrameRegister : 4;
    unsigned char FrameOffset : 4;
    UNWIND_CODE UnwindCode[1];
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes+1)&~1)-1];
 *  union {
 *      OPTIONAL unsigned long ExceptionHandler;
 *      OPTIONAL unsigned long FunctionEntry;
 *  };
 *  OPTIONAL unsigned long ExceptionData[];
 */
} UNWIND_INFO, * PUNWIND_INFO;

#pragma warning (pop)

typedef struct _UNWIND_HISTORY_TABLE* PUNWIND_HISTORY_TABLE;

typedef struct _xDISPATCHER_CONTEXT {
    long long ControlPc;
    long long ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG64 TargetIp;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    ULONG ScopeIndex;
} DispatcherContext;					// changed the case of the name to conform to EH conventions

#ifdef _KERNEL_MODE
//
// Define unwind information flags.
//

#define UNW_FLAG_NHANDLER       0x0
#define UNW_FLAG_EHANDLER       0x1
#define UNW_FLAG_UHANDLER       0x2
#define UNW_FLAG_CHAININFO      0x4

#define UNW_FLAG_NO_EPILOGUE    0x80000000UL    // Software only flag

#define UNWIND_CHAIN_LIMIT      32

//
// Define unwind history table structure.
//

#define UNWIND_HISTORY_TABLE_SIZE 12

typedef struct _UNWIND_HISTORY_TABLE_ENTRY {
    DWORD64 ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
} UNWIND_HISTORY_TABLE_ENTRY, * PUNWIND_HISTORY_TABLE_ENTRY;

typedef struct _UNWIND_HISTORY_TABLE {
    DWORD Count;
    BYTE  LocalHint;
    BYTE  GlobalHint;
    BYTE  Search;
    BYTE  Once;
    DWORD64 LowAddress;
    DWORD64 HighAddress;
    UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
} UNWIND_HISTORY_TABLE, * PUNWIND_HISTORY_TABLE;

//
// Define dynamic function table entry.
//

typedef
_Function_class_(GET_RUNTIME_FUNCTION_CALLBACK)
PIMAGE_RUNTIME_FUNCTION_ENTRY
GET_RUNTIME_FUNCTION_CALLBACK(
    _In_ DWORD64 ControlPc,
    _In_opt_ PVOID Context
);
typedef GET_RUNTIME_FUNCTION_CALLBACK* PGET_RUNTIME_FUNCTION_CALLBACK;

typedef
_Function_class_(OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK)
DWORD
OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK(
    _In_ HANDLE Process,
    _In_ PVOID TableAddress,
    _Out_ PDWORD Entries,
    _Out_ PIMAGE_RUNTIME_FUNCTION_ENTRY* Functions
);
typedef OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK* POUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK;

#define OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK_EXPORT_NAME \
    "OutOfProcessFunctionTableCallback"

//
// Define exception dispatch context structure.
//

typedef struct _DISPATCHER_CONTEXT {
    DWORD64 ControlPc;
    DWORD64 ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
    DWORD64 EstablisherFrame;
    DWORD64 TargetIp;
    PCONTEXT ContextRecord;
    PEXCEPTION_ROUTINE LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    DWORD ScopeIndex;
    DWORD Fill0;
} DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

//
// Define exception filter and termination handler function types.
//

struct _EXCEPTION_POINTERS;
typedef
LONG
(*PEXCEPTION_FILTER) (
    struct _EXCEPTION_POINTERS* ExceptionPointers,
    PVOID EstablisherFrame
    );

typedef
VOID
(*PTERMINATION_HANDLER) (
    BOOLEAN AbnormalTermination,
    PVOID EstablisherFrame
    );

//
// Nonvolatile context pointer record.
//

#pragma warning(push)
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
typedef struct _KNONVOLATILE_CONTEXT_POINTERS {
    union {
        PM128A FloatingContext[16];
        struct {
            PM128A Xmm0;
            PM128A Xmm1;
            PM128A Xmm2;
            PM128A Xmm3;
            PM128A Xmm4;
            PM128A Xmm5;
            PM128A Xmm6;
            PM128A Xmm7;
            PM128A Xmm8;
            PM128A Xmm9;
            PM128A Xmm10;
            PM128A Xmm11;
            PM128A Xmm12;
            PM128A Xmm13;
            PM128A Xmm14;
            PM128A Xmm15;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    union {
        PDWORD64 IntegerContext[16];
        struct {
            PDWORD64 Rax;
            PDWORD64 Rcx;
            PDWORD64 Rdx;
            PDWORD64 Rbx;
            PDWORD64 Rsp;
            PDWORD64 Rbp;
            PDWORD64 Rsi;
            PDWORD64 Rdi;
            PDWORD64 R8;
            PDWORD64 R9;
            PDWORD64 R10;
            PDWORD64 R11;
            PDWORD64 R12;
            PDWORD64 R13;
            PDWORD64 R14;
            PDWORD64 R15;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME2;

} KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;
#pragma warning(pop)

#endif // _KERNEL_MODE

//
// On P7 we don't have a registration node, just a pointer to the stack frame base
//
typedef ULONG_PTR EHRegistrationNode;

#elif defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

#define PRN_NEXT(prn)		__ERROR__
#define PRN_HANDLER(prn)	__ERROR__
#define PRN_STATE(prn)		__ERROR__
#define PRN_STACK(prn)		__ERROR__
#define PRN_FRAME(prn)		__ERROR__

#define FRAME_OFFSET		0


#pragma warning (push)
#pragma warning (disable: 4201)

typedef union _UNWIND_CODE {
    struct {
        unsigned char CodeOffset;
        unsigned char UnwindOp : 4;
        unsigned char OpInfo : 4;
    };
    unsigned short FrameOffset;
} UNWIND_CODE, * PUNWIND_CODE;

#pragma warning (pop)
typedef struct _UNWIND_INFO {
    unsigned char Version : 3;
    unsigned char Flags : 5;
    unsigned char SizeOfProlog;
    unsigned char CountOfCodes;
    unsigned char FrameRegister : 4;
    unsigned char FrameOffset : 4;
    UNWIND_CODE UnwindCode[1];
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes+1)&~1)-1];
 *  union {
 *      OPTIONAL unsigned long ExceptionHandler;
 *      OPTIONAL unsigned long FunctionEntry;
 *  };
 *  OPTIONAL unsigned long ExceptionData[];
 */
} UNWIND_INFO, * PUNWIND_INFO;

typedef struct _UNWIND_HISTORY_TABLE* PUNWIND_HISTORY_TABLE;

typedef struct _xDISPATCHER_CONTEXT {
    ULONG_PTR ControlPc;
    ULONG_PTR ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG_PTR TargetPc;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    DWORD ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    UCHAR Pad[3];
    PUCHAR NonVolatileRegisters;
} DispatcherContext;

//
// Define unwind information flags.
//

#define UNW_FLAG_NHANDLER               0x0             /* any handler */
#define UNW_FLAG_EHANDLER               0x1             /* filter handler */
#define UNW_FLAG_UHANDLER               0x2             /* unwind handler */

//
// Define unwind history table structure.
//

#define UNWIND_HISTORY_TABLE_SIZE 12

typedef struct _UNWIND_HISTORY_TABLE_ENTRY {
    DWORD64 ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
} UNWIND_HISTORY_TABLE_ENTRY, * PUNWIND_HISTORY_TABLE_ENTRY;

typedef struct _UNWIND_HISTORY_TABLE {
    DWORD Count;
    BYTE  LocalHint;
    BYTE  GlobalHint;
    BYTE  Search;
    BYTE  Once;
    DWORD64 LowAddress;
    DWORD64 HighAddress;
    UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
} UNWIND_HISTORY_TABLE, * PUNWIND_HISTORY_TABLE;

#pragma push_macro("_DISPATCHER_CONTEXT_ARM64")
#undef _DISPATCHER_CONTEXT_ARM64
#define _DISPATCHER_CONTEXT_ARM64 _DISPATCHER_CONTEXT

//
// Define exception dispatch context structure.
//

typedef struct _DISPATCHER_CONTEXT_ARM64 {
    ULONG_PTR ControlPc;
    ULONG_PTR ImageBase;
    PIMAGE_RUNTIME_FUNCTION_ENTRY FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG_PTR TargetPc;
    PARM64_NT_CONTEXT ContextRecord;
    PEXCEPTION_ROUTINE LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    DWORD ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    PBYTE  NonVolatileRegisters;
} DISPATCHER_CONTEXT_ARM64, * PDISPATCHER_CONTEXT_ARM64;

#undef _DISPATCHER_CONTEXT_ARM64
#pragma pop_macro("_DISPATCHER_CONTEXT_ARM64")

typedef DISPATCHER_CONTEXT_ARM64 DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

//
// Define exception filter and termination handler function types.
// N.B. These functions use a custom calling convention.
//

struct _EXCEPTION_POINTERS;
typedef
LONG
(*PEXCEPTION_FILTER) (
    struct _EXCEPTION_POINTERS* ExceptionPointers,
    DWORD64 EstablisherFrame
    );

typedef
VOID
(*PTERMINATION_HANDLER) (
    BOOLEAN AbnormalTermination,
    DWORD64 EstablisherFrame
    );

//
// Define dynamic function table entry.
//

typedef
_Function_class_(GET_RUNTIME_FUNCTION_CALLBACK)
PIMAGE_RUNTIME_FUNCTION_ENTRY
GET_RUNTIME_FUNCTION_CALLBACK(
    _In_ DWORD64 ControlPc,
    _In_opt_ PVOID Context
);
typedef GET_RUNTIME_FUNCTION_CALLBACK* PGET_RUNTIME_FUNCTION_CALLBACK;

typedef
_Function_class_(OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK)
DWORD
OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK(
    _In_ HANDLE Process,
    _In_ PVOID TableAddress,
    _Out_ PDWORD Entries,
    _Out_ PIMAGE_RUNTIME_FUNCTION_ENTRY* Functions
);
typedef OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK* POUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK;

#define OUT_OF_PROCESS_FUNCTION_TABLE_CALLBACK_EXPORT_NAME \
    "OutOfProcessFunctionTableCallback"

//
// Nonvolatile context pointer record.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS_ARM64 {

    PDWORD64 X19;
    PDWORD64 X20;
    PDWORD64 X21;
    PDWORD64 X22;
    PDWORD64 X23;
    PDWORD64 X24;
    PDWORD64 X25;
    PDWORD64 X26;
    PDWORD64 X27;
    PDWORD64 X28;
    PDWORD64 Fp;
    PDWORD64 Lr;

    PDWORD64 D8;
    PDWORD64 D9;
    PDWORD64 D10;
    PDWORD64 D11;
    PDWORD64 D12;
    PDWORD64 D13;
    PDWORD64 D14;
    PDWORD64 D15;

} KNONVOLATILE_CONTEXT_POINTERS_ARM64, * PKNONVOLATILE_CONTEXT_POINTERS_ARM64;

typedef KNONVOLATILE_CONTEXT_POINTERS_ARM64 KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;

//
// On ARM64 we don't have a registration node, just a pointer to the stack frame base
//
typedef ULONG_PTR EHRegistrationNode;

#else
#error "Machine not supported"
#endif

/////////////////////////////////////////////////////////////////////////////
//
// The NT Exception record that we use to pass information from the throw to
// the possible catches.
//
// The constants in the comments are the values we expect.
// This is based on the definition of EXCEPTION_RECORD in winnt.h.
//
#if defined(_WIN64)
#pragma pack(push, EHExceptionRecord, 8)
#endif
typedef struct EHExceptionRecord {
    unsigned long	ExceptionCode;			// The code of this exception. (= EH_EXCEPTION_NUMBER)
    unsigned long	ExceptionFlags;			// Flags determined by NT
    struct _EXCEPTION_RECORD *	ExceptionRecord;	// An extra exception record (not used)
    void *		ExceptionAddress;		// Address at which exception occurred
    unsigned long 		NumberParameters;		// Number of extended parameters. (= EH_EXCEPTION_PARAMETERS)
    struct EHParameters {
        unsigned long		magicNumber;		// = EH_MAGIC_NUMBER1
        void *		pExceptionObject;	// Pointer to the actual object thrown
        ThrowInfo*	pThrowInfo;		// Description of thrown object
#if _EH_RELATIVE_TYPEINFO
        void *		pThrowImageBase;	// Image base of thrown object
#endif
        } params;
} EHExceptionRecord;

#if defined(_WIN64)
#pragma pack(pop, EHExceptionRecord)
#endif

#define PER_CODE(per)		((per)->ExceptionCode)
#define PER_FLAGS(per)		((per)->ExceptionFlags)
#define PER_NEXT(per)		((per)->ExceptionRecord)
#define PER_ADDRESS(per)	((per)->ExceptionAddress)
#define PER_NPARAMS(per)	((per)->NumberParameters)
#define PER_MAGICNUM(per)	((per)->params.magicNumber)
#define PER_PEXCEPTOBJ(per)	((per)->params.pExceptionObject)
#define PER_PTHROW(per)		((per)->params.pThrowInfo)
#define PER_EXCEPTINFO(per)	((PULONG_PTR)&(PER_MAGICNUM(per)))
#if _EH_RELATIVE_TYPEINFO
#define PER_PTHROWIB(per)	((per)->params.pThrowImageBase)
#endif
#define PER_THROW(per)		(*PER_PTHROW(per))

#define PER_ISSIMPLETYPE(t)	(PER_THROW(t).isSimpleType)
#define PER_ISREFERENCE(t)	(PER_THROW(t).isReference)
#define PER_ISCONST(t)		(PER_THROW(t).isConst)
#define PER_ISVOLATILE(t)	(PER_THROW(t).isVolatile)
#define PER_ISUNALIGNED(t)	(PER_THROW(t).isUnaligned)
#define PER_UNWINDFUNC(t)	(PER_THROW(t).pmfnUnwind)
#define PER_PCTLIST(t)		(PER_THROW(t).pCatchable)
#define PER_CTLIST(t)		(*PER_PCTLIST(t))

#if defined(_M_CEE_PURE)
#define PER_IS_MSVC_EH(per)	((PER_CODE(per) == EH_EXCEPTION_NUMBER) && 			\
                             (PER_NPARAMS(per) == EH_EXCEPTION_PARAMETERS) &&	\
                             ((PER_MAGICNUM(per) == EH_PURE_MAGIC_NUMBER1) ||	\
                              PER_PTHROW(per) == NULL))
#else
#define PER_IS_MSVC_EH(per)	((PER_CODE(per) == EH_EXCEPTION_NUMBER) && 			\
                             (PER_NPARAMS(per) == EH_EXCEPTION_PARAMETERS) &&	\
                             ((PER_MAGICNUM(per) == EH_MAGIC_NUMBER1) ||   \
                              (PER_MAGICNUM(per) == EH_MAGIC_NUMBER2) ||   \
                              (PER_MAGICNUM(per) == EH_MAGIC_NUMBER3)))
#endif

#define PER_IS_MSVC_PURE_OR_NATIVE_EH(per) \
                            ((PER_CODE(per) == EH_EXCEPTION_NUMBER) && 			\
                             (PER_NPARAMS(per) == EH_EXCEPTION_PARAMETERS) &&	\
                             ((PER_MAGICNUM(per) == EH_MAGIC_NUMBER1) ||   \
                              (PER_MAGICNUM(per) == EH_MAGIC_NUMBER2) ||   \
                              (PER_MAGICNUM(per) == EH_MAGIC_NUMBER3) ||   \
                              (PER_MAGICNUM(per) == EH_PURE_MAGIC_NUMBER1)))

#pragma pack(pop, ehdata)

#endif /* _INC_EHDATA */
