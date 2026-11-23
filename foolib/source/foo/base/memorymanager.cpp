#include <stdlib.h> // atexit()

#include "string.h"
#include "memory.h"
#include "base.h"
#include "log.h"


using namespace NFoo;
CLock NFoo::NMemoryManager::m_Lock;

#ifdef FOO_MEMORYMANAGER_ENABLED

#undef new
//
// MEMORY HANDLER
//
void operator delete[](void *_pPtr) { NFoo::NMemoryManager::Free(_pPtr); }
void operator delete(void *_pPtr) { NFoo::NMemoryManager::Free(_pPtr); }
void operator delete[](void *_pPtr, const int8 *_pFilename, int32 _Line) { NFoo::NMemoryManager::Free(_pPtr); }
void operator delete(void *_pPtr, const int8 *_pFilename, int32 _Line) { NFoo::NMemoryManager::Free(_pPtr); }

//

void *operator new[](NFoo::NMemoryManager::memsize _Size) { return NFoo::NMemoryManager::Allocate(_Size); }
void *operator new(NFoo::NMemoryManager::memsize _Size) { return NFoo::NMemoryManager::Allocate(_Size); }

void *operator new[](NFoo::NMemoryManager::memsize _Size, const int8 *_pFilename, int32 _Line)
{ return NFoo::NMemoryManager::Allocate(_Size, _pFilename, _Line); }

void *operator new(NFoo::NMemoryManager::memsize _Size, const int8 *_pFilename, int32 _Line)
{ return NFoo::NMemoryManager::Allocate(_Size, _pFilename, _Line); }

#endif


int32 g_AllocatedBlocks = 0;
int32 g_AllocatedSize = 0;
int32 g_Inited = 0;
int32 g_MemPeak = 0;
int32 g_LargestBlock = 0;
bool g_Trace = false;

NMemoryManager::CStats g_Stats;

struct SBlockHeader
{
	uint32 m_Size;
	bool m_Deallocated;
	int8 m_aFilename[256];
	int32 m_Line;
	SBlockHeader *m_pPrev;
	SBlockHeader *m_pNext;
};

SBlockHeader *m_pFirst = NULL;

void Add(SBlockHeader *_pBlock)
{
	_pBlock->m_pPrev = NULL;
	_pBlock->m_pNext = m_pFirst;
	if(m_pFirst)
		m_pFirst->m_pPrev = _pBlock;
	m_pFirst = _pBlock;
}

void Remove(SBlockHeader *_pBlock)
{
	if(_pBlock->m_pPrev)
		_pBlock->m_pPrev->m_pNext = _pBlock->m_pNext;
	else
		m_pFirst = _pBlock->m_pNext;
	
	if(_pBlock->m_pNext)
		_pBlock->m_pNext->m_pPrev = _pBlock->m_pPrev;
}

enum
{
	EHeaderSize=sizeof(SBlockHeader),
	EGuardSize=4,
	EGuardByte=0xFE
};

//
//
//
void NMemoryManager::DumpMemInfo()
{
	CAutoLock Lock(m_Lock);
	NLog::WriteF("\nLeaked memory: %d KiB, %d blocks\n", g_AllocatedSize/1024, g_AllocatedBlocks);
	if(!g_AllocatedBlocks)
		return;

	Clean();

	NLog::WriteF("Dumping blocks to logger\n");
	SBlockHeader *pBlock = m_pFirst;
	while(pBlock)
	{
		int8 aBuf[1024];
		StringCFormat(aBuf, "%s(%d)", pBlock->m_aFilename, pBlock->m_Line);
		uint8 *pBlockAddr = ((uint8 *)pBlock)+EHeaderSize+EGuardSize;
		NLog::WriteF("%-80s %8x %10d B  ", aBuf, (int32)pBlockAddr, pBlock->m_Size);

		for(int32 i = 0; i < pBlock->m_Size && i < 32; i++)
		{
			if(*(pBlockAddr+i) > 30 && *(pBlockAddr+i) < 128)
				NLog::WriteF("%c", *(pBlockAddr+i));
			else
				NLog::Write(".");
		}
		NLog::Write("\n");
		pBlock =  pBlock->m_pNext;	
	}
}

void NMemoryManager::SetTrace(bool _Trace)
{
	g_Trace = _Trace;
}

//
//
//
void NMemoryManager::Init()
{
	CAutoLock Lock(m_Lock);
	atexit(DumpMemInfo);
	ClearStats();
}

//
//
//
bool NMemoryManager::CheckMemory(bool _Log)
{
	CAutoLock Lock(m_Lock);

	bool OK = true;
	uint8 aGuard[EGuardSize];
	MemorySet(aGuard, EGuardByte, EGuardSize);

	SBlockHeader *pBlock = m_pFirst;
	while(pBlock)
	{
		if(	MemoryCompare(((uint8*)pBlock)+EHeaderSize, aGuard, EGuardSize) != 0 ||
			MemoryCompare(((uint8*)pBlock)+EHeaderSize+EGuardSize+pBlock->m_Size, aGuard, EGuardSize) != 0)
		{
			if(_Log)
			{
				int8 aBuf[1024];
				StringCFormat(aBuf, "%s(%d)", pBlock->m_aFilename, pBlock->m_Line);
				NLog::WriteF("%-80s %10d KiB %10d B MEMORY DAMAGE!\n", aBuf, pBlock->m_Size/1024, pBlock->m_Size);
			}
			else
			{
				return false;
			}

			OK = false;
		}

		pBlock =  pBlock->m_pNext;
	}

	return OK;
}

//
//
//
void NMemoryManager::Clean()
{
	CAutoLock Lock(m_Lock);
	SBlockHeader *pBlock = m_pFirst;
	while(pBlock)
	{
		SBlockHeader *pNext = pBlock->m_pNext;

		if(pBlock->m_Deallocated)
		{
			Remove(pBlock);
			MemoryFree(pBlock);
		}

		pBlock = pNext;
	}
}

//
//
//
void *NMemoryManager::Allocate(int32 _Size, const int8 *_pFilename, int32 _Line)
{
	CAutoLock Lock(m_Lock);
	if(!g_Inited)
	{
		Init();
		g_Inited = 1;
	}

	// update counters
	g_AllocatedBlocks++;
	g_AllocatedSize += _Size;
	if(_Size > g_LargestBlock)
		g_LargestBlock = _Size;
	
	if(g_AllocatedSize > g_MemPeak)
		g_MemPeak = g_AllocatedSize;
	
	// allocate memory
	int32 TotalSize = _Size+EHeaderSize+EGuardSize*2;
	uint8 *pPtr = (uint8 *)MemoryAllocate(TotalSize);

	// init header
	SBlockHeader *pHeader = (SBlockHeader*)pPtr;
	pHeader->m_Size = _Size;
	pHeader->m_Line = _Line;
	pHeader->m_aFilename[0] = 0;
	pHeader->m_Deallocated = false;

	// set guard bits
	MemorySet(pPtr+EHeaderSize, EGuardByte, EGuardSize);
	MemorySet(pPtr+TotalSize-EGuardSize, EGuardByte, EGuardSize);
	
	// copy filename if we have one
	if(_pFilename)
		StringCopy(pHeader->m_aFilename, _pFilename);
		
	Add(pHeader);
	
//	Assert(NMemoryManager::CheckMemory());

	if(g_Trace)
		NLog::WriteF("%-60s(%4d) %8x Allocated %d bytes\n", _pFilename, _Line, pPtr + EHeaderSize+EGuardSize, _Size);

	g_Stats.m_Allocations++;

	return pPtr + EHeaderSize+EGuardSize;
}

void NMemoryManager::Free(void *_pPtr, const int8 *_pFilename, int32 _Line)
{
	CAutoLock Lock(m_Lock);
	if (!_pPtr)
		return;

	uint8 *pPtr = (uint8*)_pPtr - EHeaderSize - EGuardSize;
	SBlockHeader *pHeader = (SBlockHeader*)(pPtr);

	int32 Size = pHeader->m_Size;
	if(pHeader->m_Deallocated)
	{
		NLog::WriteF("WARNING: 0x%x deleted several times\n", _pPtr);
		return;
	}
	
	g_AllocatedBlocks--;
	g_AllocatedSize -= pHeader->m_Size;
	pHeader->m_Deallocated = true;
	g_Stats.m_Deallocations++;

	Remove(pHeader);
	MemoryFree(pPtr);

	if(g_Trace)
		NLog::WriteF("%-60s(%4d) %8x Freed %d bytes\n", _pFilename, _Line, _pPtr, Size);

}

void NMemoryManager::ClearStats()
{
	CAutoLock Lock(m_Lock);
	g_Stats.m_Allocations = 0;
	g_Stats.m_Deallocations = 0;
}

NMemoryManager::CStats NMemoryManager::GetStats()
{
	return g_Stats;
}


