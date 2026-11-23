#include <foo/base/string.h>
#include <foo/base/memory.h>

//#include "string.h"
#include <foo/func/string.h>

using namespace NFoo;

//
// Constructor
//
CString::CString()
{
	m_pData = NULL;
	m_Size = 0;
	CopyData("");
}

//
// Constructor - Format String Creation
//
CString::CString(const int8 *_pFormat, ...)
{
	//ASSUME(_pFormat);

	m_pData = NULL;
	m_Size = 0;

	int8 Buffer[4092*10];
	StringCFormatArgList(Buffer, &_pFormat);
	CopyData(Buffer);
}

//
// Constructor
//
CString::CString(const CString &_Other)
{
	m_pData = NULL;
	*this = _Other;
}

//
// Destructor
//
CString::~CString()
{
	SafeArrayDelete(m_pData);
}

//
//
//
int32 CString::operator = (const CString &_Other)
{
	if(_Other.m_pData)
		CopyData((int8 *)_Other.m_pData);
	else
		Set("");

	return 0;
}

void CString::operator += (const CString& _Other)
{
	if (_Other.m_pData)
		AppendData(_Other.m_pData, _Other.Len());
}

//
//
//
int32 CString::operator == (const int8 *_pOther) const
{
	if(!m_pData)
		return false;

	return (StringCompare(_pOther, m_pData) == 0);
}

int32 CString::operator != (const int8 *_pOther) const
{
	if(!m_pData)
		return false;

	return (StringCompare(_pOther, m_pData) != 0);
}

//
//
//
void CString::CopyData(const int8 *_pStr)
{	
	if(!_pStr)
	{
		Clear();
		return;
	}
	CopyData(_pStr, StringLength(_pStr));
}

void CString::CopyData(const int8 *_pStr, int32 _Len)
{
	SafeDelete(m_pData);
	m_Size = _Len+1;
	m_pData = new int8 [m_Size];
	MemoryCopy(m_pData, _pStr, m_Size);
	m_pData[m_Size-1] = 0;
}


//
//
//
void CString::AppendData(const int8 *_pStr, int32 _Len)
{
	int8 *pNewData = new int8 [m_Size+_Len];
	MemoryCopy(pNewData, m_pData, m_Size);
	MemoryCopy(pNewData+m_Size-1, _pStr, _Len);
	m_Size = m_Size+_Len;
	pNewData[m_Size-1] = 0;

	SafeDelete(m_pData);
	m_pData = pNewData;
}

//
//
//
void CString::Set(const int8 *_pFormat, ...)
{
//	ASSUME(_pFormat);

	int8 Buffer[4092*10];
	StringCFormatArgList(Buffer, &_pFormat);
	CopyData(Buffer);
}

void CString::SetSize(int32 _Size)
{
	SafeDelete(m_pData);
	m_Size = _Size;
	m_pData = new int8[m_Size];
	m_pData[0] = 0;
}

CString CString::Delete(int32 _iStart, int32 _DelLen)const
{
	int32 OrgLen = Len();

	if (_DelLen <= 0 || _iStart < 0 || _iStart > OrgLen)
		return CString();
	if (_DelLen > OrgLen)
		_DelLen = OrgLen;
	if ((_iStart+_DelLen) > OrgLen) 
		_DelLen = OrgLen - _iStart;

	CString Temp;
	if (OrgLen-_DelLen > 0)
	{
		if (_iStart > 0)
			Temp.CopyData(m_pData,_iStart);
		Temp.AppendData(m_pData + _iStart + _DelLen, OrgLen - _DelLen - _iStart + 1);
	}
	return Temp;
}

//
//
//
int32 CString::GetInt()
{
	return StringGetInt(GetData());
}

fp64 CString::GetFloat()
{
	return StringGetFloat(GetData());
}

//
//
//
bool CString::Find(const int8 *_pFormat, ...) const
{
//	ASSUME(_pFormat);
	return (FindPos(_pFormat) != -1);
}

//
//
//
void CString::Reverse()
{
	int32 L = Len();
	for (int32 i = 0; i < L/2; i++)
	{
		int8 t = m_pData[i];
		m_pData[i] = m_pData[L-i-1];
		m_pData[L-i-1] = t;
	}
}

//
//
//
void CString::TrimWhitespaces()
{
	int32 L = Len();
	const int8* pData = GetData();
	if (L <= 0 || !pData)
		return;

	int32 Count = 0;
	for (int32 i = 0; i < L; i++)
	{
		if (pData[i] == ' '  || pData[i] == '\t' || pData[i] == '\n' || pData[i] == '\r')
			Count++;
		else
			break;
	}

	if (Count > 0)
		*this = Delete(0,Count);

	pData = GetData();
	Count = 0;
	for (int32 i = Len()-1; i > 0; i++)
	{
		if (pData[i] == ' '  || pData[i] == '\t' || pData[i] == '\n' || pData[i] == '\r')
			Count++;
		else
			break;
	}
	if (Count > 0)
		*this = Delete(Len()-Count,Count);
}

int32 CString::FindPos(const int8 *_pFormat, ...) const
{
	int8 Buffer[4092*10];
	StringCFormatArgList(Buffer, &_pFormat);
	
	if(StringLength(Buffer) > Len())
		return -1;
	
	bool Found = true;
	for(int32 i = 0; i <= Len() - StringLength(Buffer); i++)
	{
		Found = true;
		int32 iFound = i;
		for(int32 j = 0; j < StringLength(Buffer); j++)
		{
			if(Buffer[j] != m_pData[i+j])
			{
				Found = false;
				break;
			}
		}
		
		if(Found)
			return iFound;
	}
	
	return -1;
}

// Returns string upto given separator, if no separator is found, returns new "complete" string 
// and empties this one
CString CString::GetStrSep(const int8* _pStr)
{
	int32 iPos = FindPos(_pStr);
	if (iPos >= 0)
	{
		int32 Length = StringLength(_pStr);
		CString Return;
		Return.CopyData(m_pData, iPos);
		
		*this = Delete(0, iPos+Length);
		return Return;
	}
	else
	{
		CString Return = *this;
		Clear();
		return Return;
	}
}

void CString::Clear()
{
	SetSize(1);
}
