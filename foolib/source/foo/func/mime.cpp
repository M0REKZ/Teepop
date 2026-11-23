#include <foo/base/string.h>
#include "mime.h"

using namespace NFoo;

CMime NFoo::m_NullMime;
TDynamicArray< TPair<CString,CString> > CMime::ms_lFileExt;

//
CMime::CMime()
{
	m_Hash = 0;
}

//
CMime::CMime(const int8 *pString)
{
	m_Name = pString;
	m_Hash = StringGetHash(pString);
}

//
CMime::CMime(const CMime &rMime)
{
	*this = rMime;
}

//
CMime &CMime::operator =(const CMime &rMime)
{
	m_Hash = rMime.m_Hash;
	m_Name = rMime.m_Name;
	return *this;
}

//
bool CMime::operator !=(const CMime &rMime) const
{
	return !(*this == rMime);
}

//
bool CMime::operator ==(const CMime &rMime) const
{
	if(m_Hash == rMime.m_Hash && rMime.m_Name == rMime.m_Name)
		return true;

	return false;
}

//
bool CMime::AddFileExt(const int8 *pFileExt, const int8 *pMimeTag)
{
	ms_lFileExt.Add(TPair<CString,CString>(pFileExt, pMimeTag));

	return true;
}

//
CMime CMime::MimeFromFileExt(const int8 *pFilename)
{
	const int8 *pFileExt = NULL;
	while(*pFilename != 0)
	{
		if(*pFilename == '.')
			pFileExt = pFilename+1;
		pFilename++;
	}

	if(!pFileExt)
		return CMime();

//	if(StringCompare(pFileExt, "txt") == 0) return CMime("text/plain");
//	else if(StringCompare(pFileExt, "zip") == 0) return CMime("application/zip");

	for (int i = 0; i < ms_lFileExt.Len(); i++)
		if (ms_lFileExt[i].m_First == pFileExt)
			return CMime(ms_lFileExt[i].m_Second);

	return CMime();
}
