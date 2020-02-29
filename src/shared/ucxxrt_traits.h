/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ucxxrt_traits.h
 * DATA:      2020/02/09
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once


namespace ucxxrt::traits
{
    //
    // Basic type
    //

    template<class T = void, T ...v>
    struct $type
    {
        using type = $type;
        using value_type = T;

        static constexpr auto size() noexcept -> size_t
        {
            return (sizeof...(v));
        }

        constexpr $type() {}
    };

    template<class T, T v>
    struct $type<T, v>
    {
        static constexpr T value = v;

        using type = $type;
        using value_type = T;

        static constexpr auto size() noexcept -> size_t
        {
            return 1;
        }

        constexpr $type() {}

        constexpr operator value_type() const noexcept
        {
            return (value);
        }

        constexpr value_type operator()() const noexcept
        {	// return stored value
            return (value);
        }

        constexpr auto operator==(const T& t) const -> bool
        {
            return v == t;
        }

        constexpr auto operator!=(const T& t) const -> bool
        {
            return v != t;
        }
    };

    template<class T, T v> bool operator==($type<T, v>, const T& t) { return v == t; }
    template<class T, T v> bool operator!=($type<T, v>, const T& t) { return v != t; }
    template<class T, T v> bool operator==(const T& t, $type<T, v>) { return v == t; }
    template<class T, T v> bool operator!=(const T& t, $type<T, v>) { return v != t; }

    template<bool  ...v> using $bool = $type<bool, v...>;

    using $true = $bool<true>;
    using $false = $bool<false>;

    template <class, class>
    constexpr bool $$is_same = false;

    template <class T> 
    constexpr bool $$is_same<T, T> = true;

    template <class T1, class T2>
    struct $is_same : $bool<$$is_same<T1, T2>> {};

}
