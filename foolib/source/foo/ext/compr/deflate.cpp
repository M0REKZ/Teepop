#include <foo/base/log.h>
#include "deflate.h"

using namespace NFoo;

//
// Tables
//
const int32 g_aLenBase[29] = { 3,4,5,6,7,8,9,10, 11,13,15,17,19,23,27,31,
							35,43,51,59,67,83,99,115, 131,163,195,227,258};

const int32 g_aLenExtra[29] = { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};

const int32 g_aDistBase[30] = {	1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
							257,385,513,769,1025,1537,2049,3073,
							4097,6145,8193,12289,16385,24577};

const int32 g_aDistExtra[30] = {  0,0,0,0, 1,1,2,2, 3,3,4,4, 5,5,6,6,
								7,7,8,8, 9,9,10,10, 11,11,12,12, 13,13};



//
//
//
void CDeflateReader::WrapDown(int32 &Index)
{
	while(Index >= m_BufferSize)
		Index -= m_BufferSize;
}

void CDeflateReader::WrapUp(int32 &Index)
{
	while(Index < 0)
		Index += m_BufferSize;
}

//
//
//
CDeflateReader::CDeflateReader(CInputStream &rSource, int32 WindowSize)
: m_rSource(rSource), m_rBitSource(rSource), m_BufferSize(WindowSize*2)
{
	m_pBuffer = new uint8[m_BufferSize];
	m_State = EHeader;
	m_FinalBlock = false;
	m_Size = -1;

	m_Index = 0;

#ifdef DEFLATE_DEBUG
	for(int32 n = 0; n < EDone; n++)
		m_aTimes[n] = 0;
	m_OutputSize = 0;
#endif
}

//
//
//
CDeflateReader::~CDeflateReader()
{
#ifdef DEFLATE_DEBUG
	NLog::WriteF("Deflater:\n");
	for(int32 n = 0; n < EDone; n++)
		NLog::WriteF("\t%d: %.4f\n", n, m_aTimes[n]/(fp32)GetTickFrequency());
	NLog::WriteF("\tCodeLengthAlphabet: "); m_CodeLengthAlphabet.LogTimes();
	NLog::WriteF("\tLiteralAlphabet: "); m_LiteralAlphabet.LogTimes();
	NLog::WriteF("\tDistAlphabet: "); m_DistAlphabet.LogTimes();
#endif
	SafeArrayDelete(m_pBuffer);
}

//
//
//
bool CDeflateReader::IsReady() { return m_rSource.IsReady(); }
bool CDeflateReader::EndOfStream() { return m_State == EDone; }

//
//
//
int32 CDeflateReader::Read(void *pBuffer, int32 Size)
{
	if(Size <= 0)
		return 0;
	int32 StartSize = Size;
	uint8 *pDst = (uint8 *)pBuffer;
	int32 OutputSize = 0;

	while(1)
	{
#ifdef DEFLATE_DEBUG
		int64 StartTime = GetTickCount();
#endif
		int32 StartState = m_State;
		//
		switch(m_State)
		{
		// Read header
		case EHeader:
			{
				m_FinalBlock = m_rBitSource.Read(1);
				int32 BlockType = m_rBitSource.Read(2);
				if(BlockType == 0)
				{
					m_State = ERaw;
					m_rBitSource.Flush();
					m_CopyLength = m_rSource.ReadLE<uint16>();
					OutputSize = m_rSource.ReadLE<uint16>();
					//throw "ROARRRRRRR! Can't handle this blocktype";
				}
				else if(BlockType == 1) // fixed huffman codes
					throw "ROARRRRRRR! Can't handle this blocktype";
				else if(BlockType == 2) // dynamic huffman codes
					m_State = ETables;
				else
					throw "ROARRRRRRR! Can't handle this blocktype";
			} break;
		// Read tables
		case ETables:
			{
				static const int32 aOrder[19] = 
					{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
						
				// read huffman codes
				int32 NumLiteral = m_rBitSource.Read(5)+257;
				int32 NumDist = m_rBitSource.Read(5)+1;
				int32 NumLength = m_rBitSource.Read(4)+4;

				// clear alphabets
				m_CodeLengthAlphabet.Clear();
				m_LiteralAlphabet.Clear();
				m_DistAlphabet.Clear();

				// read code length alphabet
				for(int32 i = 0; i < NumLength; i++)
				{
					int32 a = m_rBitSource.Read(3);
					if(a)
						m_CodeLengthAlphabet.Add(aOrder[i], a);
				}
				m_CodeLengthAlphabet.CalculateCodes();

				// Read literal and distance alphabets
				int32 aCodeSizes[350] = {0};
				int32 *pLiteralCodeSizes = aCodeSizes;
				int32 *pDistCodeSizes = aCodeSizes+NumLiteral;
				int32 Total = NumLiteral + NumDist;
				int32 Index = 0;
				while(Index < Total)
				{
					int32 a = m_CodeLengthAlphabet.Decode(m_rBitSource);
					if(a <= 15)
					{
						aCodeSizes[Index++] = a;
					}
					else
					{
						int32 nCopy, Val;
						if(a == 16)
						{
							nCopy = m_rBitSource.Read(2)+3;
							Val = aCodeSizes[Index-1];
						}
						else
						{
							Val = 0;
							if(a == 17)
								nCopy = m_rBitSource.Read(3)+3;
							else
								nCopy = m_rBitSource.Read(7)+11;
						}
						if(Index+nCopy > Total)
							nCopy = Total-Index;

						while(nCopy--)
							aCodeSizes[Index++] = Val;
					}
				}
				
				// construct
				for(int32 i = 0; i < NumLiteral; i++)
					if(pLiteralCodeSizes[i])
						m_LiteralAlphabet.Add(i, pLiteralCodeSizes[i]);
				for(int32 i = 0; i < NumDist; i++)
					if(pDistCodeSizes[i])
						m_DistAlphabet.Add(i, pDistCodeSizes[i]);
				m_LiteralAlphabet.CalculateCodes();
				m_DistAlphabet.CalculateCodes();

				//
				m_State = EDecode;
			} break;

		// decode a lit,dist combo
		case EDecode:
			{
				int32 a = m_LiteralAlphabet.Decode(m_rBitSource);
				if(a < 256)
				{
					OutputSize++;
					if(pDst)
						*(pDst++) = a;
					m_pBuffer[m_Index] = a;
					m_Index++;
					WrapDown(m_Index);
					Size--;

					if(Size == 0)
						return OutputSize;
					break;
				}
				else if(a == 256)
				{
					//NLog::WriteF("Found EOS!\n");
					m_State = EBlockDone;
					break;
				}
				else
				{
					int32 lcode = a - 257;
					m_CopyLength = g_aLenBase[lcode] + m_rBitSource.Read(g_aLenExtra[lcode]);
					int32 dcode = m_DistAlphabet.Decode(m_rBitSource);
					m_CopyDistance = g_aDistBase[dcode] + m_rBitSource.Read(g_aDistExtra[dcode]);

					//NLog::WriteF("%d %d,%d\n", m_OutputSize, m_CopyLength, m_CopyDistance);

					m_State = ECopy;
					m_CopyIndex = m_Index - m_CopyDistance;
					WrapUp(m_CopyIndex);
				}
			} break;
		//
		case ECopy:
			{
				while(m_CopyLength && Size)
				{
					if(pDst)
						*(pDst++) = m_pBuffer[m_CopyIndex];
					m_pBuffer[m_Index++] = m_pBuffer[m_CopyIndex++];

					WrapDown(m_Index);
					WrapDown(m_CopyIndex);
					OutputSize++;
					m_CopyLength--;
					Size--;
				}

				if(!m_CopyLength)
					m_State = EDecode;

				if(!Size)
					return OutputSize;
			} break;
		// raw block
		case ERaw:
			{
				while(m_CopyLength && Size)
				{
					uint8 b = m_rSource.ReadNE<uint8>();
					if(pDst)
						*(pDst++) = b;
					m_pBuffer[m_Index++] = b;

					WrapDown(m_Index);
					OutputSize++;
					m_CopyLength--;
					Size--;
				}				
				
				if(!m_CopyLength)
					m_State = EBlockDone;
				if(!Size)
					return OutputSize;
			} break;
		// Block done
		case EBlockDone:
			if(m_FinalBlock)
				m_State = EDone;
			else
				m_State = EHeader;
			break;
		// done
		case EDone:
			return OutputSize;
		default:
			throw "Invalid decoder state";
		}

		#ifdef DEFLATE_DEBUG
			m_aTimes[StartState] += GetTickCount()-StartTime;
		#endif
	}
	return -1;
}
