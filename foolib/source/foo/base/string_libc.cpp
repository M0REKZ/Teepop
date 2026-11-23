#ifdef FOO_LOWLEVEL_STRING_LIBC

#include "string.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

namespace std {}
using namespace std;

void NFoo::StringCopy(int8 *_pDest, const int8 *_pSrc) { strcpy(_pDest, _pSrc); }
void NFoo::StringCopy(int8 *_pDest, const int8 *_pSrc, intr _Max) { strncpy(_pDest, _pSrc, _Max); }

intr NFoo::StringCompare(const int8 *_pDest, const int8 *_pSrc) { return strcmp(_pDest, _pSrc); }
intr NFoo::StringCompare(const int8 *_pDest, const int8 *_pSrc, intr _Max) { return strncmp(_pDest, _pSrc, _Max); }

int8 *NFoo::StringCFormatV(int8 *_pDst, const int8 *_pFormat, va_list _Args)
{
	vsprintf(_pDst, _pFormat, _Args);
	return _pDst;
}

int8 *NFoo::StringCFormat(int8 *_pDst, const int8 *_pFormat, ...)
{
	va_list Args;
	va_start(Args, _pFormat);
	StringCFormatV(_pDst, _pFormat, Args);
	va_end(Args);
	return _pDst;
}

intr NFoo::StringLength(const int8 *_pSrc) { return strlen(_pSrc); }

int32 NFoo::StringGetInt(const int8 *_pDest) { return atoi(_pDest); }
fp64 NFoo::StringGetFloat(const int8 *_pDest) { return atof(_pDest); }


#endif
