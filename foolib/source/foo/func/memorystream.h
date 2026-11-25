#ifndef __FOO_MEMORYSTREAM_H__
#define __FOO_MEMORYSTREAM_H__

#include "stream.h"

namespace NFoo
{
	class CMemoryWriter : public COutputStream
	{
		uint8_t *m_pAddress;
		int32_t m_Written;
	public:

		CMemoryWriter(void *_pAddress=NULL);
		virtual ~CMemoryWriter();

		void Set(void *_pAddress=NULL);

		// Various Functions
		bool virtual IsReady();
		int32_t virtual GetSize();

		// Write
		int32_t virtual Write(const void *_pBuffer, int32_t _Size);
	};

	class CMemoryReader : public CInputStream
	{
		uint8_t *m_pAddress;
		int32_t m_Pos;
		int32_t m_Size;
	public:
		CMemoryReader(void *_pAddress=NULL, int32_t _Size=-1);
		virtual ~CMemoryReader();

		void Set(void *_pAddress=NULL, int32_t _Size=-1);

		// Various Functions
		bool virtual IsReady();
		int32_t virtual GetSize();
		bool virtual EndOfStream();

		// Read
		int32_t virtual Read(void *Buffer, int32_t Size);
	};
} // end namespace
#endif // __FOO_MEMORYSTREAM_H__
