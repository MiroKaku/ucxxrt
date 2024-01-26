#pragma once


#ifdef __cplusplus
extern "C++"
{
	extern DWORD __crt_global_state_mode_flsindex;
	namespace __crt_state_management
	{
		const size_t state_index_count = 2;

		[[maybe_unused]] static bool initialize_global_state_isolation()
		{
			__crt_global_state_mode_flsindex = __acrt_FlsAlloc(0);
			return __crt_global_state_mode_flsindex != FLS_OUT_OF_INDEXES;
		}

		[[maybe_unused]] static void uninitialize_global_state_isolation(const bool)
		{
			if (__crt_global_state_mode_flsindex != FLS_OUT_OF_INDEXES)
			{
				__acrt_FlsFree(__crt_global_state_mode_flsindex);
				__crt_global_state_mode_flsindex = FLS_OUT_OF_INDEXES;
			}
		}

		[[maybe_unused]] static size_t get_current_state_index()
		{
			DWORD dwLastError;
			size_t current_state_index;

			dwLastError = GetLastError();
			current_state_index = (size_t)__acrt_FlsGetValue(__crt_global_state_mode_flsindex);
			SetLastError(dwLastError);
			return current_state_index;
		}

		[[maybe_unused]] static size_t get_current_state_index(const __crt_scoped_get_last_error_reset&)
		{
			return (size_t)__acrt_FlsGetValue(__crt_global_state_mode_flsindex);
		}

		[[maybe_unused]] static bool is_os_call()
		{
			return get_current_state_index() == 1;
		}

		[[maybe_unused]] static void enter_os_call_inline()
		{
			__acrt_FlsSetValue(__crt_global_state_mode_flsindex, (PVOID)1);
		}

		[[maybe_unused]] static void enter_os_call()
		{
			enter_os_call_inline();
		}

		[[maybe_unused]] static void leave_os_call_inline()
		{
			__acrt_FlsSetValue(__crt_global_state_mode_flsindex, (PVOID)0);
		}

		[[maybe_unused]] static void leave_os_call()
		{
			leave_os_call_inline();
		}

		class scoped_global_state_reset
		{
			bool _was_os_call;
		public:
			scoped_global_state_reset()
			{
				this->_was_os_call = __crt_state_management::is_os_call();
				if (this->_was_os_call)
					__crt_state_management::leave_os_call();
			}
			~scoped_global_state_reset()
			{
				if (this->_was_os_call)
					__crt_state_management::enter_os_call();
			}
		};

		template <typename T>
		class dual_state_global
		{
			T _value[2];
		public:
			void initialize(T new_value)
			{
				for (T& every_value : _value)
				{
					every_value = new_value;
				}
			}

			template <typename DATA_ARRAY>
			void initialize_from_array(DATA_ARRAY data_array[2])
			{
				for (size_t i = 0; i != _countof(_value); ++i)
				{
					_value[i] = data_array[i];
				}
			}

			void uninitialize(void f(T& v))
			{
				for (T& every_value : _value)
				{
					f(every_value);
				}
			}
			T* dangerous_get_state_array()
			{
				return _value;
			}
			T& value()
			{
				return _value[__crt_state_management::get_current_state_index()];
			}
			T& value_explicit(const size_t current_global_state_index)
			{
				return _value[current_global_state_index];
			}
			T& value(__crt_cached_ptd_host& ptd) throw();
			T const& value(__crt_cached_ptd_host& ptd) const throw();
		};
	}

	[[maybe_unused]] static __acrt_lock_id __acrt_select_exit_lock()
	{
		__acrt_lock_id lock_id;

		if (__crt_state_management::is_os_call())
			lock_id = __acrt_os_exit_lock;
		else
			lock_id = __acrt_exit_lock;
		return lock_id;
	}
}
#endif