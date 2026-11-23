#ifndef __FOO_MEMORYSTREAM_H__
#define __FOO_MEMORYSTREAM_H__

#include "stream.h"

namespace NFoo
{
	class CMemoryWriter : public COutputStream
	{
		uint8 *m_pAddress;
		int32 m_Written;
	public:

		CMemoryWriter(void *_pAddress=NULL);
		virtual ~CMemoryWriter();

		void Set(void *_pAddress=NULL);

		// Various Functions
		bool virtual IsReady();
		int32 virtual GetSize();

		// Write
		int32 virtual Write(const void *_pBuffer, int32 _Size);
	};

	class CMemoryReader : public CInputStream
	{
		uint8 *m_pAddress;
		int32 m_Pos;
		int32 m_Size;
	public:
		CMemoryReader(void *_pAddress=NULL, int32 _Size=-1);
		virtual ~CMemoryReader();

		void Set(void *_pAddress=NULL, int32 _Size=-1);

		// Various Functions
		bool virtual IsReady();
		int32 virtual GetSize();
		bool virtual EndOfStream();

		// Read
		int32 virtual Read(void *Buffer, int32 Size);
	};
} // end namespace
#endif // __FOO_MEMORYSTREAM_H__
