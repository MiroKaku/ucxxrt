// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <exception>
#include <new>

//#include "awint.hpp"

namespace Concurrency {
    namespace details {
        class __declspec(novtable) stl_critical_section_interface {
        public:
            virtual void lock()                     = 0;
            virtual bool try_lock()                 = 0;
            virtual bool try_lock_for(unsigned int) = 0;
            virtual void unlock()                   = 0;
            virtual void destroy()                  = 0;
        };

        class __declspec(novtable) stl_condition_variable_interface {
        public:
            virtual void wait(stl_critical_section_interface*)                   = 0;
            virtual bool wait_for(stl_critical_section_interface*, unsigned int) = 0;
            virtual void notify_one()                                            = 0;
            virtual void notify_all()                                            = 0;
            virtual void destroy()                                               = 0;
        };

        class stl_critical_section_km final : public stl_critical_section_interface {
        public:
            stl_critical_section_km() {
                ExInitializeFastMutex(&m_lock);
            }

            ~stl_critical_section_km()                                         = delete;
            stl_critical_section_km(const stl_critical_section_km&)            = delete;
            stl_critical_section_km& operator=(const stl_critical_section_km&) = delete;

            void destroy() override {}

            void lock() override {
                ExAcquireFastMutex(&m_lock);
            }

            bool try_lock() override {
                return ExTryToAcquireFastMutex(&m_lock) != 0;
            }

            bool try_lock_for(unsigned int) override {
                // STL will call try_lock_for once again if this call will not succeed
                return stl_critical_section_km::try_lock();
            }

            void unlock() override {
                ExReleaseFastMutex(&m_lock);
            }

            PFAST_MUTEX native_handle() {
                return &m_lock;
            }

        private:
            FAST_MUTEX m_lock{};
        };

        class stl_condition_variable_km final : public stl_condition_variable_interface {
        public:
            stl_condition_variable_km() {
                KeInitializeSemaphore(&m_semaphore, 0, LONG_MAX);
            }

            ~stl_condition_variable_km()                                           = delete;
            stl_condition_variable_km(const stl_condition_variable_km&)            = delete;
            stl_condition_variable_km& operator=(const stl_condition_variable_km&) = delete;

            void destroy() override {}

            void wait(stl_critical_section_interface* lock) override {
                if (!stl_condition_variable_km::wait_for(lock, INFINITE)) {
                    std::terminate();
                }
            }

            bool wait_for(stl_critical_section_interface* lock, unsigned int timeout) override {

                LARGE_INTEGER wait_time;
                wait_time.QuadPart = Int32x32To64(timeout, -10000);

                lock->unlock();
                {
                    if (InterlockedIncrement(&m_wait_count) > 0) {

                        const auto status = KeWaitForSingleObject(&m_semaphore, Executive,
                            KernelMode, FALSE, timeout == INFINITE ? nullptr : &wait_time);
                        if (status != STATUS_SUCCESS) {
                            _set_errno(status);
                            return false;
                        }
                    }
                }
                lock->lock();
                return true;
            }

            void notify_one() override {
                if (InterlockedCompareExchange(&m_wait_count, 0, 0) > 0) {
                    const long count = InterlockedDecrement(&m_wait_count);
                    if (count >= 0) {
                        (void)KeReleaseSemaphore(&m_semaphore, SEMAPHORE_INCREMENT, 1, false);
                    }
                }
            }

            void notify_all() override {
                if (InterlockedCompareExchange(&m_wait_count, 0, 0) > 0) {
                    long count;

                    do {
                        count = InterlockedDecrement(&m_wait_count);
                        if (count >= 0) {
                            (void)KeReleaseSemaphore(&m_semaphore, SEMAPHORE_INCREMENT, 1, false);
                        }
                    } while (count);
                }
            }

        private:
            KSEMAPHORE    m_semaphore{};
            volatile long m_wait_count = 0l;
        };

        inline void create_stl_critical_section(stl_critical_section_interface* p) {
            new (p) stl_critical_section_km;
        }

        inline void create_stl_condition_variable(stl_condition_variable_interface* p) {
            new (p) stl_condition_variable_km;
        }

        const size_t stl_critical_section_max_size        = sizeof(stl_critical_section_km);
        const size_t stl_condition_variable_max_size      = sizeof(stl_condition_variable_km);

        // concrt, vista, and win7 alignments are all identical to alignof(void*)
        const size_t stl_critical_section_max_alignment   = alignof(stl_critical_section_km);
        const size_t stl_condition_variable_max_alignment = alignof(stl_condition_variable_km);
    } // namespace details
} // namespace Concurrency
