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
		CString(const int8 *_pFormat, ...); // Constructor - Format String Creation
		~CString(); // Destructor
	
		// Functions
		inline int32_t Len() const { return m_Size-1; }
	
		// Operators
		inline int32_t operator = (const int8 *_pStr) { CopyData((int8 *)_pStr); return 0; }
		int32_t operator = (const CString &_Other);

		inline int32_t operator == (const CString &_Other) const { return operator==(_Other.m_pData); }
		inline int32_t operator == (int8 *_pOther) const { return operator==((const int8*)_pOther); }
	
		void operator += (const CString& _Other);
	
		int32_t operator == (const int8 *_pOther) const;
		int32_t operator != (const int8 *_pOther) const;
	
		inline int8 operator [] (const int32_t _Num) const { return m_pData[_Num]; }
	
		inline operator int8 *() { return (int8 *)m_pData; }
		inline operator uint8 *() { return (uint8 *)m_pData; }
		//inline operator const int8 *() const { return (const int8 *)m_pData; }
		//inline operator const uint8 *() const { return (const uint8 *)m_pData; }
		
		void AppendData(const int8 *_pStr, int32_t Len);
		const int8 *GetData() const { return (const int8 *)m_pData; }
		// Hmm
		int8* GetData() { return (int8 *)m_pData; }
		void CopyData(const int8 *_pStr);
		void CopyData(const int8 *_pStr, int32_t _Len);
		void Set(const int8 *_pFormat, ...);
		void SetSize(int32_t _Size);
		CString Delete(int32_t _iStart, int32_t Len)const;
	
		int32_t GetInt();
		fp64 GetFloat();

		bool Find(const int8 *_pFormat, ...) const;
		void Reverse();
		void TrimWhitespaces();
		int32_t FindPos(const int8 *_pFormat, ...) const;

		// Cuts string at given strsep
		CString GetStrSep(const int8 *_pStr);
		int32_t StrLen(const int8 *_pStr);
		void Clear();
	private:
		// Data
		int8 *m_pData;
		int32_t m_Size;
	};

} // namespaces

#endif // __FOO_STRING_H__


