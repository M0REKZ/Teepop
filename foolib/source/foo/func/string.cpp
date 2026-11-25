#include <foo/base/string.h>
#include <foo/base/memory.h>

//#include "string.h"
#include <foo/func/string.h>
#include <cstdarg>
#include <cstdio>

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
CString::CString(const char *_pFormat, ...)
{
    m_pData = NULL;
    m_Size = 0;

    char Buffer[4092 * 10];

    va_list Args;
    va_start(Args, _pFormat);

    StringCFormatArgList(Buffer, sizeof(Buffer), _pFormat, Args);

    va_end(Args);

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
int32_t CString::operator = (const CString &_Other)
{
	if(_Other.m_pData)
		CopyData((char *)_Other.m_pData);
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
int32_t CString::operator == (const char *_pOther) const
{
	if(!m_pData)
		return false;

	return (StringCompare(_pOther, m_pData) == 0);
}

int32_t CString::operator != (const char *_pOther) const
{
	if(!m_pData)
		return false;

	return (StringCompare(_pOther, m_pData) != 0);
}

//
//
//
void CString::CopyData(const char *_pStr)
{	
	if(!_pStr)
	{
		Clear();
		return;
	}
	CopyData(_pStr, StringLength(_pStr));
}

void CString::CopyData(const char *_pStr, int32_t _Len)
{
	SafeDelete(m_pData);
	m_Size = _Len+1;
	m_pData = new char [m_Size];
	MemoryCopy(m_pData, _pStr, m_Size);
	m_pData[m_Size-1] = 0;
}


//
//
//
void CString::AppendData(const char *_pStr, int32_t _Len)
{
	char *pNewData = new char [m_Size+_Len];
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
void CString::Set(const char *_pFormat, ...)
{
    char Buffer[4092 * 10];

    va_list Args;
    va_start(Args, _pFormat);

    StringCFormatArgList(Buffer, sizeof(Buffer), _pFormat, Args);

    va_end(Args);

    CopyData(Buffer);
}

void CString::SetSize(int32_t _Size)
{
	SafeDelete(m_pData);
	m_Size = _Size;
	m_pData = new char[m_Size];
	m_pData[0] = 0;
}

CString CString::Delete(int32_t _iStart, int32_t _DelLen)const
{
	int32_t OrgLen = Len();

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
int32_t CString::GetInt()
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
bool CString::Find(const char *_pFormat, ...) const
{
    char Buffer[4092 * 10];

    va_list Args;
    va_start(Args, _pFormat);

    StringCFormatArgList(Buffer, sizeof(Buffer), _pFormat, Args);

    va_end(Args);

    return FindPos(Buffer) != -1;
}

//
//
//
void CString::Reverse()
{
	int32_t L = Len();
	for (int32_t i = 0; i < L/2; i++)
	{
		char t = m_pData[i];
		m_pData[i] = m_pData[L-i-1];
		m_pData[L-i-1] = t;
	}
}

//
//
//
void CString::TrimWhitespaces()
{
	int32_t L = Len();
	const char* pData = GetData();
	if (L <= 0 || !pData)
		return;

	int32_t Count = 0;
	for (int32_t i = 0; i < L; i++)
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
	for (int32_t i = Len()-1; i > 0; i++)
	{
		if (pData[i] == ' '  || pData[i] == '\t' || pData[i] == '\n' || pData[i] == '\r')
			Count++;
		else
			break;
	}
	if (Count > 0)
		*this = Delete(Len()-Count,Count);
}

int32_t CString::FindPos(const char *_pFormat, ...) const
{
    char Buffer[4092 * 10];

    va_list Args;
    va_start(Args, _pFormat);

    StringCFormatArgList(Buffer, sizeof(Buffer), _pFormat, Args);

    va_end(Args);

    int32_t SearchLen = StringLength(Buffer);
    int32_t StrLen = Len();

    if(SearchLen > StrLen)
        return -1;

    for(int32_t i = 0; i <= StrLen - SearchLen; i++)
    {
        bool Found = true;

        for(int32_t j = 0; j < SearchLen; j++)
        {
            if(Buffer[j] != m_pData[i+j])
            {
                Found = false;
                break;
            }
        }

        if(Found)
            return i;
    }

    return -1;
}

// Returns string upto given separator, if no separator is found, returns new "complete" string 
// and empties this one
CString CString::GetStrSep(const char* _pStr)
{
	int32_t iPos = FindPos(_pStr);
	if (iPos >= 0)
	{
		int32_t Length = StringLength(_pStr);
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
