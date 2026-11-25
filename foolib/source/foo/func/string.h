//==========================================================================================
// Copyright (c), Clas M. Auvinen 2003
//==========================================================================================

#ifndef __FOO_STRING_H__
#define __FOO_STRING_H__

#include <foo/base/base.h>

namespace NFoo {
	
	class CString
	{
	public:
		CString();// Constructor - No Args
		CString(const CString &_Other);
		CString(const char *_pFormat, ...); // Constructor - Format String Creation
		~CString(); // Destructor
	
		// Functions
		inline int32_t Len() const { return m_Size-1; }
	
		// Operators
		inline int32_t operator = (const char *_pStr) { CopyData((char *)_pStr); return 0; }
		int32_t operator = (const CString &_Other);

		inline int32_t operator == (const CString &_Other) const { return operator==(_Other.m_pData); }
		inline int32_t operator == (char *_pOther) const { return operator==((const char*)_pOther); }
	
		void operator += (const CString& _Other);
	
		int32_t operator == (const char *_pOther) const;
		int32_t operator != (const char *_pOther) const;
	
		inline char operator [] (const int32_t _Num) const { return m_pData[_Num]; }
	
		inline operator char *() { return (char *)m_pData; }
		inline operator uint8_t *() { return (uint8_t *)m_pData; }
		//inline operator const int8_t *() const { return (const int8_t *)m_pData; }
		//inline operator const uint8_t *() const { return (const uint8_t *)m_pData; }
		
		void AppendData(const char *_pStr, int32_t Len);
		const char *GetData() const { return (const char *)m_pData; }
		// Hmm
		char* GetData() { return (char *)m_pData; }
		void CopyData(const char *_pStr);
		void CopyData(const char *_pStr, int32_t _Len);
		void Set(const char *_pFormat, ...);
		void SetSize(int32_t _Size);
		CString Delete(int32_t _iStart, int32_t Len)const;
	
		int32_t GetInt();
		fp64 GetFloat();

		bool Find(const char *_pFormat, ...) const;
		void Reverse();
		void TrimWhitespaces();
		int32_t FindPos(const char *_pFormat, ...) const;

		// Cuts string at given strsep
		CString GetStrSep(const char *_pStr);
		int32_t StrLen(const char *_pStr);
		void Clear();
	private:
		// Data
		char *m_pData;
		int32_t m_Size;
	};

} // namespaces

#endif // __FOO_STRING_H__


