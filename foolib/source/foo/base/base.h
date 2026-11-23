// Part of foolib. Check docs/license.txt for licensing information.

#ifndef __FOO_BASE_H__
#define __FOO_BASE_H__

// Compiler Settings
#if defined(FOO_COMPILER_MSVC7)
	#include "compiler_msvc7.h"
#elif defined(FOO_COMPILER_MSVC6)
	#include "compiler_msvc6.h"
#elif defined(FOO_COMPILER_GCC)
	#include "compiler_gcc.h"
#elif defined(FOO_COMPILER_CLANG)
	#include "compiler_gcc.h"
#elif defined(FOO_COMPILER_CW)
	#include "compiler_cw.h"
#elif defined(FOO_COMPILER_MW)
	#include "compiler_cw.h"
#else
	#error "FOO ERROR: No include for compiler"
#endif

// CPU settings
#if defined(FOO_CPU_X86)
	#include "cpu_x86.h"
#elif defined(FOO_CPU_PPC)
	#include "cpu_ppc.h"
#elif defined(FOO_CPU_ARM64)
	#include "cpu_arm64.h"
#else
	#error "FOO ERROR: No include for cpu"
#endif

// Platform settings
#if defined(FOO_PLATFORM_DARWIN)
	#include "platform_posix.h"
#elif defined(FOO_PLATFORM_FREEBSD)
	#include "platform_posix.h"
#elif defined(FOO_PLATFORM_OPENBSD)
	#include "platform_posix.h"
#elif defined(FOO_PLATFORM_WIN32)
	#include "platform_win32.h"
#elif defined(FOO_PLATFORM_LINUX)
	#include "platform_linux.h"
#else
	#error "FOO ERROR: No include for platform"
#endif


#include "datatypes.h"
#include "bytetools.h"
#include "memorymanager.h"
#include "commonfunctions.h" // rename?
// logger
// sys?
/*
#ifndef __MACROINLINEACCESS
#define __MACROINLINEACCESS
#define MACRO_INLINEACCESS_RW(name, type) \
	inline const type& Get##name() const { return m_##name; } \
	inline type& Get##name() { return m_##name; }

#define MACRO_INLINEACCESS_R(name, type) \
	inline const type& Get##name() const { return m_##name; }

#define MACRO_INLINEACCESS_RWEXT(name, variable, type) \
	inline const type& Get##name() const { return variable; } \
	inline type& Get##name() { return variable; }

#define MACRO_INLINEACCESS_REXT(name, variable, type) \
	inline const type& Get##name() const { return variable; }
#endif
*/
#endif // __FOO_BASE_H__

