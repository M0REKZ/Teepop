#ifdef FOO_LOWLEVEL_STRING_LIBC

#include "string.h"
#include <string.h>
#include <stdio.h>
#include <cstdarg>
#include <cstdlib>

namespace std {}
using namespace std;

void NFoo::StringCopy(char *_pDest, const char *_pSrc) { strcpy(_pDest, _pSrc); }
void NFoo::StringCopy(char *_pDest, const char *_pSrc, intr _Max) { strncpy(_pDest, _pSrc, _Max); }

intr NFoo::StringCompare(const char *_pDest, const char *_pSrc) { return strcmp(_pDest, _pSrc); }
intr NFoo::StringCompare(const char *_pDest, const char *_pSrc, intr _Max) { return strncmp(_pDest, _pSrc, _Max); }

char *NFoo::StringCFormatArgList(char *_pDst, size_t _DstSize, const char *_pFormat, va_list _Args)
{
    vsnprintf(_pDst, _DstSize, _pFormat, _Args);
    return _pDst;
}

char *NFoo::StringCFormat( char *_pDst, size_t _DstSize, const char *_pFormat, ...)
{
    va_list Args;
    va_start(Args, _pFormat);

    StringCFormatArgList(_pDst, _DstSize, _pFormat, Args);

    va_end(Args);

    return _pDst;
}

intr NFoo::StringLength(const char *_pSrc) { return strlen(_pSrc); }

int32_t NFoo::StringGetInt(const char *_pDest) { return atoi(_pDest); }
fp64 NFoo::StringGetFloat(const char *_pDest) { return atof(_pDest); }


#endif
