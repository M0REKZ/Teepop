#include <foo/base/memory.h>
#include "rle.h"

using namespace NFoo;

//
//
//
CRLEEncoder::CRLEEncoder(COutputStream *_pTarget, int32 _UnitSize)
: m_pTarget(_pTarget), m_UnitSize(_UnitSize)
{}

//
//
//
int32 CRLEEncoder::GetSize()
{
	if(m_pTarget)
		return m_pTarget->GetSize();
	return -1;
}

//
//
//
bool CRLEEncoder::IsReady()
{
	if(m_pTarget &&  m_UnitSize > 0)
		return m_pTarget->IsReady();
	return false;
}

//
//
//
bool CRLEEncoder::Flush()
{
	if(m_pTarget)
		return m_pTarget->Flush();
	return false;
}

//
//
//
int32 CRLEEncoder::Write(const void *_pBuffer, int32 _Size)
{
	if(!IsReady())
		throw "RLECompressor isn't ready to write!";

	uint8 *pSrc = (uint8 *)_pBuffer;
	//uint8 *pDst = pRLEData;
	int32 Count = _Size/m_UnitSize;
	int32 UnitSize = m_UnitSize;
	int32 Written = 0;

	while(Count)
	{
		int32 Max = 128;
		if(Count-1 < Max)
			Max = Count-1;
		if(Count > 1 && MemoryCompare(pSrc, pSrc+UnitSize, UnitSize) == 0)
		{
			// RLE mode
			int32 Len = 1;
			while(MemoryCompare(pSrc, pSrc+UnitSize*Len, UnitSize) == 0 && Len < Max)
				Len++;

			Count -= Len;
			m_pTarget->WriteNE<uint8>((1<<7)|(Len-1));
			m_pTarget->Write(pSrc, UnitSize);
			pSrc += UnitSize*Len;
			Written += UnitSize+1;
		}
		else
		{
			// Raw
			int32 Len = 1; 
			while(Len < Max)
			{
				if(Count-Len > 2)
				{
					if(	MemoryCompare(pSrc+UnitSize*Len, pSrc+UnitSize*(Len+1), UnitSize) == 0 &&
						MemoryCompare(pSrc+UnitSize*(Len+1), pSrc+UnitSize*(Len+2), UnitSize))
					{
						break; // RLE packet
					}
					else
						Len++;
				}
				else
					Len++;
			}

			m_pTarget->WriteNE<uint8>(Len-1);
			Count -= Len;
			while(Len--)
			{
				m_pTarget->Write(pSrc, UnitSize);
				pSrc += UnitSize;
			}
			Written += UnitSize*Len+1;
		}
	}

	return Written;
}



CRLEDecoder::CRLEDecoder(CInputStream *_pSource, int32 _UnitSize)
{
	m_pSource = _pSource;
	m_UnitSize = _UnitSize;
	m_ReadBytes = 0;
}

int32 CRLEDecoder::GetSize()
{
	return m_pSource->GetSize();
}

bool CRLEDecoder::IsReady()
{
	if(m_pSource && m_UnitSize > 0)
		return m_pSource->IsReady();
	return false;
}

bool CRLEDecoder::EndOfStream()
{
	return m_pSource->EndOfStream();
}

int32 CRLEDecoder::Read(void *_pBuffer, int32 _Size)
{
	uint8 *pDst = (uint8 *)_pBuffer;
	int32 Count = _Size/m_UnitSize;
	while(Count)
	{
		int32 BlockHeader = m_pSource->ReadNE<uint8>();
		int32 Len = (BlockHeader&127)+1;
		Count -= Len;
		if(Count < 0)
			throw "RLEDecoder: Error, Out of bounds";

		if(BlockHeader>>7)
		{
			// RLE
			uint8 *pSrc = pDst;
			m_pSource->Read(pDst, m_UnitSize);
			pDst += m_UnitSize;
			Len--;
			while(Len--)
			{
				MemoryCopy(pDst, pSrc, m_UnitSize);
				pDst += m_UnitSize;
			}

		}
		else // RAW
		{
			m_pSource->Read(pDst, m_UnitSize*Len);
			pDst += m_UnitSize*Len;
		}
	}
	return _Size;
}
