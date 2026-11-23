#ifndef __FOO_MIME_H__
#define __FOO_MIME_H__

#include "dynamicarray.h"
#include "string.h"
#include "pair.h"

//
namespace NFoo
{

	//
	// Mime Class
	//
	class CMime
	{
		CString m_Name;
		int32 m_Hash;

		static TDynamicArray< TPair<CString,CString> > ms_lFileExt;

	public:
		CMime();
		CMime(const int8 *pString);
		CMime(const CMime &rMime);

		CMime &operator =(const CMime &rMime);
		bool operator ==(const CMime &rMime) const;
		bool operator !=(const CMime &rMime) const;

		static bool AddFileExt(const int8 *pFileExt, const int8 *pMimeTag);
		static CMime MimeFromFileExt(const int8 *pFilename);
	};

	extern CMime m_NullMime;

}

#endif