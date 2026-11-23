// Part of foolib. Check docs/license.txt for licensing information.

#ifndef __FOO_DATATYPES_H__
#define __FOO_DATATYPES_H__

// typedef?
typedef unsigned int uintr;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int intr;
typedef char int8;
typedef short int16;
typedef int int32;

#if defined(FOO_COMPILER_INT64_INT64)
	typedef __int64 int64;
	typedef unsigned __int64 uint64;
#elif defined(FOO_COMPILER_INT64_LONGLONG)
	typedef long long int64;
	typedef unsigned long long uint64;
#elif defined(FOO_COMPILER_INT64_LONGLONG_EXT)
	__extension__ typedef long long int64;
	__extension__ typedef unsigned long long uint64;
#else
	#error "FOO ERROR: No FOO_COMPILER_INT64_* define found"
#endif

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
