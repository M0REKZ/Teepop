
#include <foo/func/stream.h>
#include <foo/func/image.h>

#include <foo/ext/compr/zip.h>

using namespace NFoo;

/*

PNG image type			Colour type Allowed bit depths 	Interpretation
Greyscale				0			1, 2, 4, 8*, 16		Each pixel is a greyscale sample
Truecolour				2 			8*, 16				Each pixel is an R,G,B triple
Indexed-colour			3 			1, 2, 4, 8			Each pixel is a palette index; a PLTE chunk shall appear.
Greyscale with alpha	4 			8*, 16				Each pixel is a greyscale sample followed by an alpha sample.
Truecolour with alpha 	6 			8*, 16				Each pixel is an R,G,B triple followed by an alpha sample.

* = supported

*/

//
// http://www.w3.org/TR/PNG/
//
class CPNGFormat : public CImageFormat
{
public:
	static CMime ms_Mime;

	CPNGFormat()
	{
		CMime::AddFileExt("png","image/png");
	}

	bool CanHandle(const CMime &rMime)
	{
		if(rMime == ms_Mime)
			return true;

		return false;
	}
	bool Load(CInputStream &rStream, CImage &rImage);
};
CMime CPNGFormat::ms_Mime("image/png");
MACRO_REGISTER_CLASS(CPNGFormat, "imageformat");

//
// There is a possiblity that there are several
//
class CPNGIDATReader : public CInputStream
{
	CInputStream &m_rSource;
	bool m_FirstTime;
	int32 m_Left;
public:
	CPNGIDATReader(CInputStream &rSource, int32 Left)
	: m_rSource(rSource)
	{
		m_FirstTime = true;
		m_Left = Left;
	}

	bool virtual IsReady() { return m_rSource.IsReady(); }
	bool virtual EndOfStream() { return false; };

	void Flush()
	{
		if(m_Left)
			m_rSource.Read(NULL, m_Left);
		m_Left = 0;
	}
	
	int32 virtual Read(void *pBuffer, int32 Size)
	{
		int32 r = 0;
		uint8 *pDst = (uint8 *)pBuffer;
		while(Size)
		{
			bool NeedMore = Size > m_Left;
			int32 Readlen = NeedMore?m_Left:Size;
			if(Readlen)
			{
				r += m_rSource.Read(pDst, Readlen);
				if(pDst)
					pDst += Readlen;
				Size -= Readlen;
				m_Left -= Readlen;
			}

			if(NeedMore)
			{
				if(!m_FirstTime)
					m_FirstTime = false;
				else
					m_rSource.ReadBE<uint32>(); // CRC
				m_Left = m_rSource.ReadBE<uint32>();
				if(m_rSource.ReadBE<uint32>() != 'IDAT')
					throw "Invalid PNG file. IDAT chunks must";
			}
		}

		return r;
	}
};

template<class T>
T AbsInt(T v)
{
	return (v ^ (v >> (sizeof(T) * 8 - 1))) - (v >> (sizeof(T) * 8 - 1));
}

uint8 PaethPredict(int32 a, int32 b, int32 c)
{
	int32 p = a+b-c;
	int32 pa = AbsInt(p - a);
	int32 pb = AbsInt(p - b);
	int32 pc = AbsInt(p - c);
	if(pa <= pb && pa <= pc)
		return a;
	else if(pb <= pc)
		return b;
	return c;
}

bool CPNGFormat::Load(CInputStream &_rStream, CImage &rImage)
{
	CBlockReader rStream(_rStream);
	const uint8 aSignature[] = {137,'P','N','G',13,10,26,10};

	for(int32 i = 0; i < 8; i++)
		if(rStream.ReadNE<uint8>() != aSignature[i])
			return false;

	uint32 Width = 0;
	uint32 Height = 0;
	int32 bpp = -1;
	int32 Pitch = -1;
	int32 BitDepth = -1;
	int32 ColorType = -1;
	int32 CompressionMethod = -1;
	int32 FilterMethod = -1;
	int32 InterlaceMethod = -1;
			
	while(1)
	{
		if(rStream.EndOfStream())
			break;
		
		uint32 ChunkSize = rStream.ReadBE<uint32>();
		uint32 ChunkType = rStream.ReadBE<uint32>();
		bool bAncillary = (ChunkType>>24) >= 'a';
		bool bPrivate = ((ChunkType>>16)&255) >= 'a';
		bool bReserved = ((ChunkType>>8)&255) >= 'a';
		bool bSafeToCopy = (ChunkType&255) >= 'a';
		//NLog::WriteF("Chunk: %c%c%c%c\n", ChunkType>>24, (ChunkType>>16)&255, (ChunkType>>8)&255, ChunkType&255);
		//NLog::WriteF("\tBits: a=%s p=%s r=%s s=%s\n", bAncillary?"yes":"no",
		//				bPrivate?"yes":"no", bReserved?"yes":"no", bSafeToCopy?"yes":"no");
		
		if(ChunkType == 'IHDR')
		{
			Width = rStream.ReadBE<uint32>();
			Height = rStream.ReadBE<uint32>();
			BitDepth = rStream.ReadBE<uint8>();
			ColorType = rStream.ReadBE<uint8>();
			CompressionMethod = rStream.ReadBE<uint8>();
			FilterMethod = rStream.ReadBE<uint8>();
			InterlaceMethod = rStream.ReadBE<uint8>();
			
			if(ColorType == 0 && BitDepth == 8) // Grayscale, Y8
			{
				rImage.Create(CImage::CFormat(CImage::EQuick_Yi8, Width, Height));
				bpp = 1;
			}
			else if(ColorType == 2 && BitDepth == 8) // Truecolor, RGB8
			{
				rImage.Create(CImage::CFormat(CImage::EQuick_RGBi8, Width, Height));
				bpp = 3;
			}
			else if(ColorType == 4 && BitDepth == 8) // Grayscale with Alpha, Y8A8
			{
				rImage.Create(CImage::CFormat(CImage::EQuick_YAi8, Width, Height));
				bpp = 2;
			}
			else if(ColorType == 6 && BitDepth == 8) // Truecolor with alpha, RGBA8
			{
				rImage.Create(CImage::CFormat(CImage::EQuick_RGBAi8, Width, Height));
				bpp = 4;
			}
			else
				throw "Unable to load. Can't handle this colortype and bitdepth";
		}
		else if(ChunkType == 'IDAT')
		{
			uint8 *pOutputMem = rImage.Data();
			CPNGIDATReader IDATReader(rStream, ChunkSize);
			CZReader ZReader(IDATReader);
		
			if(BitDepth < 0) throw "Unable to load. IDAT chunk before IHDR chunk";
			if(CompressionMethod != 0) throw "Unable to load. Unknown compression method";
			if(FilterMethod != 0) throw "Unable to load. Unknown filter method";

			int32 Pitch = Width*bpp;

			for(int32 i = 0; i < Height; i++)
			{
				int32 FilterType = ZReader.ReadNE<uint8>();
				ZReader.Read(pOutputMem, Width*bpp); // read row
				uint8 *pPrevious = i > 0 ? pOutputMem-Pitch : NULL;
				if(FilterType != 0)
				{
					if(FilterType < 1 || FilterType > 4)
						throw "Unable to load. Unknown PNG filter type";

					for(int32 x = 0; x < Pitch; x++)
					{
						int32 Left = x >= bpp ? pOutputMem[x-bpp] : 0;
						int32 Up = pPrevious ? pPrevious[x] : 0;
						int32 UpLeft = pPrevious && x >= bpp ? pPrevious[x-bpp] : 0;
						int32 Predictor = 0;

						switch(FilterType)
						{
						case 1: Predictor = Left; break;
						case 2: Predictor = Up; break;
						case 3: Predictor = (Left+Up)/2; break;
						case 4: Predictor = PaethPredict(Left, Up, UpLeft); break;
						}

						pOutputMem[x] = (uint8)(pOutputMem[x]+Predictor);
					}
				}

				pOutputMem += Width*bpp;
			}

			IDATReader.Flush(); // discard loose bytes
		}
		else if(ChunkType == 'IEND')
			return true;
		else if(!bAncillary) // check if we can skip this chunk
			throw "Unable to load. Found a critical chunk that I don't understand.";
		else
			rStream.Read(NULL, ChunkSize); // skip chunk
		
		uint32 CRC = rStream.ReadBE<uint32>();
	}

	return false;
}