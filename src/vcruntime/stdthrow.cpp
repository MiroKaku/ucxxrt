/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      stdthrow.cpp
 * DATA:      2020/02/28
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 // Copyright (c) Microsoft Corporation.
 // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 //
 // exception handling support functions


#if __has_include(<wdm.h>)

namespace stdext
{
    class exception;
}

namespace std
{
    typedef void(__cdecl* _Prhand)(const stdext::exception&);
    _Prhand _Raise_handler; // pointer to raise handler
}

namespace stdext
{
    class exception { // base of all library exceptions
    public:
        static _STD _Prhand _Set_raise_handler(_STD _Prhand _Pnew) { // register a handler for _Raise calls
            const _STD _Prhand _Pold = _STD _Raise_handler;
            _STD _Raise_handler = _Pnew;
            return _Pold;
        }

        // this constructor is necessary to compile
        // successfully header new for _HAS_EXCEPTIONS==0 scenario
        explicit __CLR_OR_THIS_CALL exception(const char* _Message = "unknown", int = 1) noexcept : _Ptr(_Message) {}

        __CLR_OR_THIS_CALL exception(const exception& _Right) noexcept : _Ptr(_Right._Ptr) {}

        exception& __CLR_OR_THIS_CALL operator=(const exception& _Right) noexcept {

            _Ptr = _Right._Ptr;
            return *this;
        }

        virtual __CLR_OR_THIS_CALL ~exception() noexcept {}

        _NODISCARD virtual const char* __CLR_OR_THIS_CALL what() const noexcept { // return pointer to message string
            return _Ptr ? _Ptr : "unknown exception";
        }

        void __CLR_OR_THIS_CALL _Raise() const { // raise the exception
            if (_STD _Raise_handler) {
                (*_STD _Raise_handler)(*this); // call raise handler if present
            }

            _Doraise(); // call the protected virtual
            _RAISE(*this); // raise this exception
        }

    protected:
        virtual void __CLR_OR_THIS_CALL _Doraise() const { // perform class-specific exception handling
        }

    protected:
        const char* _Ptr; // the message pointer
    };
}

namespace std
{
#ifdef _DEBUG
    extern "C" void __cdecl _invalid_parameter(
        wchar_t const* const /*expression*/,
        wchar_t const* const /*function_name*/,
        wchar_t const* const /*file_name*/,
        unsigned int   const /*line_number*/,
        uintptr_t      const /*reserved*/)
    {
    }
#endif

    extern "C" void __cdecl _invalid_parameter_noinfo(void)
    {
    }

    extern "C" __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn(void)
    {
    }

    extern "C" __declspec(noreturn) void __cdecl _invoke_watson(
        wchar_t const* const /*expression*/,
        wchar_t const* const /*function_name*/,
        wchar_t const* const /*file_name*/,
        unsigned int   const /*line_number*/,
        uintptr_t      const /*reserved*/)
    {
    }
}

namespace std
{
    // CLASS logic_error
    class logic_error : public exception { // base of all logic-error exceptions
    public:
        using _Mybase = exception;

        explicit logic_error(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS invalid_argument
    class invalid_argument : public logic_error { // base of all invalid-argument exceptions
    public:
        using _Mybase = logic_error;

        explicit invalid_argument(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS length_error
    class length_error : public logic_error { // base of all length-error exceptions
    public:
        using _Mybase = logic_error;

        explicit length_error(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS out_of_range
    class out_of_range : public logic_error { // base of all out-of-range exceptions
    public:
        using _Mybase = logic_error;

        explicit out_of_range(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS runtime_error
    class runtime_error : public exception { // base of all runtime-error exceptions
    public:
        using _Mybase = exception;

        explicit runtime_error(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS overflow_error
    class overflow_error : public runtime_error { // base of all overflow-error exceptions
    public:
        using _Mybase = runtime_error;

        explicit overflow_error(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS underflow_error
    class underflow_error : public runtime_error { // base of all underflow-error exceptions
    public:
        using _Mybase = runtime_error;

        explicit underflow_error(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS range_error
    class range_error : public runtime_error { // base of all range-error exceptions
    public:
        using _Mybase = runtime_error;

        explicit range_error(const char* _Message) : _Mybase(_Message) {}
    };

    // CLASS bad_function_call
    class bad_function_call : public exception { // exception thrown when an empty std::function is called
    public:
        bad_function_call() noexcept {}

        _NODISCARD virtual const char* what() const noexcept override
        {
            // return pointer to message string
            return "bad function call";
        }
    };
}

namespace std
{
    [[noreturn]] void __cdecl _Xbad_alloc() { // report a bad_alloc error
        throw bad_alloc{};
    }

    [[noreturn]] void __cdecl _Xinvalid_argument(
        _In_z_ const char* _Message) { // report an invalid_argument error
        throw invalid_argument(_Message);
    }

    [[noreturn]] void __cdecl _Xlength_error(
        _In_z_ const char* _Message) { // report a length_error
        throw length_error(_Message);
    }

    [[noreturn]] void __cdecl _Xout_of_range(
        _In_z_ const char* _Message) { // report an out_of_range error
        throw out_of_range(_Message);
    }

    [[noreturn]] void __cdecl _Xoverflow_error(
        _In_z_ const char* _Message) { // report an overflow error
        throw overflow_error(_Message);
    }

    [[noreturn]] void __cdecl _Xruntime_error(
        _In_z_ const char* _Message) { // report a runtime_error
        throw runtime_error(_Message);
    }

    [[noreturn]] void __cdecl _Xbad_function_call() { // report a bad_function_call error
        throw bad_function_call{};
    }

#if __has_include(<regex>)
    [[noreturn]] void __cdecl _Xregex_error( /*regex_constants::error_type*/ unsigned int _Code) { // report a regex_error
        throw regex_error(_Code);
    }
#endif

}

#endif
