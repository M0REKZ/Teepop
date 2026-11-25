#include <foo/base/memory.h>
#include "rle.h"

using namespace NFoo;

//
//
//
CRLEEncoder::CRLEEncoder(COutputStream *_pTarget, int32_t _UnitSize)
: m_pTarget(_pTarget), m_UnitSize(_UnitSize)
{}

//
//
//
int32_t CRLEEncoder::GetSize()
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
int32_t CRLEEncoder::Write(const void *_pBuffer, int32_t _Size)
{
	if(!IsReady())
		throw "RLECompressor isn't ready to write!";

	uint8_t *pSrc = (uint8_t *)_pBuffer;
	//uint8_t *pDst = pRLEData;
	int32_t Count = _Size/m_UnitSize;
	int32_t UnitSize = m_UnitSize;
	int32_t Written = 0;

	while(Count)
	{
		int32_t Max = 128;
		if(Count-1 < Max)
			Max = Count-1;
		if(Count > 1 && MemoryCompare(pSrc, pSrc+UnitSize, UnitSize) == 0)
		{
			// RLE mode
			int32_t Len = 1;
			while(MemoryCompare(pSrc, pSrc+UnitSize*Len, UnitSize) == 0 && Len < Max)
				Len++;

			Count -= Len;
			m_pTarget->WriteNE<uint8_t>((1<<7)|(Len-1));
			m_pTarget->Write(pSrc, UnitSize);
			pSrc += UnitSize*Len;
			Written += UnitSize+1;
		}
		else
		{
			// Raw
			int32_t Len = 1; 
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

			m_pTarget->WriteNE<uint8_t>(Len-1);
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



CRLEDecoder::CRLEDecoder(CInputStream *_pSource, int32_t _UnitSize)
{
	m_pSource = _pSource;
	m_UnitSize = _UnitSize;
	m_ReadBytes = 0;
}

int32_t CRLEDecoder::GetSize()
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

int32_t CRLEDecoder::Read(void *_pBuffer, int32_t _Size)
{
	uint8_t *pDst = (uint8_t *)_pBuffer;
	int32_t Count = _Size/m_UnitSize;
	while(Count)
	{
		int32_t BlockHeader = m_pSource->ReadNE<uint8_t>();
		int32_t Len = (BlockHeader&127)+1;
		Count -= Len;
		if(Count < 0)
			throw "RLEDecoder: Error, Out of bounds";

		if(BlockHeader>>7)
		{
			// RLE
			uint8_t *pSrc = pDst;
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
