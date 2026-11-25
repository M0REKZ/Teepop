#ifndef __FOO_DEFLATE_H__
#define __FOO_DEFLATE_H__

#include <foo/func/stream.h>
#include <foo/func/bitstream.h>
#include "huffman.h"

//#define DEFLATE_DEBUG

namespace NFoo
{
	//
	// RFC 1951 for Deflate compressed data format
	//
	class CDeflateReader : public CInputStream
	{
		CInputStream &m_rSource;
		CBitInputStream m_rBitSource;

		int32_t m_Left;		// bytes left in the block
		bool m_FinalBlock;

		int32_t m_BufferSize;	// window buffer
		uint8_t *m_pBuffer;
		int32_t m_Index;	// write index in the window

		enum
		{
			EHeader=0,
			ETables,
			ERaw,
			EDecode,
			ECopy,
			EBlockDone,
			EDone
		};
		int32_t m_State;	// machine state

		// huffman alphabets
		CHuffmanAlphabet m_CodeLengthAlphabet;
		CHuffmanAlphabet m_LiteralAlphabet;
		CHuffmanAlphabet m_DistAlphabet;

		inline void WrapDown(int32_t &Index);
		inline void WrapUp(int32_t &Index);

		// ECopy
		int32_t m_CopyLength;
		int32_t m_CopyDistance;
		int32_t m_CopyIndex;

		//
		int32_t m_Size; // final size

		#ifdef DEFLATE_DEBUG
			int64_t m_aTimes[EDone];
			int32_t m_OutputSize; // debug
		#endif

	public:
		CDeflateReader(CInputStream &rSource, int32_t WindowSize=32*1024);
		~CDeflateReader();

		bool virtual IsReady();
		bool virtual EndOfStream();

		int32_t virtual GetSize() { return m_Size; }
		void SizeHint(int32_t Size) { m_Size = Size > 0 ? Size : -1; }; // Tells the deflator the final size

		int32_t virtual Read(void *pBuffer, int32_t Size);
	};
}

#endif // __FOO_DEFLATE_H__
