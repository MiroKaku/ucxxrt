/***
*wcstombs.cpp - Convert wide char string to multibyte char string.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a wide char string into the equivalent multibyte char string.
*
*******************************************************************************/
#include <corecrt_internal_mbstring.h>
#include <corecrt_internal_ptd_propagation.h>
#include <corecrt_internal_securecrt.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>


extern "C" errno_t __cdecl wcstombs_s(
    size_t * pConvertedChars,
    char* mbstr,
    size_t sizeInBytes,
    const wchar_t* wcstr,
    size_t count
)
{
    errno_t result = 0;

    do {

        if (wcstr == nullptr) {
            result = EINVAL;
            break;
        }

        if (mbstr == nullptr && sizeInBytes > 0) {
            result = EINVAL;
            break;
        }

        if (mbstr != nullptr && sizeInBytes == 0) {
            result = EINVAL;
            break;
        }

        if (count == _TRUNCATE) {
            count = sizeInBytes - 1;
        }

        if (sizeInBytes < (count + 1)) {
            result = ERANGE;
            break;
        }

        auto size = wcstombs(mbstr, wcstr, count);
        if (size == static_cast<size_t>(-1)) {
            size = 0;
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
