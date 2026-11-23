
#include <foo/base/log.h>
#include <foo/func/stream.h>
#include <foo/func/bitstream.h>
#include <foo/func/image.h>

#include <foo/ext/compr/huffman.h>

#include "dct.h"

using namespace NFoo;

//
// There is a possiblity that there are several
//
class CJPEGScanReader : public CInputStream
{
	CInputStream &m_rSource;
public:
	CJPEGScanReader(CInputStream &rSource)
	: m_rSource(rSource)
	{
	}

	bool virtual IsReady() { return m_rSource.IsReady(); }
	bool virtual EndOfStream() { return false; };

	int32 virtual Read(void *pBuffer, int32 Size)
	{
		int32 r = Size;
		uint8 *pDst = (uint8 *)pBuffer;
		while(Size--)
		{
			*pDst = m_rSource.ReadNE<uint8>();
			if(*pDst == 0xFF)
				m_rSource.ReadNE<uint8>();
			pDst++;
		}
		return r;
	}
};

struct SComponent
{
	int32 m_ID;
	int32 m_Factors;
	int32 m_Quat;

	int32 m_XSizeUps;
	int32 m_YSizeUps;
};

//
//
//
class CJPEGFormat : public CImageFormat
{
	static CMime ms_Mime;

	CHuffmanAlphabet m_aHuffmanAlphabet[8];
	SComponent m_aComponents[3];
	int32 m_NumComponents;
	fp32 m_aQuatTable[4][64];
	int32 m_Width, m_Height;
	//CImage m_aComponentData[3];

	int32 m_NumBlocks;

	int32 m_MCUCount;
	int32 m_MCUSizeX;
	int32 m_MCUSizeY;

	uint8 m_aVector[64];
	int32 m_aComponentDC[3];

	//
	static int32 GetValue(int32 _Cat, int32 _Value)
	{
		int32 Base;
		int32 Sign = (_Value&(1<<(_Cat-1))) ? 1 : -1;

		if(Sign > 0)
			Base = (1<<(_Cat))/2;
		else
			Base = -(1<<(_Cat))+1;

		int32 ValueMask = (1<<(_Cat-1))-1;
		int32 ValuePart = _Value&ValueMask;

		int32 Final = (Base+ValuePart);
		return Final;
	}

	void ReadBlock(CBitInputStreamHigh &rBitStream, int32 iDCTable, int32 iACTable, int32 iComp);

public:
	CJPEGFormat()
	{
		CMime::AddFileExt("jpg","image/jpeg");
		CMime::AddFileExt("jpeg","image/jpeg");
	}

	bool CanHandle(const CMime &rMime)
	{
		if(rMime == ms_Mime)
			return true;

		return false;
	}

	bool Load(CInputStream &_rStream, CImage &_rImage);
};

CMime CJPEGFormat::ms_Mime("image/jpeg");
MACRO_REGISTER_CLASS(CJPEGFormat, "imageformat")

// 
const int32 g_aZigPattern[64] = {
			0, 1, 5, 6,14,15,27,28,
			2, 4, 7,13,16,26,29,42,
			3, 8,12,17,25,30,41,43,
			9,11,18,24,31,40,44,53,
			10,19,23,32,39,45,52,54,
			20,22,33,38,46,51,55,60,
			21,34,37,47,50,56,59,61,
			35,36,48,49,57,58,62,63};

//fp32 g_aDCTLookup[8*8*8*8];

	//
	//
	//
	int32 GetValue(int32 _Cat, int32 _Value)
	{
		int32 Base;
		int32 Sign = (_Value&(1<<(_Cat-1))) ? 1 : -1;

		if(Sign > 0)
			Base = (1<<(_Cat))/2;
		else
			Base = -(1<<(_Cat))+1;

		int32 ValueMask = (1<<(_Cat-1))-1;
		int32 ValuePart = _Value&ValueMask;

		int32 Final = (Base+ValuePart);
		return Final;
	}


	enum
	{
		SOF0 = 0xc0, // Start of Frame 0
		DHT = 0xc4, // Define Huffman Table
		SOI = 0xd8, // Start of Image
		EOI = 0xd9, // End of Image
		SOS = 0xda, // Start of Scans
		DQT = 0xdb, // Define Quantization Table
		APP0 = 0xe0,
		COM = 0xfe  // Comment
	};

	enum
	{
		EComp_Y=1,
		EComp_Cb=2,
		EComp_Cr=3,
		EComp_I=4,
		EComp_Q=5
	};

//
//
//
bool CJPEGFormat::Load(CInputStream &_rStream, CImage &_rImage)
{
	const bool Log = false;

	if(_rStream.ReadNE<uint8>() != 0xFF)
	{
		NLog::WriteF("Err: not a jpeg\n");
		return false;
	}
	
	if(_rStream.ReadNE<uint8>() != SOI)
	{
		NLog::WriteF("Err: not a jpeg\n");
		return false;
	}
	
	while(1)
	{
		if(_rStream.ReadNE<uint8>() != 0xFF)
		{
			NLog::WriteF("Err: not a segment, %d\n", _rStream.GetPosition());
			return false;
		}

		uint8 segtype = _rStream.ReadNE<uint8>();
		uint16 segsize = _rStream.ReadBE<uint16>();
		
		if(Log)
			NLog::WriteF("%x: %d\n", segtype, segsize);
		
		if(segtype == EOI)
			return true;
		
		if(segtype == SOF0)
		{
			//
			// Start Of Image
			//
			int32 Prec = _rStream.ReadNE<uint8>();
			m_Height = _rStream.ReadBE<uint16>();
			m_Width = _rStream.ReadBE<uint16>();
			m_NumComponents = _rStream.ReadNE<uint8>();
			if(Log)
				NLog::WriteF("\tsize:%dx%dx%x\n", m_Width, m_Height, m_NumComponents);

			int32 maxx = 0, maxy = 0;
			int32 minx = 10, miny = 10;
			for(int32 i = 0; i < m_NumComponents; i++)
			{
				m_aComponents[i].m_ID = _rStream.ReadNE<uint8>();
				m_aComponents[i].m_Factors = _rStream.ReadNE<uint8>();
				m_aComponents[i].m_Quat = _rStream.ReadNE<uint8>();

				if(Log)
				{
					NLog::WriteF("\tc%did:%d\n", i, m_aComponents[i].m_ID);
					NLog::WriteF("\tc%dfactors:%dx%d\n", i, (m_aComponents[i].m_Factors&0xf), m_aComponents[i].m_Factors>>4);
					NLog::WriteF("\tc%dquat:%d\n", i, m_aComponents[i].m_Quat);
				}

				if((m_aComponents[i].m_Factors&0xf) > maxx) maxx = (m_aComponents[i].m_Factors&0xf);
				if((m_aComponents[i].m_Factors>>4) > maxy) maxy = (m_aComponents[i].m_Factors>>4);
				if((m_aComponents[i].m_Factors&0xf) < minx) minx = (m_aComponents[i].m_Factors&0xf);
				if((m_aComponents[i].m_Factors>>4) < miny) miny = (m_aComponents[i].m_Factors>>4);
			}

			m_MCUCount = 0;
			m_NumBlocks = 0;
			m_MCUSizeX = 100000;
			m_MCUSizeY = 100000;

			for(int32 i = 0; i < m_NumComponents; i++)
			{
				int32 x = (m_aComponents[i].m_Factors&0xF);
				int32 y = (m_aComponents[i].m_Factors>>4);
				int32 sx = m_Width/(maxx-x+1);
				int32 sy = m_Height/(maxy-y+1);
				m_NumBlocks += (m_Width/8)*(m_Height/8);
				m_aComponents[i].m_XSizeUps = m_Width/sx;
				m_aComponents[i].m_YSizeUps = m_Height/sy;
				//NLog::WriteF("SAMPLING: %d, %dx%d  %d blcks\n", x*y, sx, sy, (sx/8)*(sy/8));
				//m_aComponentData[i].Create(CImage::CFormat(CImage::EQuick_Yi8, sx, sy));

				if(m_MCUSizeX > sx/8) m_MCUSizeX = sx/8;
				if(m_MCUSizeY > sy/8) m_MCUSizeY = sy/8;
				
			}

			m_MCUCount = m_MCUSizeX*m_MCUSizeY;

			if(Log)
			{
				NLog::WriteF("Blocks: %d\n", m_NumBlocks);
				NLog::WriteF("MCUSize: %dx%d\n", m_MCUSizeX, m_MCUSizeY);
				NLog::WriteF("MCUCount: %d\n", m_MCUCount);
			}
		}
		else if(segtype == DQT)
		{
			//
			// Define Quantization table
			//
			if(Log)
				NLog::WriteF("\tdefine quat table\n");

			int32 bytes = segsize-2;
			int32 i = 0;
			while(bytes)
			{
				uint8 Info = _rStream.ReadNE<uint8>();
				int32 QTnum = Info&0xF;
				int32 Prec = Info>>4;
				uint8 aTemp[64];
				_rStream.Read(aTemp, 64*(Prec+1));

				int32 j = 0;
				for(int32 y = 0; y < 8; y++)
					for(int32 x = 0; x < 8; x++, j++)
						m_aQuatTable[QTnum][j] = aTemp[j];

				if(Log)
				{
					NLog::WriteF("\t%d:qtnum:%d\n", i, QTnum);
					NLog::WriteF("\t%d:prec:%d\n", i, Prec);
				}

				i++;
				bytes -= 1+64*(Prec+1);
			}
		}
		else if(segtype == DHT)
		{
			//
			// Define Huffman Table
			//
			if(Log)
				NLog::WriteF("\tdefine huffman table\n");
			int32 bytes = segsize-2;
			int32 n = 0;
			while(bytes)
			{
				uint8 Info;
				uint8 Counts[16];
				int32 NumEntires = 0;
				
				Info = _rStream.ReadNE<uint8>();
				int32 HT = Info&0xF;
				if(HT > 3)
					throw "HT Error!";
				int32 Type = (Info>>4)&1;
				int32 iTable = HT|(Type<<2);
				
				_rStream.Read(Counts, 16);
				for(int32 i = 0; i < 16; i++)
					NumEntires += Counts[i];

				//NLog::WriteF("FILLING %d\n", iTable);
				int8 aBin[64];
				int32 Current = 0;
				uint32 Code = 0;
				for(int32 i = 1; i < 17; i++)
				{
					for(int32 j = 0; j < Counts[i-1]; j++)
					{
						m_aHuffmanAlphabet[iTable].Add(_rStream.ReadNE<uint8>(), i, Code);
						Code++;
						Current++;
					}
					Code <<= 1;
				}

				//m_aHuffmanAlphabet[iTable].
				
				if(Log)
				{
					NLog::WriteF("\t%dht:%d\n", n, HT);
					NLog::WriteF("\t%dtype:%d\n", n, Type);
					NLog::WriteF("\t%dnum:%d\n", n, Current);
				}
				
				bytes -= Current+16+1;
				n++;
			}
		}
		else if(segtype == SOS)
		{
			//
			// Start of Scans
			//
			uint8 aScanTables[4] = {0};
			int32 aScanIDs[4] = {0};

			uint8 NumScans = _rStream.ReadNE<uint8>();

			// create image
			if(NumScans == 1)
				_rImage.Create(CImage::CFormat(CImage::EQuick_Yi8, m_Width, m_Height));
			else if(NumScans == 3)
				_rImage.Create(CImage::CFormat(CImage::EQuick_YUVi8, m_Width, m_Height));
			else
				throw "Invalid number of scans";

			//MemorySet(_rImage.Data(), 0xcc, m_Width*m_Height);

			if(Log)
				NLog::WriteF("\tscans: %d\n", NumScans);

			// read scan info
			for(int32 i = 0; i < NumScans; i++)
			{
				aScanIDs[i] = _rStream.ReadNE<uint8>();
				aScanTables[i] = _rStream.ReadNE<uint8>();
				for(int32 c = 0; c < m_NumComponents; c++)
				{
					if(m_aComponents[c].m_ID == aScanIDs[i])
					{
						aScanIDs[i] = c;
						m_aComponentDC[c] = 0;
						break;
					}
				}

				if(Log)
				{
					NLog::WriteF("\t%did: %d\n", i, aScanIDs[i]);
					NLog::WriteF("\t%dtable: %d\n", i, aScanTables[i]);
				}
			}
			_rStream.Read(NULL, 3); // ignore?
			
			CJPEGScanReader JpegScanReader(_rStream);
			CBitInputStreamHigh BitStream(JpegScanReader);

			int32 Pitch = _rImage.Format().Pitch();
			int32 bpp = _rImage.Format().BPP()/8;

			for(int32 m = 0; m < m_MCUCount; m++)
			{
				int32 my = m/(m_Width/m_MCUSizeX);
				int32 mx = m%(m_Width/m_MCUSizeX);
				for(int32 s = 0; s < NumScans; s++)
				{
					int32 CompID = aScanIDs[s];
					int32 DCTable = aScanTables[s]>>4;
					int32 ACTable = aScanTables[s]&0xF;
					int32 fx = m_aComponents[CompID].m_Factors&0xF;
					int32 fy = m_aComponents[CompID].m_Factors>>4;
					int32 sx = m_aComponents[CompID].m_XSizeUps;
					int32 sy = m_aComponents[CompID].m_YSizeUps;

					for(int32 y = 0; y < fy; y++)
						for(int32 x = 0; x < fx; x++)
						{
							ReadBlock(BitStream, DCTable, (ACTable)|4, CompID);

							//
							int32 startx = ((m%m_MCUSizeX)*fx*8+x*8)*sx;
							int32 starty = ((m/m_MCUSizeX)*fy*8+y*8)*sy;

							// FIXME!!! prevent memory overwrite here when image isn't a power of 8
							//if(CompID==0)
							for(int32 iy = 0; iy < 8; iy++)
								for(int32 ix = 0; ix < 8; ix++)
								{
									for(int32 siy = 0; siy < sy; siy++)
										for(int32 six = 0; six < sx; six++)
										{
											_rImage.Data()[(starty+iy*sy+siy)*Pitch+(startx+ix*sx+six)*bpp+CompID]
												= m_aVector[iy*8+ix];
										}
								}
						}
				}
			}

			// resize and compose image FIX ME!!!
			/*
			CImage aFinal[2];
			m_aComponentData[1].Sizeup(1, aFinal[0]);
			m_aComponentData[2].Sizeup(1, aFinal[1]);
			_rImage.AddChannel(CImage::EChannel_Y, m_aComponentData[0]);
			_rImage.AddChannel(CImage::EChannel_U, aFinal[0]);
			_rImage.AddChannel(CImage::EChannel_V, aFinal[1]);
			_rImage.ConvertYUVtoRGB(;)
			*/
		}
		else
			_rStream.Read(NULL, segsize-2);
		
	}
	return false;
}

//
//
//
void CJPEGFormat::ReadBlock(CBitInputStreamHigh &rBitStream, int32 iDCTable, int32 iACTable, int32 iComp)
{
	int32 aRawVector[64]= {0};
	fp32 aVector[64] = {0};

	// decode DC
	int32 Diff = 0;
	bool ReadACs = true;
	{
		int32 Header = m_aHuffmanAlphabet[iDCTable].DecodeL(rBitStream);
		int32 Cat = Header&0xf;
		int32 Prev0 = Header>>4;
		if(!Prev0 && !Cat)
		{
			//ReadACs = false;
			aRawVector[0] = m_aComponentDC[iComp];
		}
		else
		{
			int32 Value = rBitStream.Read(Cat); //= HuffmanDecoder.Raw(Cat);
			m_aComponentDC[iComp] += GetValue(Cat, Value);
			aRawVector[0] = m_aComponentDC[iComp];
		}
	}
	
	// decode ACs
	int32 Count = 1;
	int32 ACs = 1;
	while(ACs < 64)
	{
		int32 Header = m_aHuffmanAlphabet[iACTable].DecodeL(rBitStream);
		int32 Cat = Header&0xf;
		int32 Prev0 = Header>>4;

		if(Prev0==0 && Cat==0)
			break;
		if(Prev0==15 && Cat==0)
		{
			Prev0++;
			while(Prev0--)
				aRawVector[ACs++] = 0;
			continue;
		}

		while(Prev0--)
			aRawVector[ACs++] = 0;
		aRawVector[ACs++] = GetValue(Cat, rBitStream.Read(Cat));
	}
	
	while(ACs < 64)
		aRawVector[ACs++] = 0;

	// apply dequatization and reorder
	{
		const int32 *pPattern = g_aZigPattern;
		fp32 *pVector = aVector;
		int32 *pSrcVector = aRawVector;
		fp32 *pQuat = m_aQuatTable[m_aComponents[iComp].m_Quat];
		for(int32 i = 0; i < 64; i++)
		{
			int32 z = *pPattern++;
			*pVector++ = pSrcVector[z]*pQuat[z];
		}
	}

	// IDCT
	fp32 *pDCTLUT = (fp32*)g_aDCTLookup;
	for(int32 y = 0; y < 8; y++)
	{
		for(int32 x = 0; x < 8; x++)
		{
			fp32 *pVector = aVector;
			fp32 Value = 0;
			for(int32 u = 8*8; u > 0; u--)
				Value += *pDCTLUT++ * *pVector++;
			
			fp32 f = Value+128;
			if(f > 255)
				m_aVector[x+(y<<3)] = 255;
			else if(f < 0)
				m_aVector[x+(y<<3)] = 0;
			else
				m_aVector[x+(y<<3)] = (uint8)f;
		}
	}
}
