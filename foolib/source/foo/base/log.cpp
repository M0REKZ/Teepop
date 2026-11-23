// Part of foolib. Check docs/license.txt for licensing information.

#include "string.h"
#include "log.h"

using namespace NFoo;

NLog::CLogger *NLog::m_pFirst = 0;
CLock NLog::m_Lock;

//
//
//
void NLog::Write(const int8 *_pBuf)
{
	m_Lock.Aquire();
	for(CLogger *pCurrent = m_pFirst; pCurrent; pCurrent = pCurrent->m_pNext)
		pCurrent->Write(_pBuf);
	m_Lock.Release();
}

//
//
//
void NLog::WriteF(const int8 *_pFormat, ...)
{
	int8 Buffer[4092];
	va_list Args;
	va_start(Args, _pFormat);
	StringCFormatV(Buffer, _pFormat, Args);
	va_end(Args);
	Write(Buffer);
}

//
//
//
void NLog::Attach(CLogger *_pLogger)
{
	m_Lock.Aquire();
	_pLogger->m_pNext = m_pFirst;
	m_pFirst = _pLogger;
	m_Lock.Release();
}

//
//
//
void NLog::Detach(CLogger *_pLogger)
{
	m_Lock.Aquire();
	if(m_pFirst == _pLogger)
	{
		m_pFirst = _pLogger->m_pNext;
		m_Lock.Release();
		return;
	}
	
	for(CLogger *pCurrent = m_pFirst; pCurrent; pCurrent = pCurrent->m_pNext)
	{
		if(pCurrent->m_pNext == _pLogger)
		{
			pCurrent->m_pNext = _pLogger->m_pNext;
			m_Lock.Release();
			return;
		}
	}
	m_Lock.Release();
}

void Assert_(const int8 *pFile, int32 Line, bool _bVal)
{
	if (!_bVal)
	{
		static int8 aBuffer[1024];
		NFoo::NLog::WriteF("ASSERT: %s:%d\n", pFile, Line);
		NFoo::DebugBreak();
	}
}


