#ifndef __FOO_LOWLEVEL_MEMORY_H__
#define __FOO_LOWLEVEL_MEMORY_H__

#include "base.h"

//# Context: Base/Memory
//# Req_Namespace: NFoo
//# Req_Header: base/memory.h
//# Req_Library: foobase.lib

namespace NFoo
{
	//# Function: MemoryAllocate
	//# Description: Allocates memory.
	//# Parameters:
	//#		Size, Number of bytes to allocate.
	//# Returns: Returns a pointer to the allocated block. Returns NULL if there was an error like out of memory.
	//# Declaration:
	void *MemoryAllocate(uint32_t _Size);

	//# Function: MemoryFree
	//# Description: Frees allocated memory.
	//# Parameters:
	//#		Ptr, Pointer to the block to free.
	//# Declaration:
	void MemoryFree(void *_pPtr);
	
	//# Function: MemoryCopy
	//# Description: Frees allocated memory.
	//# Parameters:
	//#		Dest, Pointer to the block to free.
	//#		Src, Pointer to the block to free.
	//#		Size, Pointer to the block to free.
	//# Declaration:
	void MemoryCopy(void *_pDest, const void *_pSrc, intr _Size);

	//# Function: MemoryCompare
	//# Description: Allocates memory.
	//# Parameters:
	//#		Size, Number of bytes to allocate.
	//# Returns:
	//# Declaration:
	intr MemoryCompare(void *_pDest, const void *_pSrc, intr _Size);

	//# Function: MemorySet
	//# Description: Allocates memory.
	//# Parameters:
	//#		Size, Number of bytes to allocate.
	//# Declaration:
	void MemorySet(void *_pDest, uint8_t _Value, intr _Size);

	inline void MemoryZero(void *_pDest, intr _Size) { MemorySet(_pDest, 0, _Size); }
	template<class T> inline void MemoryZero(T *_pDest) { MemoryZero(_pDest, sizeof(T)); }
}

#endif // __FOO_LOWLEVEL_MEMORY_H__

