#if defined(FOO_PLATFORM_WIN32) || defined(FOO_PLATFORM_WIN32CONSOLE) || defined(FOO_PLATFORM_XBOX)

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN
#include <windows.h> // CRAP
#include <process.h>
#undef MessageBox

#include "sys.h"

using namespace NFoo;

int64_t NFoo::GetTickFrequency()
{
	int64_t Freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&Freq);
	return Freq;
}

int64_t NFoo::GetTickCount()
{
	int64_t Count;
	QueryPerformanceCounter((LARGE_INTEGER*)&Count);
	return Count;
}

void NFoo::MessageBox(const char *_pMsg,const char *_pCaption)
{
	::MessageBoxA(0,_pMsg,_pCaption,MB_ICONINFORMATION);
}

void NFoo::Sleep(int32_t _Milliseconds)
{
	::Sleep(_Milliseconds);
}

void NFoo::DebugBreak()
{
	#if defined(FOO_COMPILER_MSVC7) || defined(FOO_COMPILER_MSVC6)
		__asm int 3;
	#endif
}

//
CLock::CLock()
{
	CRITICAL_SECTION *pSection = new CRITICAL_SECTION;
	::InitializeCriticalSection(pSection);
	m_pPrivateData = pSection;
}

CLock::~CLock()
{
	::DeleteCriticalSection((CRITICAL_SECTION*)m_pPrivateData);
	CRITICAL_SECTION *pSection = (CRITICAL_SECTION *)m_pPrivateData;
	m_pPrivateData = 0;
	delete pSection;
}

void CLock::Aquire()
{
	if(m_pPrivateData)
		::EnterCriticalSection((CRITICAL_SECTION*)m_pPrivateData);
}

void CLock::Release()
{
	if(m_pPrivateData)
		::LeaveCriticalSection((CRITICAL_SECTION*)m_pPrivateData);
}

bool NFoo::CreateThread(void (*_pfn)(void*), void *_Arg)
{
#ifdef FOO_COMPILER_MSVC7
	_beginthread(_pfn, 0, _Arg);
#endif
	return true;
}


#endif



