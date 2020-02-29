/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      pair.h
 * DATA:      2020/02/09
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once


namespace ucxxrt
{
    template<typename T1, typename T2>
    struct Pair
    {
        using first_type    = T1;
        using second_type   = T2;

        T1  first;
        T2  second;

        Pair(const Pair& p) = default;
        Pair(Pair && p)     = default;

        constexpr Pair() = default;
        constexpr Pair(const T1& x, const T2& y)
            : first(x)
            , second(y)
        {}
    };
}
