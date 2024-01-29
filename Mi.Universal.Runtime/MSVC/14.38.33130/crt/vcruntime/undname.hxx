//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft shared
// source or premium shared source license agreement under which you licensed
// this source code. If you did not accept the terms of the license agreement,
// you are not authorized to use this source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the SOURCE.RTF on your install media or the root of your tools installation.
// THE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
#pragma once

#include "undname.h"

#ifndef __UNDNAME_IMP
#define __UNDNAME_IMP __cdecl
#endif

#ifdef _CRTBLD
#define __UNDNAME_NAME      __unDName
#define __UNDNAME_NAME_EX   __unDNameEx
#else
#define __UNDNAME_NAME      unDName
#define __UNDNAME_NAME_EX   unDNameEx
#endif

#ifndef NO_COMPILER_NAMES
#define NO_COMPILER_NAMES 0
#endif

#ifndef VERS_32BIT
#define VERS_32BIT 1
#endif

// I can't live without 'elif'

#define elif else if


// Good ol' TRUE and FALSE

//#define TRUE    true
//#define FALSE   false


// The 'operator-code' values for the decorated name specification

#define OC_ctor                         ((unsigned char)'0')
#define OC_dtor                         ((unsigned char)'1')
#define OC_new                          ((unsigned char)'2')
#define OC_delete                       ((unsigned char)'3')
#define OC_assign                       ((unsigned char)'4')
#define OC_rshift                       ((unsigned char)'5')
#define OC_lshift                       ((unsigned char)'6')
#define OC_not                          ((unsigned char)'7')
#define OC_equal                        ((unsigned char)'8')
#define OC_unequal                      ((unsigned char)'9')
#define OC_index                        ((unsigned char)'A')
#define OC_udc                          ((unsigned char)'B')
#define OC_pointer                      ((unsigned char)'C')
#define OC_star                         ((unsigned char)'D')
#define OC_incr                         ((unsigned char)'E')
#define OC_decr                         ((unsigned char)'F')
#define OC_minus                        ((unsigned char)'G')
#define OC_plus                         ((unsigned char)'H')
#define OC_amper                        ((unsigned char)'I')
#define OC_ptrmem                       ((unsigned char)'J')
#define OC_divide                       ((unsigned char)'K')
#define OC_modulo                       ((unsigned char)'L')
#define OC_less                         ((unsigned char)'M')
#define OC_leq                          ((unsigned char)'N')
#define OC_greater                      ((unsigned char)'O')
#define OC_geq                          ((unsigned char)'P')
#define OC_comma                        ((unsigned char)'Q')
#define OC_call                         ((unsigned char)'R')
#define OC_compl                        ((unsigned char)'S')
#define OC_xor                          ((unsigned char)'T')
#define OC_or                           ((unsigned char)'U')
#define OC_land                         ((unsigned char)'V')
#define OC_lor                          ((unsigned char)'W')
#define OC_asmul                        ((unsigned char)'X')
#define OC_asadd                        ((unsigned char)'Y')
#define OC_assub                        ((unsigned char)'Z')
#define OC_asdiv                        ((unsigned char)'0')
#define OC_asmod                        ((unsigned char)'1')
#define OC_asrshift                     ((unsigned char)'2')
#define OC_aslshift                     ((unsigned char)'3')
#define OC_asand                        ((unsigned char)'4')
#define OC_asor                         ((unsigned char)'5')
#define OC_asxor                        ((unsigned char)'6')
#define OC_vftable                      ((unsigned char)'7')
#define OC_vbtable                      ((unsigned char)'8')
#define OC_vcall                        ((unsigned char)'9')
#define OC_metatype                     ((unsigned char)'A')
#define OC_guard                        ((unsigned char)'B')
#define OC_string                       ((unsigned char)'C')
#define OC_vbdtor                       ((unsigned char)'D')
#define OC_vdeldtor                     ((unsigned char)'E')
#define OC_defctor                      ((unsigned char)'F')
#define OC_sdeldtor                     ((unsigned char)'G')
#define OC_vctor                        ((unsigned char)'H')
#define OC_vdtor                        ((unsigned char)'I')
#define OC_vallctor                     ((unsigned char)'J')
#define OC_vdispmap                     ((unsigned char)'K')
#define OC_ehvctor                      ((unsigned char)'L')
#define OC_ehvdtor                      ((unsigned char)'M')
#define OC_ehvctorvb                    ((unsigned char)'N')
#define OC_copyctorclosure              ((unsigned char)'O')
#define OC_udtthunk                     ((unsigned char)'P')    //extended code for all thunks for udt returns
#define OC_eh_init                      ((unsigned char)'Q')    //extended code for eh initialized structs
#define OC_rtti_init                    ((unsigned char)'R')    //extended code for rtti initialized structs
#define OC_locvftable                   ((unsigned char)'S')
#define OC_locvfctorclosure             ((unsigned char)'T')
#define OC_arrayNew                     ((unsigned char)'U')    // operator new[]
#define OC_arrayDelete                  ((unsigned char)'V')    // operator delete[]
#define OC_omni_callsig_init            ((unsigned char)'W')    // Special reserved value
#define OC_placementDeleteClosure       ((unsigned char)'X')
#define OC_placementArrayDeleteClosure  ((unsigned char)'Y')

#define OC_anonymousNamespace           ((unsigned char)'0')

// Values for for 'eh initializers'

#define OC_eh_PMD                       ((unsigned char)'0')
#define OC_eh_CT                        ((unsigned char)'1')
#define OC_eh_CTA                       ((unsigned char)'2')
#define OC_eh_TI                        ((unsigned char)'3')

// Values for for 'rtti initializers'

#define OC_rtti_TD                      ((unsigned char)'0')
#define OC_rtti_BCD                     ((unsigned char)'1')
#define OC_rtti_BCA                     ((unsigned char)'2')
#define OC_rtti_CHD                     ((unsigned char)'3')
#define OC_rtti_COL                     ((unsigned char)'4')

// Managed vector constructor|destructor iterators

#define OC_man_vec_ctor                 ((unsigned char)'A')
#define OC_man_vec_dtor                 ((unsigned char)'B')

#define OC_ehvcctor                     ((unsigned char)'C')
#define OC_ehvcctorvb                   ((unsigned char)'D')

// Dynamic init/dtor of statics

#define OC_staticinit                   ((unsigned char)'E')
#define OC_staticatexit                 ((unsigned char)'F')

// vector copy-constructor iterators

#define OC_vec_copy_ctor                ((unsigned char)'G')
#define OC_vec_copy_ctor_vb             ((unsigned char)'H')
#define OC_man_vec_copy_ctor            ((unsigned char)'I')

// guard for static __declspec(thread) in extern inline function

#define OC_thread_guard                 ((unsigned char)'J')

#define OC_await                        ((unsigned char)'L')
#define OC_spaceship                    ((unsigned char)'M')
#define OC_udl                          ((unsigned char)'K')
#define OC_NTTP_class_type              ((unsigned char)'N')

// Values for the 'type-encoding'

#define TE_isbadtype(t)         ((t)==0xFFFF)
#define TE_setisbadtype(t)      ((t)=0xFFFF)

#define TE_istruncated(t)       ((t)==0xFFFE)
#define TE_setistruncated(t)    ((t)=0xFFFE)

#define TE_isCident(t)          ((t)==0xFFFD)
#define TE_setisCident(t)       ((t)=0xFFFD)

#define TE_isfunction(t)        ((t)&0x8000)
#define TE_setisfunction(t)     ((t)|=0x8000)

#define TE_isbased(t)           ((t)&0x4000)
#define TE_setisbased(t)        ((t)|=0x4000)

#define TE_isnear(t)            (((t)&0x2000)==0x0000)
#define TE_setisnear(t)         ((t)&=~0x2000)

#define TE_isfar(t)             ((t)&0x2000)
#define TE_setisfar(t)          ((t)|=0x2000)

#define TE_isexternc(t)         ((t)&0x10000)
#define TE_setisisexternc(t)    ((t)|=0x10000)

#define TE_ismember(t)          (TE_isfunction(t)?(((t)&0x1800)==0x0800):(((t)&0x6000)==0x0000))
#define TE_setismember(t)       (TE_isfunction(t)?((t)=(((t)&~0x1800)|0x0800)):((t)=((t)&~0x6000)))

#define TE_isvirtual(t)         (TE_isfunction(t)&&(((t)&0x0700)==0x0100))
#define TE_setisvirtual(t)      ((t)=(((t)&~0x0700)|0x0100))

#define TE_isthunk(t)           (TE_ismember(t)?((t)&0x0400):((t)&0x1000))

#define TE_isvcall(t)           (TE_isfunction(t)?(((t)&0x1800)==0x1800):0)
#define TE_setisvcall(t)        ((t)=(((t)&~0x9800)|0x9800))

#define TE_islocaldtor(t)       (TE_isfunction(t)?(((t)&0x1b00)==0x1000):0)
#define TE_setislocaldtor(t)    ((t)=(((t)&~0x9b00)|0x9000))

#define TE_istemplatector(t)    (TE_isfunction(t)?(((t)&0x1b00)==0x1100):0)
#define TE_setistemplatector(t) ((t)=(((t)&~0x9b00)|0x9100))

#define TE_istemplatedtor(t)    (TE_isfunction(t)?(((t)&0x1b00)==0x1200):0)
#define TE_setistemplatedtor(t) ((t)=(((t)&~0x9b00)|0x9200))

#define TE_isadjustor(t)        (TE_ismember(t)&&(((t)&0x0700)==0x0400))
#define TE_setisadjustor(t)     ((t)=(((t)&~0x0700)|0x0400))

#define TE_isvtoradj(t)         (TE_ismember(t)&&(((t)&0x0700)==0x0500))
#define TE_setisvtoradj(t)      ((t)=(((t)&~0x0700)|0x0500))

#define TE_isvtoradjex(t)       (TE_ismember(t)&&(((t)&0x0700)==0x0600))
#define TE_setisvtoradjex(t)    ((t)=(((t)&~0x0700)|0x0600))

#define TE_ispublic(t)          (TE_ismember(t)&&(TE_isfunction(t)?(((t)&0x00C0)==0x0000):(((t)&0x1800)==0x0000)))
#define TE_setispublic(t)       ((t)=(TE_isfunction(t)?((t)&~0x00C0):((t)&~0x1800)))

#define TE_isprivate(t)         (TE_ismember(t)&&(TE_isfunction(t)?(((t)&0x00C0)==0x0040):(((t)&0x1800)==0x0800)))
#define TE_setisprivate(t)      ((t)=(TE_isfunction(t)?(((t)&~0x00C0)|0x0040):(((t)&~0x1800)|0x0800)))

#define TE_isprotected(t)       (TE_ismember(t)&&(TE_isfunction(t)?(((t)&0x00C0)==0x0080):(((t)&0x1800)==0x1000)))
#define TE_setisprotected(t)    ((t)=(TE_isfunction(t)?(((t)&~0x00C0)|0x0080):(((t)&~0x1800)|0x1000)))

#define TE_isstatic(t)          (TE_ismember(t)&&(TE_isfunction(t)?(((t)&0x0700)==0x0200):1))
#define TE_setisstatic(t)       ((t)=(TE_isfunction(t)?(((t)&~0x0700)|0x0200):TE_setismember(t)))


#define TE_isdata(t)            (!TE_isfunction(t))
#define TE_setisdata(t)         ((t)&=~0x8000)

#define TE_isguard(t)           (TE_isdata(t)&&(((t)&0x7C00)==0x6000))
#define TE_setisguard(t)        ((t)=(((t)&~0x7C00)|0x6000))

#define TE_isvftable(t)         (TE_isdata(t)&&(((t)&0x7C00)==0x6800))
#define TE_setisvftable(t)      ((t)=(((t)&~0x7C00)|0x6800))

#define TE_isvbtable(t)         (TE_isdata(t)&&(((t)&0x7C00)==0x7000))
#define TE_setisvbtable(t)      ((t)=(((t)&~0x7C00)|0x7000))

#define TE_ismetaclass(t)       (TE_isdata(t)&&(((t)&0x7C00)==0x7800))
#define TE_setismetaclass(t)    ((t)=(((t)&~0x7C00)|0x7800))

#define TE_isvdispmap(t)        (TE_isdata(t)&&(((t)&0x7C00)==0x7C00))
#define TE_setvdispmap(t)       ((t)=(((t)&~0x7C00)|0x7C00))

#define TE_islocal(t)           (TE_isdata(t)&&(((t)&0x6000)==0x2000))
#define TE_setislocal(t)        ((t)=(((t)&~0x6000)|0x2000))

#define TE_isglobal(t)          (TE_isdata(t)&&(((t)&0x6000)==0x4000))
#define TE_setisglobal(t)       ((t)=(((t)&~0x6000)|0x4000))

#define TE_structured_binding           ';'
#define TE_setisstructuredbinding(t)    ((t)=(((t)&~0x7C00)|0x6C00))

#define TE_member       ((unsigned char)0)
#define TE_static       ((unsigned char)2)
#define TE_virtual      ((unsigned char)4)
#define TE_adjustor     ((unsigned char)6)
#define TE_external     ((unsigned char)24)

#define TE_far          ((unsigned char)0x01)

#define TE_private      ((unsigned char)0x00)
#define TE_protect      ((unsigned char)0x08)
#define TE_public       ((unsigned char)0x10)

#define TE_access       (TE_protect|TE_public)

#define TE_private_d    ((unsigned char)0x00)
#define TE_protect_d    ((unsigned char)0x01)
#define TE_public_d     ((unsigned char)0x02)

#define TE_access_d     (TE_protect_d|TE_public_d)

#define TE_private_vadj ((unsigned char)0x00)
#define TE_protect_vadj ((unsigned char)0x02)
#define TE_public_vadj  ((unsigned char)0x04)

#define TE_access_vadj  (TE_protect_vadj|TE_public_vadj)

#define TE_static_d     ((unsigned char)'0')
#define TE_global       ((unsigned char)'3')
#define TE_local        ((unsigned char)'4')
#define TE_guard        ((unsigned char)'5')
#define TE_vftable      ((unsigned char)'6')
#define TE_vbtable      ((unsigned char)'7')
#define TE_metatype     ((unsigned char)'8')
#define TE_vcall        ((unsigned char)'9')
#define TE_clinkage     ((unsigned char)'9')

// Values for the 'data-indirect-type'

#define DIT_near        ((unsigned char)0x00)
#define DIT_far         ((unsigned char)0x04)
#define DIT_huge        ((unsigned char)0x08)
#define DIT_based       ((unsigned char)0x0C)
#define DIT_modelmask   ((unsigned char)0x0C)

#define DIT_const       ((unsigned char)0x01)
#define DIT_volatile    ((unsigned char)0x02)

#define DIT_member      ((unsigned char)0x10)

#define DIT_ptr64       ((unsigned char)0x04)
#define DIT_unaligned   ((unsigned char)0x05)

#define DIT_lref        ((unsigned char)0x06)
#define DIT_rref        ((unsigned char)0x07)

#define DIT_restrict    ((unsigned char)0x08)

// Values for the 'calling-convention'

#define CC_cdecl        ((unsigned char)0x00)
#define CC_pascal       ((unsigned char)0x02)
#define CC_thiscall     ((unsigned char)0x04)
#define CC_stdcall      ((unsigned char)0x06)
#define CC_fastcall     ((unsigned char)0x08)
#define CC_interrupt    ((unsigned char)0x0A)
#define CC_cocall       ((unsigned char)0x0C)
#define CC_eabi         ((unsigned char)0x0E)
#define CC_vectorcall   ((unsigned char)0x10)
#define CC_swift_1      ((unsigned char)0x12)
#define CC_swift_2      ((unsigned char)0x14)
#define CC_swift_3      ((unsigned char)0x16)
#define CC_last         CC_eabi

#define CC_saveregs     ((unsigned char)0x01)


// Values for the 'reference-data-type'

#define RDT_array   'Y'


// Values for the 'pointer-data-type'

#define PoDT_void   'X'


// Values for the 'primary-data-type'

#define PDT_reference           'A'
#define PDT_volatileReference   'B'
#define PDT_extend              '$'
#define PDT_ex_function         'A'
#define PDT_ex_other            'B'
#define PDT_ex_qualified        'C'
#define PDT_ex_ellipsis         'D'
// Avoid 'E' ... 'P' as they are used to encode managed-ness using a similar pattern
#define PDT_ex_rvalue_ref       'Q'
#define PDT_ex_vol_rvalue_ref   'R'
#define PDT_ex_nullptr          'S'
#define PDT_ex_nullptr_t        'T'

#define PDT_placeHolder         'U'
#define PDT_empty               'V'
#define PDT_packExpansion       'W'
#define PDT_terminator          'Z'
#define PDT_aliasTemplate       'Y'

// Values for com+ pointer types __gc and __pin

#define DIT_GCPointer           'A'
#define DIT_PinPointer          'B'
#define DIT_InteriorPointer     'C'

// Values for the 'basic-data-type'

#define BDT_unsigned    0x01

#define BDT_const       0x01
#define BDT_volatile    0x02

#define BDT_schar           'C'
#define BDT_char            'D'
#define BDT_char8_t         'Q'
#define BDT_char16_t        'S'
#define BDT_char32_t        'U'
#define BDT_short           'F'
#define BDT_int             'H'
#define BDT_long            'J'
#define BDT_segment         'L'
#define BDT_float           'M'
#define BDT_double          'N'
#define BDT_longdouble      'O'
#define BDT_pointer         'P'
#define BDT_union           'T'
#define BDT_struct          'U'
#define BDT_class           'V'
#define BDT_enum            'W'
#define BDT_void            'X'
#define BDT_auto            'P'
#define BDT_decltypeAuto    'T'
#define BDT_explicit_object 'V'

// Adding big ints overflows the available BDT_* codes, so we extend to
// 2 char codes, with a '_' as a lead in char
#define BDT_extend          '_'     //1st char of 2 char codes

#define BDT_int8            'D'
#define BDT_int16           'F'
#define BDT_int32           'H'
#define BDT_int64           'J'
#define BDT_int128          'L'
#define BDT_bool            'N'
#define BDT_unknown         'R'
#define BDT_array           'O'
#define BDT_wchar_t         'W'
#if CC_COR
#define BDT_coclass         'X'
#define BDT_cointerface     'Y'
#endif // CC_COR

#define BDT_boxed           'Z'

// Values for the 'enum-type'

#define ET_schar            '0'
#define ET_uchar            '1'
#define ET_sshort           '2'
#define ET_ushort           '3'
#define ET_sint             '4'
#define ET_uint             '5'
#define ET_slong            '6'
#define ET_ulong            '7'


// Values for the 'function-type-encoding'

#define FT_noexcept         'E'


// Values for the 'function-indirect-type'

#define FIT_near            0x00
#define FIT_far             0x01
#define FIT_member          0x02
#define FIT_based           0x04


//    Values for the 'function-restriction-modifiers'
#if CC_RESTRICTION_SPEC
#define RST_cpu             0x01
#if CC_DP_CXX
#define RST_amp             0x02
#endif // CC_DP_CXX
#define RST_MASK            0x03
#endif // CC_RESTRICTION_SPEC

// Values for the 'function-indirect-type'

#define AT_void             'X'
#define AT_ellipsis         'Z'
#define AT_endoflist        '@'


// Values for 'indirect-type'

#define IT_isfunction(it)   ((((it)>='6')&&((it)<='9'))||((it)=='_'))


// Values for the 'based-type'

#define BT_void         '0'
#define BT_self         '1'
#define BT_nearptr      '2'
#define BT_farptr       '3'
#define BT_hugeptr      '4'
#define BT_basedptr     '5'
#define BT_segment      '6'
#define BT_segname      '7'
#define BT_segaddr      '8'


// Values for the 'based-type'

//#define TT_illegal    '@'        // An illegal value to keep the algorithm happy


// Value for the 'vcall-model-type'

#define VMT_nTnCnV      'A'
#define VMT_nTfCnV      'B'
#define VMT_fTnCnV      'C'
#define VMT_fTfCnV      'D'
#define VMT_nTnCfV      'E'
#define VMT_nTfCfV      'F'
#define VMT_fTnCfV      'G'
#define VMT_fTfCfV      'H'
#define VMT_nTnCbV      'I'
#define VMT_nTfCbV      'J'
#define VMT_fTnCbV      'K'
#define VMT_fTfCbV      'L'


// Values for the 'letter' and 'number' ArturL: can we make it a map?

#define isValidLetter(n)    (((n)=='_')||((n)=='$')||((n)=='<')||((n)=='>')||((n)=='-')||(((n)>='a')&&((n)<='z'))||(((n)>='A')&&((n)<='Z')))
#define isValidNumber(n)    (((n)>='0')&&((n)<='9'))
#define isHighANSI(n)       ((unsigned char)(n)>=0x80 && (unsigned char)(n)<=0xFE)
#define isValidIdentChar(n) (isValidLetter(n)||isValidNumber(n)||isHighANSI(n))

// Values for the 'template-constant'

#define TC_nullptr                      '@'

#define TC_integral                     '0'
#define TC_address                      '1'
#define TC_fp                           '2'
#define TC_fp_dot                       'A'
#define TC_fp_minus                     'B'
#define TC_fp_plus                      'C'
#define TC_dummy                        'D'
#define TC_name                         'E'
#define TC_vptmd                        'F'
#define TC_gptmd                        'G'
#define TC_mptmf                        'H'
#define TC_vptmf                        'I'
#define TC_gptmf                        'J'
#define TC_auto                         'M'
#define TC_nontype_dummy                'Q'
#define TC_named_dummy                  'R'
#define TC_empty                        'S'

#define TC_value_object                 '2'
#define TC_array_object                 '3'
#define TC_string_object                '4'
#define TC_address_of                   '5'
#define TC_member_access                '6'
#define TC_union_object                 '7'
#define TC_pointer_to_member            '8'

#define TC_float                        'A'
#define TC_double                       'B'
#define TC_array_access                 'C'

#define TC_null_pointer_to_member       'N'
#define TC_lambda                       'P'


#define TC_template_type_parameter      'R'
#define TC_template_type_parameter_pack 'T'
#define TC_generic_class_parameter      'U'
#define TC_generic_method_parameter     'V'

//
// Encoding for special help function
//
#define SHF_localdtor                       'A'
#define SHF_vcall                           'B'
#define SHF_vdispmap                        'C'
#define SHF_templateStaticDataMemberCtor    'D'
#define SHF_templateStaticDataMemberDtor    'E'
#define SHF_CPPManagedILFunction            'F'
#define SHF_CPPManagedILMain                'H'
#define SHF_CManagedILFunction              'J'
#define SHF_CPPManagedILDLLImportData       'L'
#define SHF_CPPManagedNativeDLLImportData   'M'
#define SHF_CManagedILDLLImportData         'N'
#define SHF_CManagedNativeDLLImportData     'O'
#define SHF_AnyDLLImportMethod              'P'

//
// Special value for per-appdomain globals
//
#define MGD_AppDomain       'Q'

#define SHF_VtorDispThunkEx 'R'

#define SHF_Hybrid          'h'
