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

		CRegistry *GetChild(const int8 *pName);
		CRegistry *GetChild(const int8 *pName, const int8 *pValue);

		void Dump(int32 Indent=0);
		bool Read(class CInputStream &rStream);
	};

}

#endif // FOO_FILE_FUNC_REGISTRY_H