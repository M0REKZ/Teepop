#ifndef __FOO_ZLIB_H__
#define __FOO_ZLIB_H__

#include <foo/func/stream.h>
#include "deflate.h"

namespace NFoo
{

	//
	// RFC 1950 for zlib format specification
	//
	class CZReader : public CInputStream
	{
		CInputStream &m_rSource;
		CDeflateReader m_DeflateReader;
		bool m_Ready;
		const int8 *m_pError;
	public:
		CZReader(CInputStream &rSource);

		bool virtual IsReady();
		bool virtual EndOfStream();
		
		int32_t virtual Read(void *pBuffer, int32_t Size);
	};
}

#endif // __FOO_ZLIB_H__
