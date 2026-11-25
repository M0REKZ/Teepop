// Part of foolib. Check docs/license.txt for licensing information.

#ifndef __FOO_DATATYPES_H__
#define __FOO_DATATYPES_H__

#include <stdint.h>

// typedef?
typedef unsigned int uintr;
//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned long uint32_t;
typedef int intr;
//typedef char int8_t;
//typedef short int16_t;
//typedef long int32_t;

/*#if defined(FOO_COMPILER_INT64_INT64)
	typedef __int64 int64_t;
	typedef unsigned __int64 uint64_t;
#elif defined(FOO_COMPILER_INT64_LONGLONG)
	typedef long long int64_t;
	typedef unsigned long long uint64_t;
#elif defined(FOO_COMPILER_INT64_LONGLONG_EXT)
	__extension__ typedef long long int64_t;
	__extension__ typedef unsigned long long uint64_t;
#else
	#error "FOO ERROR: No FOO_COMPILER_INT64_* define found"
#endif*/

// fp32, fp64
typedef float fp32;
typedef double fp64;

// pure
#define pure =0

// NULL
#ifndef NULL
	#define NULL 0
#endif

#endif
