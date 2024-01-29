/***
*mbstowcs.cpp - Convert multibyte char string to wide char string.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a multibyte char string into the equivalent wide char string.
*
*******************************************************************************/
#include <corecrt_internal_mbstring.h>
#include <corecrt_internal_ptd_propagation.h>
#include <corecrt_internal_securecrt.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdlib.h>


extern "C" errno_t __cdecl mbstowcs_s(
    size_t * pConvertedChars,
    wchar_t* wcstr,
    size_t sizeInWords,
    const char* mbstr,
    size_t count
)
{
    errno_t result = 0;

    do {

        if (mbstr == nullptr) {
            result = EINVAL;
            break;
        }

        if (wcstr == nullptr && sizeInWords > 0) {
            result = EINVAL;
            break;
        }

        if (wcstr != nullptr && sizeInWords == 0) {
            result = EINVAL;
            break;
        }

        if (count == _TRUNCATE) {
            count = sizeInWords - 1;
        }

        if (sizeInWords < (count + 1)) {
            result = ERANGE;
            break;
        }

        auto size = mbstowcs(wcstr, mbstr, count);
        if (size == static_cast<size_t>(-1)) {
            size   = 0;
            result = EILSEQ;
        }

        if (pConvertedChars) {
            *pConvertedChars = size;
        }

    } while (false);

    if (result != 0) {
        _set_errno(result);
    }

    return result;
}
