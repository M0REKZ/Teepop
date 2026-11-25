#include <foo/base/memory.h>
#include "stream.h"

using namespace NFoo;

int32_t COutputStream::Write_p8(CString Str)
{
	int32_t len = Clamp(Str.Len(), 0, 0xFF);
	WriteBE<uint8>(len);
	Write(Str.GetData(), len);
	return Str.Len();
}

int32_t COutputStream::Write_p16(CString Str)
{
	int32_t len = Clamp(Str.Len(), 0, 0xFFFF);
	WriteBE<uint16>(len);
	Write(Str.GetData(), len);
	return Str.Len();
}


int32_t CInputStream::Transfer(COutputStream &rOutput, int32_t Count, int32_t BufferSize)
{
	int32_t ReadCount = 0;
	uint8 aBuffer[16*1024];
	BufferSize = 16*1024;
	if(Count > 0)
	{
		while(Count)
		{
			int32_t l = Count>BufferSize?BufferSize:Count;
			int32_t ThisRead = Read(aBuffer, l);
			ReadCount += ThisRead;
			rOutput.Write(aBuffer, ThisRead);
			if(ThisRead < l)
				break;
			Count -= l;
		}
	}
	else
	{
		while(!EndOfStream())
		{
			int32_t ThisRead = Read(aBuffer, BufferSize);
			ReadCount += ThisRead;
			rOutput.Write(aBuffer, ThisRead);
			if(ThisRead < BufferSize)
				break;
		}
	}

	return ReadCount;
}

void CInputStream::Read_sz(int8 *_pDst)
{
	while(1)
	{
		*_pDst = ReadNE<int8>();
		if(!*_pDst)
			break;

		_pDst++;
	}
}

int8 *CInputStream::ReadLine(int8 *pDst)
{
	static int8 aLine[4092];
	if(!pDst)
		pDst = aLine;
	int8 *pStart = pDst;
	while(!EndOfStream())
	{
		*pDst = ReadNE<int8>();
		if(*pDst == '\n' || *pDst == '\r')
		{
			*pDst = 0;
			break;
		}

		pDst++;
	}
	*pDst = 0;

	return pStart;
}


// Read pascal string (uint16)
CString CInputStream::Read_p8()
{
	int32_t StrLen = ReadBE<uint8>();
	CString Str;
	Str.SetSize(StrLen+1);
	Str.GetData()[StrLen] = 0;
	Read(Str.GetData(), StrLen);
	return Str;
}

CString CInputStream::Read_p16()
{
	int32_t StrLen = ReadBE<uint16>();
	CString Str;
	Str.SetSize(StrLen+1);
	Str.GetData()[StrLen] = 0;
	Read(Str.GetData(), StrLen);
	return Str;
}

void CBlockReader::FillBuffer()
{
	m_Pos = 0;
	m_BufferLeft = m_rSource.Read(m_pBuffer, m_BufferSize);
}

CBlockReader::CBlockReader(CInputStream &rSource, int32_t BufferSize)
: CChainedInputStream(rSource), m_BufferSize(BufferSize)
{
	m_pBuffer = new uint8[m_BufferSize];
	m_Pos = 0;
	m_BufferLeft = 0;
}

CBlockReader::~CBlockReader()
{
	SafeArrayDelete(m_pBuffer);
}


int32_t CBlockReader::Read(void *pOutput, int32_t Size)
{
	uint8 *pOut = (uint8 *)pOutput;
	int32_t Left = Size;
	while(Left)
	{
		if(!m_BufferLeft)
		{
			FillBuffer();
			if(!m_BufferLeft)
				return Size-Left;
		}

		int32_t BlockSize = Left < m_BufferLeft ? Left : m_BufferLeft;
		if(pOut)
		{
			MemoryCopy(pOut, m_pBuffer+m_Pos, BlockSize);
			pOut += BlockSize;
		}
		m_Pos += BlockSize;
		m_BufferLeft -= BlockSize;
		Left -= BlockSize;
	}
	return Size;
}