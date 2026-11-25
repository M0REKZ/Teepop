#include <foo/base/memory.h>
#include <foo/base/log.h>
#include "image.h"
#include "math.h"
#include "vector3.h"
#include "memorystream.h"
#include "bitstream.h"

using namespace NFoo;

//
void CImage::CFormat::Refresh()
{
	m_BPP = 0;
	if(m_Type == EType_Indexed)
		m_BPP = m_IndexInfo.m_Bits;
	else if(m_Type == EType_Channels)
	{
		for(int32_t i = 0; i < NumChannels(); i++)
			m_BPP += ChannelDepth(i);
		for(int32_t i = NumChannels(); i < 4; i++)
		{
			m_ChannelInfo.m_aChannelDepth[i] = 0;
			m_ChannelInfo.m_aChannels[i] = 0xFF;
		}
	}
	else
		m_BPP = 0;

	if(((m_Width*m_BPP)%8) != 0)
		;
		//NLog::WriteF("CImage: Warning pitch couldn't be byte aligned");
	m_Pitch = ((m_Width*m_BPP)+7)/8;
	m_DataSize = m_Pitch*m_Height;
};


CImage::CFormat::CFormat(int32_t i, int32_t w, int32_t h)
{
	MemorySet(this, 0, sizeof(*this));
	QuickSet(i, w, h);
}

void CImage::CFormat::QuickSet(int32_t i, int32_t w, int32_t h)
{
	SetSize(w, h);
	switch(i)
	{
	case EQuick_RGBi8:
		SetType(EType_Channels);
		SetChannelDepths(8,8,8);
		SetChannels(EColorSpace_RGB, EChannel_R, EChannel_G, EChannel_B);
		break;
	case EQuick_RGBAi8:
		SetType(EType_Channels);
		SetChannelDepths(8,8,8,8);
		SetChannels(EColorSpace_RGB, EChannel_R, EChannel_G, EChannel_B, EChannel_A);
		break;
	case EQuick_Yi8:
		SetType(EType_Channels);
		SetChannelDepths(8);
		SetChannels(EColorSpace_GrayScale, EChannel_Y);
		break;
	case EQuick_YAi8:
		SetType(EType_Channels);
		SetChannelDepths(8,8);
		SetChannels(EColorSpace_GrayScale, EChannel_Y, EChannel_A);
		break;
	case EQuick_YUVi8:
		SetType(EType_Channels);
		SetChannelDepths(8,8,8);
		SetChannels(EColorSpace_YUV, EChannel_Y, EChannel_U, EChannel_V);
		break;
	}
	return;
}

//
//
//
void CImage::CFormat::SetType(int32_t Type)
{
	m_Type = Type;
}

//
//
//
void CImage::CFormat::SetSize(int32_t w, int32_t h)
{
	m_Width = w;
	m_Height = h;
	Refresh();
}

//
//
//
void CImage::CFormat::SetChannels(int32_t colorspace, int32_t c0, int32_t c1, int32_t c2, int32_t c3)
{
	if(m_Type != EType_Channels)
		throw "CImage::CFormat::SetChannels: Wrong type.";
	m_ChannelInfo.m_ColorSpace = colorspace;
	m_ChannelInfo.m_aChannels[0] = c0;
	m_ChannelInfo.m_aChannels[1] = c1;
	m_ChannelInfo.m_aChannels[2] = c2;
	m_ChannelInfo.m_aChannels[3] = c3;
	for(int32_t i = 0; i < 4; i++)
		if(m_ChannelInfo.m_aChannels[i] != 0xff)
			m_ChannelInfo.m_NumChannels = i+1;
	Refresh();
}

//
//
//
void CImage::CFormat::SetChannelDepths(int32_t c0, int32_t c1, int32_t c2, int32_t c3)
{
	if(m_Type != EType_Channels)
		throw "CImage::CFormat::SetChannelDepths: Wrong type.";
	m_ChannelInfo.m_aChannelDepth[0] = c0;
	m_ChannelInfo.m_aChannelDepth[1] = c1;
	m_ChannelInfo.m_aChannelDepth[2] = c2;
	m_ChannelInfo.m_aChannelDepth[3] = c3;
	for(int32_t i = 0; i < 4; i++)
		if(m_ChannelInfo.m_aChannelDepth[i] != 0)
			m_ChannelInfo.m_NumChannels = i+1;
	Refresh();
}


//
//
//
CImage::CKernelFilter::CKernelFilter()
{
	MemorySet(m_aFilter,0,sizeof (m_aFilter));

	m_Scale = 1.0f;
	m_Offset = 0.0f;
}

//
CImage::CKernelFilter::CKernelFilter(fp32 _v)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m_aFilter[i][j] = _v;

	m_Scale = 1.0f;
	m_Offset = 0.0f;
}

//
void CImage::CKernelFilter::SetScale(fp32 _Scale)
{
	m_Scale = _Scale;
}

//
void CImage::CKernelFilter::SetOffset(fp32 _Offset)
{
	m_Offset = _Offset;
}
// -----------------------------------

//
// 
//
CImage::CImage()
{
	m_pData = NULL;
}

//
// 
//
CImage::CImage(const CImage &rImage)
{
	m_pData = NULL;

	if(rImage.m_pData)
		Create(rImage);
}

//
// 
//
CImage::CImage(CFormat Format, uint8_t *pData)
{
	m_Format = Format;
	m_pData = pData;
}

//
// 
//
CImage::~CImage()
{
	ClearData();
}

//
//
//
void CImage::operator = (const CImage &rImage)
{
	if(rImage.m_pData)
		Create(rImage);
}

//
// 
//
void CImage::RemoveData()
{
	m_pData = NULL;
}

//
// 
//
void CImage::ClearData()
{
	SafeArrayDelete(m_pData);
}

//
// 
//
bool CImage::Create(CFormat Format)
{
	if(m_Format.DataSize() == Format.DataSize() && m_pData)
	{
		m_Format = Format;
		return true;
	}

	ClearData();
	m_Format = Format;
	m_pData = new uint8_t [m_Format.DataSize()];
	return true;
}

//
//
//
bool CImage::Create(const CImage &rImage)
{
	if(!Create(rImage.m_Format))
		return false;
	MemoryCopy(m_pData, rImage.m_pData, m_Format.DataSize());
	return true;
}

//
//
//
class CFormatHandler
{
	TDynamicArray<CImageFormat*> m_lFormats;

	void Load()
	{
		TDynamicArray<NClassRegistry::CHandle*> lHandles;
		NClassRegistry::FindHandleByTag("imageformat", lHandles);

		for(int32_t i = 0; i < lHandles.Len(); i++)
		{
			CImageFormat *pFormat = (CImageFormat*)NClassRegistry::CreateByHandle(lHandles[i]);
			if(pFormat)
				m_lFormats.Add(pFormat);
		}
	}

public:
	~CFormatHandler()
	{
		m_lFormats.DeleteAll();
	}

	CImageFormat *GetFormat(const CMime &rMime)
	{
		if(!m_lFormats.Len())
			Load();
		for(int32_t i = 0; i < m_lFormats.Len(); i++)
		{
			if(m_lFormats[i]->CanHandle(rMime))
				return m_lFormats[i];
		}
		return NULL;
	}
};

static CFormatHandler g_FormatHandler;

bool CImage::Load(CInputStream &rStream)
{
	CImageFormat *pFormat = g_FormatHandler.GetFormat(rStream.GetMime());
	if(!pFormat)
		return false;
	return pFormat->Load(rStream, *this);
}

bool CImage::Write(COutputStream &rStream)
{
	CImageFormat *pFormat = g_FormatHandler.GetFormat(rStream.GetMime());
	if(!pFormat)
		return false;
	return pFormat->Write(rStream, *this);
}


//
// All bit depth supported
//
void CImage::FlipY()
{
	int32_t Pitch = m_Format.Pitch();
	uint8_t *pBuffer = new uint8_t[Pitch];
	uint8_t *pEnd = (uint8_t *)m_pData + m_Format.Height()*Pitch - Pitch;

	for(int32_t y = 0; y < m_Format.Height()/2; y++)
	{
		MemoryCopy(pBuffer, m_pData+y*Pitch, Pitch);
		MemoryCopy(m_pData+y*Pitch, pEnd-y*Pitch, Pitch);
		MemoryCopy(pEnd-y*Pitch, pBuffer, Pitch);
	}

	SafeArrayDelete(pBuffer);
}
/*
CPixel CImage::GetPixelRGBA(int32_t x, int32_t y)
{
	CPixel Pixel;
	Pixel.c[0] = Pixel.c[1] = Pixel.c[2] = 0;
	Pixel.c[3] = 1;
	if(m_Format.Type() == EType_Channels)
	{
		uint8_t *p = Data()+y*m_Format.Pitch()+x*m_Format.BPP();
		if(m_Format.ColorSpace() == EColorSpace_GrayScale)
		{
			for(int32_t i = 0; i < m_Format.NumChannels(); i++)
			{
				if(m_Format.Channel(i) == EChannel_Y && m_Format.ChannelDepth(i) == 8)
				{
					Pixel.c[0] = *p/255.0f;
					Pixel.c[1] = Pixel.c[2] = Pixel.c[0];
					p++;
				}
				else if(m_Format.Channel(i) == EChannel_A && m_Format.ChannelDepth(i) == 8)
				{
					Pixel.c[0] = *p/255.0f;
					Pixel.c[1] = Pixel.c[2] = Pixel.c[0];
					p++;
				}
			}
		}
	}
	else
		throw "CImage::GetPixelRGBA: unsupported types";
}

void CImage::SetPixelRGBA(int32_t x, int32_t y, CPixel Pixel)
{
	if(m_Format.Type() == EType_Channels)
	{

	}
	else
		throw "CImage::GetPixelRGBA: unsupported types";
}*/

#define FIXEDP(f) ((int32_t)(f*0x7ff))
#define FPSIZE 11

#define A_NONE 0
#define A_DROP 1
#define A_ONE 2
#define A_COPY 3

template<int32_t RC, int32_t GC, int32_t BC, int32_t ALPHAMODE>
void YUV8toRGB8(uint8_t *pSrc, uint8_t *pDst, int32_t w, int32_t h)
{
	for(int32_t y = 0; y < h; y++)
		for(int32_t x = 0; x < w; x++)
		{
			int32_t Y = FIXEDP(*pSrc++);
			int32_t U = FIXEDP(*pSrc++);
			int32_t V = FIXEDP(*pSrc++);
			int32_t R = (int32_t)(Y + ((FIXEDP(1.4075f) * (V - FIXEDP(128)))>>FPSIZE))>>FPSIZE;
			int32_t G = (int32_t)(Y - ((FIXEDP(0.3455f) * (U - FIXEDP(128)))>>FPSIZE) - ((FIXEDP(0.7169) * (V - FIXEDP(128))>>FPSIZE)))>>FPSIZE;
			int32_t B = (int32_t)(Y + ((FIXEDP(1.7790f) * (U - FIXEDP(128)))>>FPSIZE))>>FPSIZE;
			
			R = R&(~(R>>31)); // if(x < 0) x = 0
			G = G&(~(G>>31));
			B = B&(~(B>>31));
			pDst[RC] = R > 0xff ? 0xff : R;
			pDst[GC] = G > 0xff ? 0xff : G;
			pDst[BC] = B > 0xff ? 0xff : B;
			pDst += 3;
			if(ALPHAMODE == A_ONE)
				*pDst++ = 0xff;
			else if(ALPHAMODE == A_COPY)
				*pDst++ = *pSrc++;
			else if(ALPHAMODE == A_DROP)
				*pSrc++;
		}
}


//
//
//
template<int32_t RC, int32_t GC, int32_t BC, int32_t ALPHAMODE>
void RGB8toYUV8(uint8_t *pSrc, uint8_t *pDst, int32_t w, int32_t h)
{
	for(int32_t y = 0; y < h; y++)
		for(int32_t x = 0; x < w; x++)
		{
			int32_t R = FIXEDP(pSrc[RC]);
			int32_t G = FIXEDP(pSrc[GC]);
			int32_t B = FIXEDP(pSrc[BC]);
			pSrc+=3;
			*pDst++ = (uint8_t)(((R * FIXEDP(0.299f)>>FPSIZE) + (G * FIXEDP(0.587f)>>FPSIZE) + (B *  FIXEDP(0.114f)>>FPSIZE))>>FPSIZE);
			*pDst++ = (uint8_t)(((R * FIXEDP(-0.169f)>>FPSIZE) + (G * FIXEDP(-0.332f)>>FPSIZE) + (B * FIXEDP(0.500f)>>FPSIZE) + FIXEDP(128.0f))>>FPSIZE);
			*pDst++ = (uint8_t)(((R * FIXEDP(0.500f)>>FPSIZE) + (G * FIXEDP(-0.419f)>>FPSIZE) + (B * FIXEDP(-0.0813f)>>FPSIZE) + FIXEDP(128.0f))>>FPSIZE);
			pDst += 3;
			if(ALPHAMODE == A_ONE)
				*pDst++ = 0xff;
			else if(ALPHAMODE == A_COPY)
				*pDst++ = *pSrc++;
			else if(ALPHAMODE == A_DROP)
				*pSrc++;
		}
}

//
//
//
template<int32_t ALPHAMODE>
void RGB8toY8(uint8_t *pSrc, uint8_t *pDst, int32_t w, int32_t h)
{
	for(int32_t y = 0; y < h; y++)
		for(int32_t x = 0; x < w; x++)
		{
			int32_t Value = *pSrc++;
			Value += *pSrc++;
			Value += *pSrc++;
			Value /= 3;
			*pDst++ = Value;
			if(ALPHAMODE == A_ONE)
				*pDst++ = 0xff;
			else if(ALPHAMODE == A_COPY)
				*pDst++ = *pSrc++;
			else if(ALPHAMODE == A_DROP)
				*pSrc++;
		}
}


//
//
//
template<int32_t C0, int32_t C1, int32_t C2, int32_t ALPHAMODE>
void SwizzleX8(uint8_t *pSrc, uint8_t *pDst, int32_t w, int32_t h)
{
	for(int32_t x = 0; x < w*h; x++)
	{
		pDst[0] = pSrc[C0];
		pDst[1] = pSrc[C1];
		pDst[2] = pSrc[C2];
		pSrc+=3;
		pDst+=3;
		if(ALPHAMODE == A_ONE)
			*pDst++ = 0xff;
		else if(ALPHAMODE == A_COPY)
			*pDst++ = *pSrc++;
		else if(ALPHAMODE == A_DROP)
			*pSrc++;
	}
}

typedef void (*PFNImageConverter)(uint8_t *pSrc, uint8_t *pDst, int32_t w, int32_t h);

CImage::CFormat iFormat(int32_t ColorSpace, uint32_t Depth, uint32_t Channels)
{
	CImage::CFormat Format;
	Format.SetType(CImage::EType_Channels);
	Format.SetChannels(ColorSpace, (Channels>>24)&0xff,(Channels>>16)&0xff,(Channels>>8)&0xff,Channels&0xff);
	Format.SetChannelDepths((Depth>>24)&0xff,(Depth>>16)&0xff,(Depth>>8)&0xff,Depth&0xff);
	return Format;
}

CImage::CFormat RGBiFormat(uint32_t Depth, uint32_t Channels)
{
	return iFormat(CImage::EColorSpace_RGB, Depth, Channels);
}

CImage::CFormat YUViFormat(uint32_t Depth, uint32_t Channels)
{
	return iFormat(CImage::EColorSpace_YUV, Depth, Channels);
}

CImage::CFormat GrayiFormat(uint32_t Depth, uint32_t Channels)
{
	return iFormat(CImage::EColorSpace_GrayScale, Depth, Channels);
}

class CConverter
{
public:
	CImage::CFormat m_SrcFormat;
	CImage::CFormat m_DstFormat;
	PFNImageConverter m_pfnFunc;

	bool MatchFormat(CImage::CFormat &FormatA, CImage::CFormat &FormatB)
	{
		if(FormatA.Type() != FormatB.Type())
			return false;
		if(FormatA.Type() == CImage::EType_Channels)
		{
			if(FormatA.BPP() != FormatB.BPP())
				return false;
			if(FormatA.ColorSpace() != FormatB.ColorSpace())
				return false;
			if(FormatA.Type() != FormatB.Type())
				return false;
			if(FormatA.ChannelDepths() != FormatB.ChannelDepths())
				return false;
			if(FormatA.Channels() != FormatB.Channels())
				return false;
			if(FormatA.ChannelType() != FormatB.ChannelType())
				return false;
		}
		else
			return false;

		return true;
	}

	bool Match(CImage::CFormat &rSrcFormat, CImage::CFormat &rDstFormat)
	{
		if(!MatchFormat(m_SrcFormat, rSrcFormat))
			return false;
		if(!MatchFormat(m_DstFormat, rDstFormat))
			return false;
		return true;
	}
};

PFNImageConverter GetConverter(CImage::CFormat SrcFormat, CImage::CFormat DstFormat)
{
	static CConverter aTable[] = {
	{RGBiFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080800, 0x020100FF), SwizzleX8<2,1,0,A_NONE>},		// RGB -> BGR
	{RGBiFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080808, 0x00010203), SwizzleX8<0,1,2,A_ONE>},		// RGB -> RGBA
	{RGBiFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080808, 0x02010003), SwizzleX8<2,1,0,A_ONE>},		// RGB -> BGRA
	{RGBiFormat(0x08080800, 0x000102FF), YUViFormat(0x08080800, 0x000102FF), RGB8toYUV8<0,1,2,A_NONE>},		// RGB -> YUV
	{RGBiFormat(0x08080800, 0x000102FF), YUViFormat(0x08080808, 0x00010203), RGB8toYUV8<0,1,2,A_ONE>},		// RGB -> YUVA
	{RGBiFormat(0x08080800, 0x000102FF),GrayiFormat(0x08000000, 0x00FFFFFF), RGB8toY8<A_NONE>},				// RGB -> Y
	{RGBiFormat(0x08080800, 0x000102FF),GrayiFormat(0x08080000, 0x0003FFFF), RGB8toY8<A_ONE>},				// RGB -> YA

	{RGBiFormat(0x08080800, 0x020100FF), RGBiFormat(0x08080800, 0x000102FF), SwizzleX8<2,1,0,A_NONE>},		// BGR -> RGB
	{RGBiFormat(0x08080800, 0x020100FF), RGBiFormat(0x08080808, 0x00010203), SwizzleX8<2,1,0,A_ONE>},		// BGR -> RGBA
	{RGBiFormat(0x08080800, 0x020100FF), RGBiFormat(0x08080808, 0x02010003), SwizzleX8<0,1,2,A_ONE>},		// BGR -> BGRA
	{RGBiFormat(0x08080800, 0x020100FF), YUViFormat(0x08080800, 0x000102FF), RGB8toYUV8<2,1,0,A_NONE>},		// BGR -> YUV
	{RGBiFormat(0x08080800, 0x020100FF), YUViFormat(0x08080808, 0x00010203), RGB8toYUV8<2,1,0,A_ONE>},		// BGR -> YUVA
	{RGBiFormat(0x08080800, 0x020100FF),GrayiFormat(0x08000000, 0x00FFFFFF), RGB8toY8<A_NONE>},				// BGR -> Y
	{RGBiFormat(0x08080800, 0x020100FF),GrayiFormat(0x08080000, 0x0003FFFF), RGB8toY8<A_ONE>},				// BGR -> YA

	{RGBiFormat(0x08080808, 0x00010203), RGBiFormat(0x08080800, 0x000102FF), SwizzleX8<0,1,2,A_DROP>},		// RGBA -> RGB
	{RGBiFormat(0x08080808, 0x00010203), RGBiFormat(0x08080800, 0x020100FF), SwizzleX8<2,1,0,A_DROP>},		// RGBA -> BGR
	{RGBiFormat(0x08080808, 0x00010203), RGBiFormat(0x08080808, 0x02010003), SwizzleX8<2,1,0,A_COPY>},		// RGBA -> BGRA
	{RGBiFormat(0x08080808, 0x00010203), YUViFormat(0x08080800, 0x000102FF), RGB8toYUV8<0,1,2,A_DROP>},		// RGBA -> YUV
	{RGBiFormat(0x08080808, 0x00010203), YUViFormat(0x08080808, 0x00010203), RGB8toYUV8<0,1,2,A_COPY>},		// RGBA -> YUVA
	{RGBiFormat(0x08080808, 0x00010203),GrayiFormat(0x08000000, 0x00FFFFFF), RGB8toY8<A_DROP>},				// RGBA -> Y
	{RGBiFormat(0x08080808, 0x00010203),GrayiFormat(0x08080000, 0x0003FFFF), RGB8toY8<A_COPY>},				// RGBA -> YA

	{RGBiFormat(0x08080808, 0x02010003), RGBiFormat(0x08080800, 0x000102FF), SwizzleX8<2,1,0,A_DROP>},		// BGRA -> RGB
	{RGBiFormat(0x08080808, 0x02010003), RGBiFormat(0x08080800, 0x020100FF), SwizzleX8<0,1,2,A_DROP>},		// BGRA -> BGR
	{RGBiFormat(0x08080808, 0x02010003), RGBiFormat(0x08080808, 0x00010203), SwizzleX8<2,1,0,A_COPY>},		// BGRA -> RGBA
	{RGBiFormat(0x08080808, 0x02010003), RGBiFormat(0x08080808, 0x02010003), SwizzleX8<0,1,2,A_COPY>},		// BGRA -> BGRA
	{RGBiFormat(0x08080808, 0x02010003), YUViFormat(0x08080800, 0x000102FF), RGB8toYUV8<2,1,0,A_DROP>},		// RGBA -> YUV
	{RGBiFormat(0x08080808, 0x02010003), YUViFormat(0x08080808, 0x00010203), RGB8toYUV8<2,1,0,A_COPY>},		// RGBA -> YUVA
	{RGBiFormat(0x08080808, 0x02010003),GrayiFormat(0x08000000, 0x00FFFFFF), RGB8toY8<A_DROP>},				// BGRA -> Y
	{RGBiFormat(0x08080808, 0x02010003),GrayiFormat(0x08080000, 0x0003FFFF), RGB8toY8<A_COPY>},				// BGRA -> YA

	{YUViFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080800, 0x000102FF), YUV8toRGB8<0,1,2,A_NONE>},		// YUV -> RGB
	{YUViFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080800, 0x020100FF), YUV8toRGB8<2,1,0,A_NONE>},		// YUV -> BGR
	{YUViFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080808, 0x00010203), YUV8toRGB8<0,1,2,A_ONE>},		// YUV -> RGBA
	{YUViFormat(0x08080800, 0x000102FF), RGBiFormat(0x08080808, 0x02010003), YUV8toRGB8<2,1,0,A_ONE>},		// YUV -> BGRA
	{YUViFormat(0x08080800, 0x000102FF), YUViFormat(0x08080808, 0x00010203), SwizzleX8<0,1,2,A_ONE>},		// YUV -> YUVA

	{YUViFormat(0x08080808, 0x00010203), RGBiFormat(0x08080800, 0x000102FF), YUV8toRGB8<0,1,2,A_NONE>},		// YUVA -> RGB
	{YUViFormat(0x08080808, 0x00010203), RGBiFormat(0x08080800, 0x020100FF), YUV8toRGB8<2,1,0,A_NONE>},		// YUVA -> BGR
	{YUViFormat(0x08080808, 0x00010203), RGBiFormat(0x08080808, 0x00010203), YUV8toRGB8<0,1,2,A_COPY>},		// YUVA -> RGBA
	{YUViFormat(0x08080808, 0x00010203), RGBiFormat(0x08080808, 0x02010003), YUV8toRGB8<2,1,0,A_COPY>},		// YUVA -> BGRA
	{YUViFormat(0x08080808, 0x00010203), YUViFormat(0x08080800, 0x000102FF), SwizzleX8<0,1,2,A_DROP>},		// YUVA -> YUV
	};
	
	for(int32_t i = 0; i < sizeof(aTable)/sizeof(CConverter); i++)
	{
		if(aTable[i].Match(SrcFormat, DstFormat))
			return aTable[i].m_pfnFunc;
	}
	return NULL;
}


//
//
//
CImage::CPixel CImage::GetPixelRGBA(int32_t x, int32_t y)
{
	CMemoryReader Stream(Data()+x*m_Format.BPP()+y*m_Format.m_Pitch);
	CBitInputStream BitStream(Stream);

	// read pixel
	CPixel Pixel;
	Pixel.c[0] = Pixel.c[1] = Pixel.c[2] = 0;
	Pixel.c[3] = 1;

	for(int32_t c = 0; c < m_Format.NumChannels(); c++)
	{
		int32_t b = BitStream.Read(m_Format.ChannelDepth(c));
		Pixel.c[m_Format.Channel(c)] = b/(fp32)((1<<m_Format.ChannelDepth(c))-1)*255.0f;
	}

	// convert from src colorspace to rgb
	if(m_Format.ColorSpace() == EColorSpace_GrayScale)
		Pixel.c[1] = Pixel.c[2] = Pixel.c[0];
	else if(m_Format.ColorSpace() == EColorSpace_YUV)
	{
		uint8_t Y = (uint8_t)Pixel.c[0];	// YUV -> RGB
		uint8_t U = (uint8_t)Pixel.c[1];
		uint8_t V = (uint8_t)Pixel.c[2];
		int32_t R = (int32_t)(Y + (1.4075f * (V - 128)));
		int32_t G = (int32_t)(Y - 0.3455f * (U - 128) - (0.7169 * (V - 128)));
		int32_t B = (int32_t)(Y + (1.7790f * (U - 128)));
		Pixel.c[0] = R > 255 ? 255 : R < 0 ? 0 : R;
		Pixel.c[1] = G > 255 ? 255 : G < 0 ? 0 : G;
		Pixel.c[2] = B > 255 ? 255 : B < 0 ? 0 : B;
	}

	return Pixel;
}

void CImage::SetPixelRGBA(int32_t x, int32_t y, CPixel Pixel)
{
	CMemoryWriter Stream(Data()+x*m_Format.BPP()+y*m_Format.m_Pitch);
	CBitOutputStream BitStream(Stream);	// convert from rgb to dst colorspace

	if(m_Format.ColorSpace() == EColorSpace_GrayScale)
		Pixel.c[0] = (Pixel.c[0]+Pixel.c[1]+Pixel.c[2])/3;
	else if(m_Format.ColorSpace() == EColorSpace_YUV)
	{
		fp32 R = Pixel.c[0]; // RGB -> YUV
		fp32 G = Pixel.c[1];
		fp32 B = Pixel.c[2];
		Pixel.c[0] = (uint8_t)(R *  0.299f + G *  0.587f + B *  0.114f);
		Pixel.c[1] = (uint8_t)(R * -0.169f + G * -0.332f + B *  0.500f + 128.0f);
		Pixel.c[2] = (uint8_t)(R *  0.500f + G * -0.419f + B * -0.0813f + 128.0f);
	}

	// write pixel
	for(int32_t c = 0; c < m_Format.NumChannels(); c++)
		BitStream.Write((int32_t)(Pixel.c[m_Format.Channel(c)]/255.0f*((1<<m_Format.ChannelDepth(c))-1)),
						m_Format.ChannelDepth(c));
}

//
//
//
void CImage::Convert(CFormat Format)
{
	CImage Dest;
	Convert(Format,Dest);
	*this = Dest;
}

//
//
//
void CImage::Convert(CFormat Format, CImage &rImage, int32_t Flags)
{
	bool ColorSpace = false;
	bool ChannelSwizzle = false; // RGB -> BGR
	bool ChannelExtend = false; //  RGB -> RGBA
	bool PrecisionChange = false;
	bool ByteSwap = false;
	//bool Resample = false;
	bool TypeChange = false;

	Format.SetSize(m_Format.Width(), m_Format.Height());
	rImage.Create(Format);

	//if(m_Format.Height() != Format.Height() || m_Format.Width() != Format.Width())
	//	Resample = true;

	if(m_Format.Type() != Format.Type())
		TypeChange = true;
	else if(m_Format.Type() == EType_Channels)
	{
		if(m_Format.ColorSpace() != Format.ColorSpace())
			ColorSpace = true;
		else if(m_Format.NumChannels() != Format.NumChannels())
			ChannelExtend = true;
		else
		{
			for(int32_t i = 0; i < m_Format.NumChannels(); i++)
			{
				if(m_Format.Channel(i) != Format.Channel(i))
					ChannelSwizzle = true;
				if(m_Format.ChannelDepth(i) != Format.ChannelDepth(i))
					PrecisionChange = true;
			}
		}
	}

	if(TypeChange)
	{
		throw "CImage::Convert: Type convertion isn't implementeded";
	}
	else
	{
		bool UseGeneric = true;

		PFNImageConverter pfnConvFunc = GetConverter(m_Format, Format);
		if(pfnConvFunc)
		{
			pfnConvFunc(Data(), rImage.Data(), m_Format.Width(), m_Format.Height());
			UseGeneric = false;
		}

		// generic, can handle alot of shit.. but it ain't fast.
		if(UseGeneric && (ColorSpace || ChannelSwizzle || PrecisionChange))
		{
			// VERY VERY GENERIC, slow as hell but gets the job done
			int64_t StartTime = GetTickCount();
			//CImage &DstImage = rImage;
			CFormat DstFormat = Format;
			/*
			DstFormat.SetChannelDepths(	Format.ChannelDepth(0), Format.ChannelDepth(1),
										Format.ChannelDepth(2), Format.ChannelDepth(3));
			DstFormat.SetChannels(Format.ColorSpace(),	Format.Channel(0), Format.Channel(1),
														Format.Channel(2), Format.Channel(3));
			DstImage.Create(DstFormat);*/

			CMemoryReader SrcStream(Data());
			CBitInputStream SrcBitStream(SrcStream);
			CMemoryWriter DstStream(rImage.Data());
			CBitOutputStream DstBitStream(DstStream);

			for(int32_t y = 0; y < m_Format.Height(); y++)
				for(int32_t x = 0; x < m_Format.Width(); x++)
				{
					// read pixel
					CPixel Pixel;
					Pixel.c[0] = Pixel.c[1] = Pixel.c[2] = 0;
					Pixel.c[3] = 255;

					for(int32_t c = 0; c < m_Format.NumChannels(); c++)
					{
						int32_t b = SrcBitStream.Read(m_Format.ChannelDepth(c));
						Pixel.c[m_Format.Channel(c)] = b/(fp32)((1<<m_Format.ChannelDepth(c))-1)*255.0f;
					}

					// convert from src colorspace to rgb
					if(m_Format.ColorSpace() == EColorSpace_GrayScale)
					{
						if(m_Format.NumChannels() == 2)
							Pixel.c[3] = Pixel.c[1];
						Pixel.c[1] = Pixel.c[2] = Pixel.c[0];
					}
					else if(m_Format.ColorSpace() == EColorSpace_YUV)
					{
						uint8_t Y = (uint8_t)Pixel.c[0];	// YUV -> RGB
						uint8_t U = (uint8_t)Pixel.c[1];
						uint8_t V = (uint8_t)Pixel.c[2];
						int32_t R = (int32_t)(Y + (1.4075f * (V - 128)));
						int32_t G = (int32_t)(Y - 0.3455f * (U - 128) - (0.7169 * (V - 128)));
						int32_t B = (int32_t)(Y + (1.7790f * (U - 128)));
						Pixel.c[0] = R > 255 ? 255 : R < 0 ? 0 : R;
						Pixel.c[1] = G > 255 ? 255 : G < 0 ? 0 : G;
						Pixel.c[2] = B > 255 ? 255 : B < 0 ? 0 : B;
					}

					// convert from rgb to dst colorspace
					if(DstFormat.ColorSpace() == EColorSpace_GrayScale)
						Pixel.c[0] = (Pixel.c[0]+Pixel.c[1]+Pixel.c[2])/3;
					else if(DstFormat.ColorSpace() == EColorSpace_YUV)
					{
						fp32 R = Pixel.c[0]; // RGB -> YUV
						fp32 G = Pixel.c[1];
						fp32 B = Pixel.c[2];
						Pixel.c[0] = (uint8_t)(R *  0.299f + G *  0.587f + B *  0.114f);
						Pixel.c[1] = (uint8_t)(R * -0.169f + G * -0.332f + B *  0.500f + 128.0f);
						Pixel.c[2] = (uint8_t)(R *  0.500f + G * -0.419f + B * -0.0813f + 128.0f);
					}

					// write pixel
					for(int32_t c = 0; c < DstFormat.NumChannels(); c++)
						DstBitStream.Write((int32_t)(Pixel.c[DstFormat.Channel(c)]/255.0f*((1<<DstFormat.ChannelDepth(c))-1)),
								DstFormat.ChannelDepth(c));
				}

			DstBitStream.Flush();
			fp32 WastedTime = (GetTickCount()-StartTime)/(fp32)GetTickFrequency();
			NLog::WriteF("Performace Warning: CImage::Convert used generic rutine for convertion. Lost %.5fs\n", WastedTime);
		}
		else
		{
			if(ByteSwap)
				throw "CImage::Convert: Byteswap convertion isn't implementeded";
		}
	}
}

/*
void CImage::ExtractChannel(int32_t iChannel, CImage &rChannel)
{
}*/

//
// Clearing to color 0 supports all bitdepth, else only 32 bit is supported
//
/*
void CImage::Clear(uint32_t _Color)
{
	//ASSUME(m_pData);

	if(m_Desc.m_Format == EFormat_BW)
	{
		MemorySet(m_pData, _Color, m_Desc.m_Size.y*m_Desc.m_Pitch);
	}
	else if(m_Desc.m_Format == EFormat_RGBA32)
	{
		uint32_t *pDat = (uint32_t *)m_pData;
		for(int32_t c = m_Desc.m_Size.y*m_Desc.m_Size.x; c > 0; c--)
			*pDat++ = _Color;
	}
	else
		throw ("Unimplemented Color Format, (Clear to color)");
}*/


//
// Doesn't support Blitting between diffrent formats
// Should be optimized
//
/*
void CImage::Blit(CImage *_pSrc, int32_t _x, int32_t _y)
{
	if(_pSrc->m_Desc.m_Format != m_Desc.m_Format)
		throw "Can't perform blit between diffrent formats";

	//
	// Get number of rows to copy
	//
	/*
	int32_t Rows = Src->m_Format.m_Size.y;

	if(m_Format.m_Size.y < Rows)
		Rows = m_Format.m_Size.y;

	if(Position.y + Rows > m_Format.m_Size.y)
		Rows = m_Format.m_Size.y - Position.y;

	if(Rows < 0)
		return;
	
	//if(m_Format.m_Size.x*m_Format.m_Size.y < Src->m_Format.m_Size.x*Src->m_Format.m_Size.y)
	//{
	int32_t DstPosY = _y;
	int32_t SrcPosY = 0;
	int32_t DstPosX = _x;
	int32_t SrcPosX = 0;
	int32_t Width = _pSrc->m_Desc.m_Size.x;
	
	if(DstPosY < 0)
	{
		SrcPosY = -DstPosY;
		DstPosY = 0;
	}

	if(DstPosX < 0)
	{
		SrcPosX = -DstPosX;
		DstPosX = 0;
	}

	if(Width+DstPosX > m_Desc.m_Size.x)
		Width = m_Desc.m_Size.x - DstPosX;

	if(Width+SrcPosX > _pSrc->m_Desc.m_Size.x)
		Width = _pSrc->m_Desc.m_Size.x - SrcPosX;

	uint8_t *pDstData = m_pData + DstPosY*m_Desc.m_Pitch + DstPosX*(m_Desc.m_Format>>3);
	uint8_t *pSrcData = _pSrc->m_pData + SrcPosY*_pSrc->m_Desc.m_Pitch + SrcPosX*(_pSrc->m_Desc.m_Format>>3);
	int32_t SrcPitch = (_pSrc->m_Desc.m_Size.x - Width)*(_pSrc->m_Desc.m_Format>>3);
	int32_t DstPitch = (m_Desc.m_Size.x - Width)*(m_Desc.m_Format>>3);
	int32_t NumBytes = Width*(m_Desc.m_Format>>3);
	
	for(; DstPosY < m_Desc.m_Size.y && SrcPosY < _pSrc->m_Desc.m_Size.y; DstPosY++, SrcPosY++)
	{
		MemoryCopy(pDstData, pSrcData, NumBytes);
		pSrcData += (SrcPitch+NumBytes);
		pDstData += (DstPitch+NumBytes);
	}

	//}
	//else
	//{

	//}
}

//
// 24bit and 32bit support, plz opt me
//
/*
void CImage::ConvertRGBtoYUV()
{
	register uint8_t *pPtr = m_pData;
	register int32_t Count = m_Desc.m_Size.x*m_Desc.m_Size.y;
	register uint32_t bpp = m_Desc.m_Format/8;

	while(Count--)
	{
		uint8_t R = pPtr[EChannel_R];
		uint8_t G = pPtr[EChannel_G];
		uint8_t B = pPtr[EChannel_B];

		pPtr[EChannel_Y] = (uint8_t)(R *  0.299f + G *  0.587f + B *  0.114f);
		pPtr[EChannel_U] = (uint8_t)(R * -0.169f + G * -0.332f + B *  0.500f + 128.0f);
		pPtr[EChannel_V] = (uint8_t)(R *  0.500f + G * -0.419f + B * -0.0813f + 128.0f);
		pPtr+=bpp;
	}
}

//
// 24bit and 32bit support, plz opt me
//

//
/*
void CImage::ConvertYUVtoRGB()
{
	register uint8_t *pPtr = m_pData;
	register int32_t Count = m_Desc.m_Size.x*m_Desc.m_Size.y;
	register uint32_t bpp = m_Desc.m_Format/8;

	while(Count--)
	{
		uint8_t Y = pPtr[EChannel_Y];
		uint8_t U = pPtr[EChannel_U];
		uint8_t V = pPtr[EChannel_V];
		int32_t R = (int32_t)(Y + (1.4075f * (V - 128)));
		int32_t G = (int32_t)(Y - 0.3455f * (U - 128) - (0.7169 * (V - 128)));
		int32_t B = (int32_t)(Y + (1.7790f * (U - 128)));
		pPtr[EChannel_R] = R > 255 ? 255 : R < 0 ? 0 : R;
		pPtr[EChannel_G] = G > 255 ? 255 : G < 0 ? 0 : G;
		pPtr[EChannel_B] = B > 255 ? 255 : B < 0 ? 0 : B;
		pPtr+=bpp;
	}
}
//		virtual void Sizedown(int32_t _Steps);
//		virtual void Sizeup(int32_t _Steps);

//
//
//
/*
void CImage::Sizeup(int32_t _Steps, CImage &_rDst)
{
	int32_t Factor = (1<<_Steps);
	_rDst.Create(m_Desc.m_Size*Factor, m_Desc.m_Format);

	register uint8_t *pSrcPtr = m_pData;
	register uint8_t *pDstPtr = _rDst.m_pData;
	register uint32_t bpp = m_Desc.m_Format/8;
	register uint32_t spitch = m_Desc.m_Pitch;
	register uint32_t dpitch = _rDst.m_Desc.m_Pitch;

	if(bpp == 1)
	{
		// fix this.
		for(int32_t y = 0; y < m_Desc.m_Size.y; y++)
		{
			uint8_t *pNextRow = pSrcPtr+spitch;
			if(y+1 == m_Desc.m_Size.y)
				pNextRow = pSrcPtr;

			for(int32_t x = 0; x < m_Desc.m_Size.x; x++)
			{
				uint8_t r00 = *pSrcPtr;
				uint8_t r10 = r00;
				uint8_t r01 = *(pNextRow);
				uint8_t r11 = r01;

				if(x+1 != m_Desc.m_Size.x)
				{
					r10 = *(pSrcPtr+bpp);
					r11 = *(pNextRow+bpp);
				}

				for(int32_t iy = 0; iy < Factor; iy++)
				{
					fp32 yf = iy/(fp32)Factor;
					uint8_t y0r = Lerp(r00, r01, yf);
					uint8_t y1r = Lerp(r10, r11, yf);
					for(int32_t ix = 0; ix < Factor; ix++)
					{
						fp32 xf = ix/(fp32)Factor;
						int32_t e = (x*Factor+ix)*bpp+(y*Factor+iy)*dpitch;
						*(pDstPtr + e) = Lerp(y0r, y1r, xf);
					}
				}

				pSrcPtr+=bpp;
			}
		}
	}
	throw "CImage::Sizeup: Bpp must be 1";

}
*/
/*
//
//
//
void CImage::Sizedown(int32_t _Steps, CImage &_rDst)
{
	int32_t Factor = (1<<_Steps);
	_rDst.Create(m_Desc.m_Size/Factor, m_Desc.m_Format);
	
	register uint8_t *pSrcPtr = m_pData;
	register uint8_t *pDstPtr = _rDst.m_pData;
	register uint32_t bpp = m_Desc.m_Format/8;
	register uint32_t pitch = m_Desc.m_Pitch;

	for(int32_t y = 0; y < _rDst.m_Desc.m_Size.y; y++)
		for(int32_t x = 0; x < _rDst.m_Desc.m_Size.x; x++)
		{
			int32_t R = 0;
			int32_t G = 0;
			int32_t B = 0;
			for(int32_t iy = 0; iy < Factor; iy++)
				for(int32_t ix = 0; ix < Factor; ix++)
				{
					int32_t e = (x*Factor+ix)*bpp+(y*Factor+iy)*pitch;
					R += *(pSrcPtr + e + EChannel_R);
					G += *(pSrcPtr + e + EChannel_G);
					B += *(pSrcPtr + e + EChannel_B);
				}
			R /= Factor*Factor;
			G /= Factor*Factor;
			B /= Factor*Factor;
			
			pDstPtr[EChannel_R] = R;
			pDstPtr[EChannel_G] = G;
			pDstPtr[EChannel_B] = B;
			pDstPtr+=bpp;
		}
}

//
//
//
void CImage::ExtractChannel(int32_t _iChannel, CImage &_rChannel)
{
	_rChannel.Create(m_Desc.m_Size, EFormat_BW);

	register int32_t Count = m_Desc.m_Size.x*m_Desc.m_Size.y; // pixel count
	register uint8_t *pSrcPtr = m_pData+_iChannel;
	register uint8_t *pDstPtr = _rChannel.m_pData;
	register uint32_t bpp = m_Desc.m_Format/8;
	while(Count--)
	{
		*pDstPtr++ = *pSrcPtr;
		pSrcPtr += bpp;
	}
}

void CImage::AddChannel(int32_t _iChannel, CImage &_rChannel)
{
	register int32_t Count = m_Desc.m_Size.x*m_Desc.m_Size.y; // pixel count
	register uint8_t *pSrcPtr = _rChannel.m_pData;
	register uint8_t *pDstPtr = m_pData+_iChannel;
	register uint32_t bpp = m_Desc.m_Format/8;
	while(Count--)
	{
		*pDstPtr = *pSrcPtr++;
		pDstPtr += bpp;
	}
}

//
// 24, 32 supported
//
void CImage::FlipRB()
{
//	ASSUME(m_pData);
	int32_t Count = m_Desc.m_Size.x*m_Desc.m_Size.y;
	int32_t Jump = m_Desc.m_Format/8;
	uint8_t *pPtr = m_pData;

	if(Jump == 3 || Jump == 4)
	{
		while(Count > 0)
		{
			uint8_t Temp = *pPtr;
			*pPtr = pPtr[2];
			pPtr[2] = Temp;

			pPtr += Jump;
			Count--;
		}
	}
	else if(m_Desc.m_Format == EFormat_RGB15)
	{

	}
}

//
// 32bit ONLY! FIX!
//
void CImage::FlipX()
{
//	ASSUME(m_pData);

	int32_t Temp;
	int32_t *pPtr = (int32_t *)m_pData;

	for(int32_t y = 0; y < m_Desc.m_Size.y; y++)
		for(int32_t x = 0; x < m_Desc.m_Size.x/2; x++)
		{
			Temp = pPtr[y*m_Desc.m_Size.x+x];
			pPtr[y*m_Desc.m_Size.x+x] = pPtr[y*m_Desc.m_Size.x+(m_Desc.m_Size.x-x-1)];
			pPtr[y*m_Desc.m_Size.x+(m_Desc.m_Size.x-x-1)] = Temp;
		}

}


//
// Image must be square like and 32bit. fix!
//
void CImage::RotCCW90()
{
	if(m_Desc.m_Size.x != m_Desc.m_Size.y)
		return;

//	ASSUME(m_pData);

	uint32_t *pPtr = (uint32_t *)m_pData;
	uint32_t *pBuffer = new uint32_t[m_Desc.m_Size.x*m_Desc.m_Size.y];
	MemoryCopy(pBuffer, m_pData, m_Desc.m_Size.x*m_Desc.m_Size.y*sizeof(pBuffer));

	for(int32_t y = 0; y < m_Desc.m_Size.y; y++)
		for(int32_t x = 0; x < m_Desc.m_Size.x; x++)
		{
			pPtr[x*m_Desc.m_Size.y+(m_Desc.m_Size.x-y-1)] = pBuffer[y*m_Desc.m_Size.y+x];
		}

	SafeArrayDelete(pBuffer);
}

//
// 24 to 32 is the only that is supported
//
void CImage::SetFormat(int32_t _NewFormat)
{
	//
	// 24 -> 32
	//
//	ASSUME(m_pData);

	if(m_Desc.m_Format == EFormat_RGB24 && _NewFormat == EFormat_RGBA32)
	{
		uint8_t *pBitmap = new uint8_t [m_Desc.m_Pitch*m_Desc.m_Size.y];
		MemoryCopy(pBitmap, m_pData, m_Desc.m_Pitch*m_Desc.m_Size.y);

		Create(m_Desc.m_Size, _NewFormat);
		uint8_t *pPtr = m_pData;
		uint8_t *pSrc = pBitmap;
		
		for(int32_t i = 0; i < m_Desc.m_Size.y*m_Desc.m_Size.x; i++)
		{
			*pPtr++ = *pSrc++;
			*pPtr++ = *pSrc++;
			*pPtr++ = *pSrc++;
			*pPtr++ = 0;
		}

		SafeArrayDelete(pBitmap);
	}
	if(m_Desc.m_Format == EFormat_RGBA32 && _NewFormat == EFormat_RGB24)
	{
		uint8_t *pBitmap = new uint8_t [m_Desc.m_Pitch*m_Desc.m_Size.y];
		MemoryCopy(pBitmap, m_pData, m_Desc.m_Pitch*m_Desc.m_Size.y);

		Create(m_Desc.m_Size, _NewFormat);
		uint8_t *pPtr = m_pData;
		uint8_t *pSrc = pBitmap;
		
		for(int32_t i = 0; i < m_Desc.m_Size.y*m_Desc.m_Size.x; i++)
		{
			*pPtr++ = *pSrc++;
			*pPtr++ = *pSrc++;
			*pPtr++ = *pSrc++;
			*pSrc++;
		}

		SafeArrayDelete(pBitmap);
	}
}


//
// Divs alls color values by 2
//
// 32 is the only that is supported
//
void CImage::Darken(const TVector2<int32_t> &_Position, const TVector2<int32_t> &_Size)
{
	if(m_Desc.m_Format == EFormat_RGBA32)
	{
		uint8_t *pDataPtr = m_pData;
		int32_t Skip = m_Desc.m_Pitch - ((m_Desc.m_Size.x - _Size.x)*(m_Desc.m_Format/8));
		
		pDataPtr += m_Desc.m_Pitch*_Position.y;
		pDataPtr += _Size.x * (m_Desc.m_Format/8);

		for(int32_t y = 0; y < _Size.y; y++)
		{
			for(int32_t x = 0; x < _Size.x; x++)
			{
				*pDataPtr++ = (*pDataPtr)>>1;
				*pDataPtr++ = (*pDataPtr)>>1;
				*pDataPtr++ = (*pDataPtr)>>1;
				*pDataPtr++;
			}
			pDataPtr += Skip;
		}
	}
}
*/

//
// Supports 8, 24 and 32
//
void CImage::Clear(uint32_t _Color)
{
	if (Format().BPP() == 8)
	{
		MemorySet(m_pData, _Color, Format().Height() * Format().Pitch());
	}
	else if (Format().BPP() == 24)
	{
		for (int32_t x = 0; x < Format().Width(); x++)
			for (int32_t y = 0; y < Format().Height(); y++)
				SetColor(x,y,_Color);
	}
	else if (Format().BPP() == 32)
	{
		uint32_t *pDat = (uint32_t *)m_pData;

		for (int32_t c = Format().Height() * Format().Width(); c > 0; c--)
			*pDat++ = _Color;
	}
	else
		throw ("NFoo::CImage::Clear() : Unimplemented Color Format");
}


//
// Doesn't support Blitting between diffrent formats
// Should be optimized
//
void CImage::Blit(CImage *_pSrc, int32_t _x, int32_t _y)
{
	if(_pSrc->Format().BPP() != Format().BPP())
		throw "Can't perform blit between diffrent formats";

	//
	// Get number of rows to copy
	//
	int32_t Rows = _pSrc->Format().Height();

	if(m_Format.Height() < Rows)
		Rows = m_Format.Height();

	if(_y + Rows > m_Format.Height())
		Rows = m_Format.Height() - _y;

	if(Rows < 0)
		return;
	
	//if(m_Format.m_Size.x*m_Format.m_Size.y < Src->m_Format.m_Size.x*Src->m_Format.m_Size.y)
	//{
	int32_t DstPosY = _y;
	int32_t SrcPosY = 0;
	int32_t DstPosX = _x;
	int32_t SrcPosX = 0;
	int32_t Width = _pSrc->m_Format.Width();
	
	if(DstPosY < 0)
	{
		SrcPosY = -DstPosY;
		DstPosY = 0;
	}

	if(DstPosX < 0)
	{
		SrcPosX = -DstPosX;
		DstPosX = 0;
	}

	if(Width+DstPosX > m_Format.Width())
		Width = m_Format.Width() - DstPosX;

	if(Width+SrcPosX > _pSrc->Format().Width())
		Width = _pSrc->Format().Width() - SrcPosX;

	uint8_t *pDstData = m_pData + DstPosY*Format().Pitch() + DstPosX*(Format().BPP()>>3);
	uint8_t *pSrcData = _pSrc->m_pData + SrcPosY*_pSrc->Format().Pitch() + SrcPosX*(_pSrc->Format().BPP()>>3);
	int32_t SrcPitch = (_pSrc->Format().Width() - Width)*(_pSrc->Format().BPP()>>3);
	int32_t DstPitch = (Format().Width() - Width)*(Format().BPP()>>3);
	int32_t NumBytes = Width*(Format().BPP()>>3);
	
	for(; DstPosY < Format().Height() && SrcPosY < _pSrc->Format().Height(); DstPosY++, SrcPosY++)
	{
		MemoryCopy(pDstData, pSrcData, NumBytes);
		pSrcData += (SrcPitch+NumBytes);
		pDstData += (DstPitch+NumBytes);
	}

	//}
	//else
	//{

	//}
}

//
// 24, 32
//
void CImage::SetR(int32_t _x, int32_t _y, uint8_t _R)
{
	if (Format().BPP() == 16)
	{
	}
	else if (Format().BPP() == 24)
	{
		m_pData[3 * (_y * Format().Width() + _x)] = _R;
	}
	else if (Format().BPP() == 32)
	{
		m_pData[4 * (_y * Format().Width() + _x)] = _R;
	}
}

//
// 24, 32
//
void CImage::SetG(int32_t _x, int32_t _y, uint8_t _G)
{
	if (Format().BPP() == 16)
	{
	}
	else if (Format().BPP() == 24)
	{
		m_pData[3 * (_y * Format().Width() + _x) + 1] = _G;
	}
	else if (Format().BPP() == 32)
	{
		m_pData[4 * (_y * Format().Width() + _x) + 1] = _G;
	}
}

//
// 24, 32
//
void CImage::SetB(int32_t _x, int32_t _y, uint8_t _B)
{
	if (Format().BPP() == 16)
	{
	}
	else if (Format().BPP() == 24)
	{
		m_pData[3 * (_y * Format().Width() + _x) + 2] = _B;
	}
	else if (Format().BPP() == 32)
	{
		m_pData[4 * (_y * Format().Width() + _x) + 2] = _B;
	}
}

//
// 32
//
void CImage::SetA(int32_t _x, int32_t _y, uint8_t _A)
{
	if (Format().BPP() == 32)
	{
		m_pData[4 * (_y * Format().Width() + _x) + 3] = _A;
	}
}

//
// 8, 16, 24, 32
// Color should be a 32 bit RGBA value
//
void CImage::SetColor(int32_t _x, int32_t _y, uint32_t _Color)
{
	if (Format().BPP() == 8)
	{
		m_pData[_y * Format().Width() + _x] = _Color;
	}
	else if(Format().BPP() == 16)
	{
		SetR(_x,_y,_Color & 0xFF);
		SetG(_x,_y,(_Color >> 8) & 0xFF);
		SetB(_x,_y,(_Color >> 16) & 0xFF);
	}
	else if(Format().BPP() == 24)
	{
		SetR(_x,_y,_Color & 0xFF);
		SetG(_x,_y,(_Color >> 8) & 0xFF);
		SetB(_x,_y,(_Color >> 16) & 0xFF);
	}
	else if(Format().BPP() == 32)
	{	
		((uint32_t *)m_pData)[_y * Format().Width() + _x] = _Color;
	}
}

//
// 24, 32
//
uint8_t CImage::GetR(int32_t _x, int32_t _y)
{
	if (Format().BPP() == 16)
	{
	}
	else if (Format().BPP() == 24)
	{
		return m_pData[3 * (_y * Format().Width() + _x)];
	}
	else if (Format().BPP() == 32)
	{
		return m_pData[4 * (_y * Format().Width() + _x)];
	}

	return 0;
}

//
// 24, 32
//
uint8_t CImage::GetG(int32_t _x, int32_t _y)
{
	if (Format().BPP() == 16)
	{
	}
	else if (Format().BPP() == 24)
	{
		return m_pData[3 * (_y * Format().Width() + _x) + 1];
	}
	else if (Format().BPP() == 32)
	{
		return m_pData[4 * (_y * Format().Width() + _x) + 1];
	}

	return 0;
}

//
// 24, 32
//
uint8_t CImage::GetB(int32_t _x, int32_t _y)
{
	if (Format().BPP() == 16)
	{
	}
	else if (Format().BPP() == 24)
	{
		return m_pData[3 * (_y * Format().Width() + _x) + 2];
	}
	else if (Format().BPP() == 32)
	{
		return m_pData[4 * (_y * Format().Width() + _x) + 2];
	}

	return 0;
}

//
// 32
//
uint8_t CImage::GetA(int32_t _x, int32_t _y)
{
	if (Format().BPP() == 32)
		return m_pData[4 * (_y * Format().Width() + _x) + 3];

	return 0;
}

//
// 8, 16, 24, 32
//
uint32_t CImage::GetColor(int32_t _x, int32_t _y)
{
	if (Format().BPP() == 8)
	{
		return m_pData[_y * Format().Width() + _x];
	}
	else if (Format().BPP() == 15)
	{
		const int Mask = (1<<5)-1;
		uint32_t Color = (m_pData[(_y * Format().Width() + _x)*2]<<8)|m_pData[(_y * Format().Width() + _x)*2+1];
		return	((Color&Mask)<<3)|
				(((Color>>5)&Mask)<<(3+8))|
				(((Color>>10)&Mask)<<(3+8+8));
	}
	else if (Format().BPP() == 16)
	{
		return ((uint16_t *)m_pData)[_y * Format().Width() + _x];
	}
	else if (Format().BPP() == 24)
	{
		return (GetB(_x,_y) << 16) | (GetG(_x,_y) << 8) | GetR(_x,_y);
	}
	else if(Format().BPP() == 32)
	{	
		return ((uint32_t *)m_pData)[_y * Format().Width() + _x];
	}

	// Hmm
	return 0;
}

//
//
//
void CImage::WrapCoord(int32_t &_rX,int32_t &_rY)
{
	while (_rX >= Format().Width())
		_rX -= Format().Width();

	while (_rX < 0)
		_rX += Format().Width();

	while (_rY >= Format().Height())
		_rY -= Format().Height();

	while (_rY < 0)
		_rY += Format().Height();
}

//
// 8, 24, 32
//
void CImage::ApplyKernelFilter(const CImage::CKernelFilter &_rFilter)
{
	CImage Image;
	
	ApplyKernelFilter(_rFilter,Image);

	*this = Image;
}

//
// 8, 24, 32
//
void CImage::ApplyKernelFilter(const CImage::CKernelFilter &_rFilter,CImage &_rDst)
{
	_rDst.Create(Format());

	if (Format().BPP() == 8)
	{
		for (int32_t y = 0; y < Format().Height(); y++)
			for (int32_t x = 0; x < Format().Width(); x++)
			{
				fp32 Sum = 0;

				for (int32_t i = 0; i < 3; i++)
					for (int32_t j = 0; j < 3; j++)
					{
						int32_t nx = x + i - 1;
						int32_t ny = y + j - 1;
						WrapCoord(nx,ny);

						Sum += (fp32)GetColor(nx,ny) * _rFilter[j][i];
					}

				Sum = Clamp(Sum / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);

				_rDst.SetColor(x,y,(uint8_t)Sum);
			}
	}
	else if (Format().BPP() == 24)
	{
		for (int32_t y = 0; y < Format().Height(); y++)
			for (int32_t x = 0; x < Format().Width(); x++)
			{
				fp32 SumR = 0;
				fp32 SumG = 0;
				fp32 SumB = 0;

				for (int32_t i = 0; i < 3; i++)
					for (int32_t j = 0; j < 3; j++)
					{
						int32_t nx = x + i - 1;
						int32_t ny = y + j - 1;
						WrapCoord(nx,ny);

						SumR += (fp32)GetR(nx,ny) * _rFilter[j][i];
						SumG += (fp32)GetG(nx,ny) * _rFilter[j][i];
						SumB += (fp32)GetB(nx,ny) * _rFilter[j][i];
					}

				SumR = Clamp(SumR / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);
				SumG = Clamp(SumG / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);
				SumB = Clamp(SumB / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);

				_rDst.SetR(x,y,(uint8_t)SumR);
				_rDst.SetG(x,y,(uint8_t)SumG);
				_rDst.SetB(x,y,(uint8_t)SumB);
			}
	}
	else if (Format().BPP() == 32)
	{
		for (int32_t y = 0; y < Format().Height(); y++)
			for (int32_t x = 0; x < Format().Width(); x++)
			{
				fp32 SumR = 0;
				fp32 SumG = 0;
				fp32 SumB = 0;
				fp32 SumA = 0;

				for (int32_t i = 0; i < 3; i++)
					for (int32_t j = 0; j < 3; j++)
					{
						int32_t nx = x + i - 1;
						int32_t ny = y + j - 1;
						WrapCoord(nx,ny);

						SumR += (fp32)GetR(nx,ny) * _rFilter[j][i];
						SumG += (fp32)GetG(nx,ny) * _rFilter[j][i];
						SumB += (fp32)GetB(nx,ny) * _rFilter[j][i];
						SumA += (fp32)GetA(nx,ny) * _rFilter[j][i];
					}

				SumR = Clamp(SumR / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);
				SumG = Clamp(SumG / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);
				SumB = Clamp(SumB / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);
				SumA = Clamp(SumA / _rFilter.Scale() + _rFilter.Offset(), 0.0f, 255.0f);

				_rDst.SetR(x,y,(uint8_t)SumR);
				_rDst.SetG(x,y,(uint8_t)SumG);
				_rDst.SetB(x,y,(uint8_t)SumB);
				_rDst.SetA(x,y,(uint8_t)SumA);
			}
	}
}

//
// 8
//
void CImage::BuildNormalMap(fp32 _Slope)
{
	CImage Dest;

	BuildNormalMap(Dest,_Slope);

	*this = Dest;
}

//
// 8
//
void CImage::BuildNormalMap(CImage &_rDst,fp32 _Slope)
{
	if (Format().BPP() != 8)
		return;

	CImage::CKernelFilter dx,dy;

	dx[0][0] = -1; dx[0][2] = 1;
	dx[1][0] = -2; dx[1][2] = 2;
	dx[2][0] = -1; dx[2][2] = 1;

	dy[0][0] = -1; dy[2][0] = 1;
	dy[0][1] = -2; dy[2][1] = 2;
	dy[0][2] = -1; dy[2][2] = 1;

	//
	_rDst.Create(CFormat(EQuick_RGBi8, Format().Width(), Format().Height()));

	for (int32_t y = 0; y < Format().Height(); y++)
		for (int32_t x = 0; x < Format().Width(); x++)
		{
			TVector3<fp32> V(0,0,0);

			for (int32_t i = 0; i < 3; i++)
				for (int32_t j = 0; j < 3; j++)
				{
					int32_t nx = x + i - 1;
					int32_t ny = y + j - 1;
					WrapCoord(nx,ny);

					V.x += (fp32)GetColor(nx,ny) * dx[j][i];
					V.y += (fp32)GetColor(nx,ny) * dy[j][i];
				}

			V.z = _Slope;
			V.Normalize();

			V += 1.0f;
			V *= 0.5f;

			_rDst.SetR(x,y,(uint8_t)(V.x * 255.0f));
			_rDst.SetG(x,y,(uint8_t)(V.y * 255.0f));
			_rDst.SetB(x,y,(uint8_t)(V.z * 255.0f));
		}
}