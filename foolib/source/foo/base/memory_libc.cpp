#ifdef FOO_LOWLEVEL_STRING_LIBC

#ifdef FOO_PLATFORM_DARWIN
	#include <alloc.h>
#else
	#include <stdlib.h>
#endif

#include <string.h>

#include "memory.h"

namespace std {}
using namespace std;

void NFoo::MemoryCopy(void *_pDest, const void *_pSrc, intr _Size) { memcpy(_pDest, _pSrc, _Size); }
intr NFoo::MemoryCompare(void *_pDest, const void *_pSrc, intr _Size) { return memcmp(_pDest, _pSrc, _Size); }
void NFoo::MemorySet(void *_pDest, uint8 _Value, intr _Size) { memset(_pDest, _Value, _Size); }
void *NFoo::MemoryAllocate(uint32 _Size) { return malloc(_Size); }
void NFoo::MemoryFree(void *p) { free(p); }

#endif
