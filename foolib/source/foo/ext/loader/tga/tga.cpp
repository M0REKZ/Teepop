#include <foo/ext/compr/rle.h>
#include <foo/func/memorystream.h>
#include <foo/func/image.h>

using namespace NFoo;

//
// Targa image format
//
class CTGAFormat : public CImageFormat
{
public:
	static CMime ms_Mime;

	CTGAFormat()
	{
		CMime::AddFileExt("tga","image/targa");
	}

	bool CanHandle(const CMime &rMime)
	{
		if(rMime == ms_Mime)
			return true;

		return false;
	}

	bool Load(CInputStream &rStream, CImage &rImage);
	bool Write(COutputStream &rStream, CImage &rImage);
};

CMime CTGAFormat::ms_Mime("image/targa");
MACRO_REGISTER_CLASS(CTGAFormat, "imageformat")

//
// Image types
//
enum
{
	EImageType_NoData=0,
	EImageType_ColorMapped=1,
	EImageType_TrueColor=2,
	EImageType_BlackAndWhite=3,
	EImageType_RLE_ColorMapped=9,
	EImageType_RLE_TrueColor=10,
	EImageType_RLE_BlackAndWhite=11
};

class CHeader
{
public:
	void Read(CInputStream &_rStream);
	void Write(COutputStream &_rStream);

	uint8_t m_IDLen;
	uint8_t m_ColorMapType;
	uint8_t m_ImageType;

	uint16_t m_ColorMap_FirstEntryIndex;
	uint16_t m_ColorMap_Length;
	uint8_t m_ColorMap_EntrySize;

	uint16_t m_PositionX;
	uint16_t m_PositionY;
	uint16_t m_Width;
	uint16_t m_Height;
	uint8_t m_PixelDepth;

	uint8_t m_ImageDescriptor;
};

//
//
//
void CHeader::Read(CInputStream &_rStream)
{
	_rStream.ReadLE(m_IDLen);
	_rStream.ReadLE(m_ColorMapType);
	_rStream.ReadLE(m_ImageType);
	
	_rStream.ReadLE(m_ColorMap_FirstEntryIndex);
	_rStream.ReadLE(m_ColorMap_Length);
	_rStream.ReadLE(m_ColorMap_EntrySize);
	
	_rStream.ReadLE(m_PositionX);
	_rStream.ReadLE(m_PositionY);
	_rStream.ReadLE(m_Width);
	_rStream.ReadLE(m_Height);
	_rStream.ReadLE(m_PixelDepth);

	_rStream.ReadLE(m_ImageDescriptor);
}


//
//
//
void CHeader::Write(COutputStream &_rStream)
{
	_rStream.WriteLE(m_IDLen);
	_rStream.WriteLE(m_ColorMapType);
	_rStream.WriteLE(m_ImageType);
	
	_rStream.WriteLE(m_ColorMap_FirstEntryIndex);
	_rStream.WriteLE(m_ColorMap_Length);
	_rStream.WriteLE(m_ColorMap_EntrySize);
	
	_rStream.WriteLE(m_PositionX);
	_rStream.WriteLE(m_PositionY);
	_rStream.WriteLE(m_Width);
	_rStream.WriteLE(m_Height);
	_rStream.WriteLE(m_PixelDepth);

	_rStream.WriteLE(m_ImageDescriptor);
}


//
// 24 and 32 bit support
// all origentaions are supported
// rle support
// Doesn't convert the format to specified format
//
bool CTGAFormat::Load(CInputStream &rStream, CImage &rImage)
{
	if(!rStream.IsReady())
		return NULL;
	// Store position
	//_Storage.Push();

	// Read Header
	CHeader Header;
	uint8_t ImageID[256];

	Header.Read(rStream);
	
	rStream.Read(ImageID, Header.m_IDLen);
	rStream.Read(NULL, Header.m_ColorMap_Length);

	// Create Image
	int32_t Count = Header.m_Width*Header.m_Height;
	int32_t DataSize = Count*(Header.m_PixelDepth/8);

	CImage::CFormat Format;
	Format.SetType(CImage::EType_Channels);
	Format.SetSize(Header.m_Width, Header.m_Height);
	Format.SetChannelDepths(8,8,8,8);

	if(Header.m_ImageType == EImageType_TrueColor || Header.m_ImageType == EImageType_RLE_TrueColor)
	{
		if(Header.m_PixelDepth == 24)
			Format.SetChannels(CImage::EColorSpace_RGB, CImage::EChannel_B,CImage::EChannel_G,CImage::EChannel_R);
		else if(Header.m_PixelDepth == 32)
			Format.SetChannels(CImage::EColorSpace_RGB, CImage::EChannel_B, CImage::EChannel_G, CImage::EChannel_R, CImage::EChannel_A);
		else
			throw "tgaloader: bad format";
	}
	else if (Header.m_ImageType == EImageType_BlackAndWhite || Header.m_ImageType == EImageType_RLE_BlackAndWhite)
	{
		if(Header.m_PixelDepth == 8)
			Format.SetChannels(CImage::EColorSpace_GrayScale, CImage::EChannel_Y);
		else
			throw "tgaloader: bad format";
	}

	rImage.Create(Format);

	/*
	_rImage.m_pData = new uint8_t [DataSize+4]; // we must waste 4 bytes inorder to load fat
	_rImage.m_Desc.m_Size.x = Header.m_Width;
	_rImage.m_Desc.m_Size.y = Header.m_Height;
	_rImage.m_Desc.m_Format = Header.m_PixelDepth;//m_Size.y = Header.m_Height;
	_rImage.m_Desc.m_Pitch = Header.m_Width*(Header.m_PixelDepth/8);
	*/

	//if(Header.m_PixelDepth == 16)
	//	_rImage.m_Desc.m_Format = 15;
	//_rImage.Create(TVector2<int32_t>(Header.m_Width, Header.m_Height), Header.m_PixelDepth);

	uint8_t *pPtr = rImage.Data();
	
	do 
	{
		if(Header.m_ImageType == EImageType_TrueColor ||
			Header.m_ImageType == EImageType_BlackAndWhite)
		{
			rStream.Read(pPtr, DataSize);
			//if(Header.m_ImageType == EImageType_TrueColor)
			//	_rImage.FlipRB();
		}
		else if(Header.m_ImageType == EImageType_RLE_TrueColor ||
				Header.m_ImageType == EImageType_RLE_BlackAndWhite)
		{
			int32_t RLEDataSize = rStream.GetSize()-rStream.GetPosition();
			unsigned char *pImageData = new unsigned char[RLEDataSize];
			rStream.Read(pImageData, RLEDataSize);			// RLE Compressed True Color Format
			CMemoryReader MemoryReader(pImageData);
			CRLEDecoder Decoder(&MemoryReader, Header.m_PixelDepth/8);
			Decoder.Read(pPtr, DataSize);
			delete [] pImageData;

			//if(Header.m_ImageType == EImageType_RLE_TrueColor)
			//	_rImage.FlipRB();
		}
		else
			break; // failure
	
		// Flip Image if needed
		int32_t Origin = (Header.m_ImageDescriptor>>4)&3;
	
		if(1-(Origin>>1))
			rImage.FlipY();
	
		//if(Origin&1)
		//	rImage.FlipX();
	
		//_Storage.Pop();
		
		return true;
	
	} while(0);

	rImage.ClearData();
	//_Storage.Pop();

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 8, 24 and 32 bit support, no rle support
// Moves the storage pointer.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CTGAFormat::Write(COutputStream &rStream, CImage &rImage)
{
	bool RLECompress = false;

	// Fix Header
	CHeader Header;
	Header.m_IDLen = 0;

	Header.m_ColorMapType = 0;
	Header.m_ColorMap_Length = 0;
	Header.m_ColorMap_FirstEntryIndex = 0;
	Header.m_ColorMap_EntrySize = 0;
	
	Header.m_PositionY = 0;
	Header.m_PositionX = 0;

	Header.m_Height = (uint16_t)rImage.Format().Height();
	Header.m_Width = (uint16_t)rImage.Format().Width();
	Header.m_PixelDepth = (uint8_t)rImage.Format().BPP();
	
	if (Header.m_PixelDepth == 8)
	{
		if (RLECompress)
			Header.m_ImageType = EImageType_RLE_BlackAndWhite;
		else
			Header.m_ImageType = EImageType_BlackAndWhite;
	}
	else if (Header.m_PixelDepth == 24 || Header.m_PixelDepth == 32)
	{
		if (RLECompress)
			Header.m_ImageType = EImageType_RLE_TrueColor;
		else
			Header.m_ImageType = EImageType_TrueColor;
	}
	else
		return false;

	Header.m_ImageDescriptor = 1 << 5;

	if(Header.m_PixelDepth == 32)
		Header.m_ImageDescriptor += 8;

	// Write Header
	Header.Write(rStream);

	CImage Image; // TGAs are BGR
	Image.Create(rImage);
	//Image.FlipRB();

	if(RLECompress)
	{
		CRLEEncoder Compressor(&rStream, Header.m_PixelDepth / 8);
		Compressor.Write(Image.Data(), Image.Format().DataSize());
	}
	else
		rStream.Write(Image.Data(), Image.Format().DataSize());

	return true;
}


