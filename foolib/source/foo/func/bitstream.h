#ifndef __FOO_BITSTREAM_H__
#define __FOO_BITSTREAM_H__

#include "stream.h"

namespace NFoo
{
	//
	// Bytes:     -Byte 1- -Byte 0-
	//        ... 76543210 76543210
	// Bits:		 ...98 76543210
	//
	// Inlined for speed reasons
	//
	class CBitInputStream
	{
		int32_t m_NumBits;
		uint32_t m_Bits;
		CInputStream &m_rInput;

		void Fill(int32_t nBitsNeeded)
		{
			while(m_NumBits < nBitsNeeded)
			{
				uint8 b = m_rInput.ReadNE<uint8>();
				m_Bits |= b<<(m_NumBits);
				m_NumBits += 8;
			}
		}

	public:
		CBitInputStream(CInputStream &rInput);
		
		void Flush()
		{
			m_Bits = 0;
			m_NumBits = 0;
		}
			
		int32_t Peek(int32_t nBits)
		{
			Fill(nBits);
			return m_Bits&((1<<nBits)-1);
		}

		void Discard(int32_t nBits)
		{
			Fill(nBits);
			m_NumBits -= nBits;
			m_Bits >>= nBits;
		}
		
		int32_t Read(int32_t nBits)
		{
			int32_t v = Peek(nBits);
			Discard(nBits);
			return v;
		}
	};

	//
	// Bytes:     -Byte 1- -Byte 0-
	//        ... 76543210 76543210
	// Bits:		 ...98 76543210
	//
	// Inlined for speed reasons
	//
	class CBitOutputStream
	{
		int32_t m_NumBits;
		uint32_t m_Bits;
		COutputStream &m_rOutput;

		int32_t written;

		void WriteOut()
		{
			while(m_NumBits >= 8)
			{
				m_rOutput.WriteNE<uint8>(m_Bits&0xFF);
				m_Bits >>= 8;
				m_NumBits -= 8;
				written++;
			}
		}

	public:
		CBitOutputStream(COutputStream &rOutput);
		
		void Flush()
		{
			WriteOut();
			if(m_NumBits)
				m_rOutput.WriteNE(m_Bits&0xFF);
			m_Bits = 0;
			m_NumBits = 0;
		}
			
		int32_t Write(uint32_t Bits, int32_t nBits)
		{
			m_Bits |= Bits<<m_NumBits;
			m_NumBits += nBits;
			WriteOut();
			return 0;
		}
	};

	//
	// Bytes: -Byte 0- -Byte 1-
	//        76543210 76543210
	// Bits:  01234567 98
	//
	// Inlined for speed reasons
	//
	class CBitInputStreamHigh
	{
		int32_t m_NumBits;
		uint32_t m_Bits;
		CInputStream &m_rInput;

		void Fill(int32_t nBitsNeeded)
		{
			while(m_NumBits < nBitsNeeded)
			{
				uint8 b = m_rInput.ReadNE<uint8>();
				m_Bits |= b<<(24-m_NumBits);
				m_NumBits += 8;
			}
		}

	public:
		CBitInputStreamHigh(CInputStream &rInput);
		
		void Flush()
		{
			m_Bits = 0;
			m_NumBits = 0;
		}
			
		int32_t Peek(int32_t nBits)
		{
			Fill(nBits);
			return m_Bits>>(32-nBits);
		}

		void Discard(int32_t nBits)
		{
			Fill(nBits);
			m_NumBits -= nBits;
			m_Bits <<= nBits;
		}
		
		int32_t Read(int32_t nBits)
		{
			int32_t v = Peek(nBits);
			Discard(nBits);
			return v;
		}
	};

} // name space end
#endif // __FOO_BITSTREAM_H__
