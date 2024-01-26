// Copyright (c) Microsoft Corporation. All rights reserved.
//
// C11 atomic support routines: extern inline definitions
#include <vcruntime_c11_atomic_support.h>

extern inline void _Check_memory_order(const unsigned int _Order);
extern inline void _Atomic_thread_fence(const unsigned int _Order);

extern inline void _Atomic_lock_acquire(volatile long* _Spinlock);
extern inline void _Atomic_lock_release(volatile long* _Spinlock);

extern inline void _Atomic_signal_fence(int _Order);
extern inline _Bool _Atomic_is_lock_free(size_t _Sz);

extern inline void _Atomic_store8(volatile char* _Ptr, char _Desired, int _Order);
extern inline void _Atomic_store16(volatile short* _Ptr, short _Desired, int _Order);
extern inline void _Atomic_store32(volatile int* _Ptr, int _Desired, int _Order);
extern inline void _Atomic_store64(volatile long long* _Ptr, long long _Desired, int _Order);
extern inline void _Atomic_storef(volatile float* _Ptr, float _Desired, int _Order);
extern inline void _Atomic_stored(volatile double* _Ptr, double _Desired, int _Order);

extern inline char _Atomic_load8(const volatile char* _Ptr, int _Order);
extern inline short _Atomic_load16(const volatile short* _Ptr, int _Order);
extern inline int _Atomic_load32(const volatile int* _Ptr, int _Order);
extern inline long long _Atomic_load64(const volatile long long* _Ptr, int _Order);
extern inline float _Atomic_loadf(const volatile float* _Ptr, int _Order);
extern inline double _Atomic_loadd(const volatile double* _Ptr, int _Order);

extern inline _Bool _Atomic_compare_exchange_strong8(volatile char* _Ptr, char* _Expected, char _Desired, int _Order);
extern inline _Bool _Atomic_compare_exchange_strong16(
    volatile short* _Ptr, short* _Expected, short _Desired, int _Order);
extern inline _Bool _Atomic_compare_exchange_strong32(volatile int* _Ptr, int* _Expected, int _Desired, int _Order);
extern inline _Bool _Atomic_compare_exchange_strong64(
    volatile long long* _Ptr, long long* _Expected, long long _Desired, int _Order);
extern inline _Bool _Atomic_compare_exchange_strongf(volatile float* _Ptr, float* _Expected, float _Desired, int _Order);
extern inline _Bool _Atomic_compare_exchange_strongd(
    volatile double* _Ptr, double* _Expected, double _Desired, int _Order);

extern inline char _Atomic_exchange8(volatile char* _Ptr, int _Desired, int _Order);
extern inline short _Atomic_exchange16(volatile short* _Ptr, int _Desired, int _Order);
extern inline int _Atomic_exchange32(volatile int* _Ptr, int _Desired, int _Order);
extern inline long long _Atomic_exchange64(volatile long long* _Ptr, long long _Desired, int _Order);
extern inline float _Atomic_exchangef(volatile float* _Ptr, float _Desired, int _Order);
extern inline double _Atomic_exchanged(volatile double* _Ptr, double _Desired, int _Order);

extern inline char _Atomic_fetch_add8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_fetch_add16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_fetch_add32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_fetch_add64(volatile long long* _Ptr, long long val, int _Order);
extern inline float _Atomic_fetch_addf(volatile float* _Ptr, float val, int _Order);
extern inline double _Atomic_fetch_addd(volatile double* _Ptr, double val, int _Order);

extern inline char _Atomic_add_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_add_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_add_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_add_fetch64(volatile long long* _Ptr, long long val, int _Order);
extern inline float _Atomic_add_fetchf(volatile float* _Ptr, float val, int _Order);
extern inline double _Atomic_add_fetchd(volatile double* _Ptr, double val, int _Order);

extern inline char _Atomic_fetch_sub8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_fetch_sub16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_fetch_sub32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_fetch_sub64(volatile long long* _Ptr, long long val, int _Order);
extern inline float _Atomic_fetch_subf(volatile float* _Ptr, float val, int _Order);
extern inline double _Atomic_fetch_subd(volatile double* _Ptr, double val, int _Order);

extern inline char _Atomic_sub_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_sub_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_sub_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_sub_fetch64(volatile long long* _Ptr, long long val, int _Order);
extern inline float _Atomic_sub_fetchf(volatile float* _Ptr, float val, int _Order);
extern inline double _Atomic_sub_fetchd(volatile double* _Ptr, double val, int _Order);

extern inline char _Atomic_fetch_and8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_fetch_and16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_fetch_and32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_fetch_and64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_and_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_and_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_and_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_and_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_fetch_or8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_fetch_or16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_fetch_or32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_fetch_or64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_or_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_or_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_or_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_or_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_fetch_xor8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_fetch_xor16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_fetch_xor32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_fetch_xor64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_xor_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_xor_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_xor_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_xor_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_mult_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_mult_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_mult_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_mult_fetch64(volatile long long* _Ptr, long long val, int _Order);
extern inline float _Atomic_mult_fetchf(volatile float* _Ptr, float val, int _Order);
extern inline double _Atomic_mult_fetchd(volatile double* _Ptr, double val, int _Order);

extern inline unsigned char _Atomic_div_fetch8(volatile unsigned char* _Ptr, unsigned int val, int _Order);
extern inline unsigned short _Atomic_div_fetch16(volatile unsigned short* _Ptr, unsigned int val, int _Order);
extern inline unsigned int _Atomic_div_fetch32(volatile unsigned int* _Ptr, unsigned int val, int _Order);
extern inline unsigned long long _Atomic_div_fetch64(
    volatile unsigned long long* _Ptr, unsigned long long val, int _Order);
extern inline float _Atomic_div_fetchf(volatile float* _Ptr, float val, int _Order);
extern inline double _Atomic_div_fetchd(volatile double* _Ptr, double val, int _Order);

extern inline signed char _Atomic_idiv_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_idiv_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_idiv_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_idiv_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_shl_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_shl_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_shl_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_shl_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline char _Atomic_shr_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_shr_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_shr_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_shr_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline signed char _Atomic_imod_fetch8(volatile char* _Ptr, int val, int _Order);
extern inline short _Atomic_imod_fetch16(volatile short* _Ptr, int val, int _Order);
extern inline int _Atomic_imod_fetch32(volatile int* _Ptr, int val, int _Order);
extern inline long long _Atomic_imod_fetch64(volatile long long* _Ptr, long long val, int _Order);

extern inline unsigned char _Atomic_mod_fetch8(volatile unsigned char* _Ptr, unsigned int val, int _Order);
extern inline unsigned short _Atomic_mod_fetch16(volatile unsigned short* _Ptr, unsigned int val, int _Order);
extern inline unsigned int _Atomic_mod_fetch32(volatile unsigned int* _Ptr, unsigned int val, int _Order);
extern inline unsigned long long _Atomic_mod_fetch64(
    volatile unsigned long long* _Ptr, unsigned long long val, int _Order);

extern inline void _Atomic_lock_and_store(volatile void* _Obj, const void* _Desired, int _Offset, size_t _Size);
extern inline void _Atomic_lock_and_load(volatile void* _Obj, void* _Dest, int _Offset, size_t _Size);
extern inline void _Atomic_lock_and_exchange(
    volatile void* _Obj, const void* _Desired, void* _Dest, int _Offset, size_t _Size);
extern inline _Bool _Atomic_lock_and_compare_exchange_strong(
    volatile void* _Obj, void* _Expected, const void* _Desired, int _Offset, size_t _Size);
