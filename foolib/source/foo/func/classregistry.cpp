#include <foo/base/log.h>
#include <foo/base/string.h>
#include "classregistry.h"

using namespace NFoo;
using namespace NFoo::NClassRegistry;

//TDynamicArray<CHandle> NClassRegistry::g_lClasses;
CHandle *NClassRegistry::m_pFirstClass = NULL;
int32_t g_CurrentID = 1;

using namespace NClassRegistry;

CHandle::CHandle(const int8 *_pName, const int8 *_pTag, CLASSCREATEFUNC _pfnCreateFunc)
{
	StringCopy(m_aName, _pName);
	StringCopy(m_aTag, _pTag);
	m_pfnCreate = _pfnCreateFunc;
	m_iClass = g_CurrentID++;
}

//
//
//
void NClassRegistry::Clear()
{
	CHandle *pHandle = m_pFirstClass;
	while(pHandle)
	{
		CHandle *pNext = pHandle->m_pNext;
		delete pHandle;
		pHandle = pNext;
	}
	m_pFirstClass = NULL;
}

//
//
//
void NClassRegistry::Register(const int8 *_pName, const int8 *_pTag, CLASSCREATEFUNC _pfnFunc)
{
	CHandle *pHandle = new CHandle(_pName, _pTag, _pfnFunc);
	pHandle->m_pNext = m_pFirstClass;
	m_pFirstClass = pHandle;
}

//
//
//
int32_t NClassRegistry::GetClassID(const int8 *_pName)
{
	CHandle *pHandle = FindHandle(_pName);
	if(!pHandle)
		return 0;
	return pHandle->m_iClass;
}

//
//
//
CHandle *NClassRegistry::FindHandle(const int8 *_pName)
{
	for(CHandle *pHandle = m_pFirstClass; pHandle; pHandle = pHandle->m_pNext)
	{
		if(StringCompare(pHandle->m_aName, _pName) == 0)
			return pHandle;
	}
	
	return NULL;
}

//
//
//
CHandle *NClassRegistry::FindHandleByTag(const int8 *_pTag)
{
	for(CHandle *pHandle = m_pFirstClass; pHandle; pHandle = pHandle->m_pNext)
	{
		if(StringCompare(pHandle->m_aTag, _pTag) == 0)
			return pHandle;
	}
	
	return NULL;
}

int32_t NClassRegistry::FindHandleByTag(const int8 *_pTag, TDynamicArray<CHandle*> &rlHandles)
{
	for(CHandle *pHandle = m_pFirstClass; pHandle; pHandle = pHandle->m_pNext)
	{
		if(StringCompare(pHandle->m_aTag, _pTag) == 0)
			rlHandles.Add(pHandle);
	}
	return rlHandles.Len();
}

// Creates a registered class
CClass *NClassRegistry::CreateByTag(const int8 *_pTag) { return CreateByHandle(FindHandleByTag(_pTag)); }
CClass *NClassRegistry::CreateByName(const int8 *_pName) { return CreateByHandle(FindHandle(_pName)); }

// Creates a registered class
CClass *NClassRegistry::CreateByHandle(CHandle *_pHandle)
{
	if(!_pHandle)
		return NULL;

	CClass *pClass = _pHandle->m_pfnCreate();
	if(!pClass)
		return NULL;
	
	pClass->m_pClassHandle = _pHandle;
	return pClass;
}

// dumps all registered classes to the log
void NClassRegistry::DumpClasses()
{
	NLog::Write("Registerd classes:\n");
	for(CHandle *pHandle = m_pFirstClass; pHandle; pHandle = pHandle->m_pNext)
		NLog::WriteF("\t%s\n", pHandle->m_aName);
}
