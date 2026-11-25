#ifndef FOO_FILE_FUNC_REGISTRY_H
#define FOO_FILE_FUNC_REGISTRY_H

#include "string.h"
#include "dynamicarray.h"

namespace NFoo
{
	//
	// simple registry class
	//
	class CRegistry
	{
	public:
		virtual ~CRegistry();

		CString m_Name;
		CString m_Value;

		TDynamicArray<CRegistry*> m_lValues;

		CRegistry *GetChild(const char *pName);
		CRegistry *GetChild(const char *pName, const char *pValue);

		void Dump(int32_t Indent=0);
		bool Read(class CInputStream &rStream);
	};

}

#endif // FOO_FILE_FUNC_REGISTRY_H