#ifndef __FOO_MEMROYMANAGER_H__
#define __FOO_MEMROYMANAGER_H__

#include "sys.h"

namespace NFoo
{
	namespace NMemoryManager
	{
		extern class CLock m_Lock;
		struct CStats
		{
			int32_t m_Allocations;
			int32_t m_Deallocations;
		};

		void *Allocate(int32_t _Size, const char *_pFilename=NULL, int32_t _Line=0);
		void Free(void *_pPtr, const char *_pFilename=NULL, int32_t _Line=0);

		void DumpMemInfo();
		bool CheckMemory(bool _Log=false);
		void Init();
		void Clean(); // frees all blockes that have been marked as deallocated

		void SetTrace(bool _Trace=true);

		void ClearStats();
		CStats GetStats();

	#if defined(FOO_COMPILER_MSVC7)
		typedef size_t memsize;
	#elif defined(FOO_COMPILER_GCC)
		typedef unsigned long int memsize;
	#endif
	}
}

//#if defined(FOO_COMPILER_MSVC7) && !defined(FOO_MODE_RELEASE)
//#define FOO_MEMORYMANAGER_ENABLED
//#endif
#ifdef FOO_MEMORYMANAGER_ENABLED
	//
	#undef new

	extern void operator delete[](void *_pPtr);
	extern void operator delete(void *_pPtr);
	extern void operator delete[](void *_pPtrconst, const char *_pFilename, int32_t _Line);
	extern void operator delete(void *_pPtrconst, const char *_pFilename, int32_t _Line);
	
	//
	extern void *operator new[](NFoo::NMemoryManager::memsize _Size);
	extern void *operator new(NFoo::NMemoryManager::memsize _Size);
	extern void *operator new[](NFoo::NMemoryManager::memsize _Size, const char *_pFilename, int32_t _Line);
	extern void *operator new(NFoo::NMemoryManager::memsize _Size, const char *_pFilename, int32_t _Line);
	#define new new(__FILE__, __LINE__)
#endif

#endif // __FOO_MEMROYMANAGER_H__
