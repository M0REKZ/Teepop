#ifndef __FOO_IMAGE_H__
#define __FOO_IMAGE_H__

// includes
#include <foo/base/base.h>
#include <foo/base/memory.h>
#include "vector2.h"
#include "classregistry.h"
#include "stream.h"

namespace NFoo
{
	//
	// Surface Class
	//
	class CImage
	{
		class CPixel
		{
		public:
			fp32 c[4]; // Components, 0-1 = 0-255, RGBA, YUVA
		};

		CPixel GetPixelRGBA(int32_t x, int32_t y); // these are slow as hell and should only beused as a last resort.
		void SetPixelRGBA(int32_t x, int32_t y, CPixel Pixel);
	public:
		enum
		{
			EQuick_None=0,
			EQuick_RGBi8,
			EQuick_RGBAi8,
			EQuick_Yi8,
			EQuick_YAi8,
			EQuick_YUVi8,

			EType_None=0,
			EType_Indexed,
			EType_Channels,

			EChannelType_Int=0,
			EChannelType_Float,

			
			EChannel_R=0,
			EChannel_G=1,
			EChannel_B=2,
			EChannel_A=3,
			EChannel_X=0,

			EChannel_Y=0,
			EChannel_U=1,
			EChannel_V=2,

			EColorSpace_GrayScale=1,
			EColorSpace_RGB,
			EColorSpace_YUV
		};

		class CFormat
		{	
			friend class CImage;
			int32_t m_Type; // None, Indexed, Channels,
			union
			{
				// Indexed
				struct
				{
					uint16 m_Bits;
					void *m_pPallete;
				} m_IndexInfo;

				// Channels
				struct
				{
					uint8 m_NumChannels;
					uint8 m_ColorSpace; // GrayScale, YUV, RGB
					uint8 m_ChannelType; // Int, Float
					uint8 m_ChannelEndian; // BE, LE
					uint8 m_aChannelDepth[4]; // in bits
					uint8 m_aChannels[4]; // R, G, B, A, Y, U, V, X
				} m_ChannelInfo;
			};

			uint16 m_Width; // in pixels
			uint16 m_Height; // in pixels

			// redundant variables, speed optimizations
			uint32_t m_DataSize;
			uint32_t m_Pitch;
			uint16 m_BPP;

			// updates the redunant variables
			void Refresh();
		public:
			CFormat(int32_t i = EQuick_None, int32_t w=-1, int32_t h=-1);
			void QuickSet(int32_t i, int32_t w, int32_t h);

			void SetType(int32_t Type);
			void SetSize(int32_t w, int32_t h);
			void SetChannels(int32_t colorspace, int32_t c0=0xff, int32_t c1=0xff, int32_t c2=0xff, int32_t c3=0xff);
			void SetChannelDepths(int32_t c0=0, int32_t c1=0, int32_t c2=0, int32_t c3=0);

			int32_t Type() const { return m_Type; }

			int32_t NumChannels() const { return m_ChannelInfo.m_NumChannels; }
			int32_t ColorSpace() const { return m_ChannelInfo.m_ColorSpace; }
			int32_t ChannelType() const { return m_ChannelInfo.m_ChannelType; }
			int32_t ChannelDepth(intr i) const { return m_ChannelInfo.m_aChannelDepth[i]; }
			uint32_t ChannelDepths() const { return ( m_ChannelInfo.m_aChannelDepth[0]<<24)|
													(m_ChannelInfo.m_aChannelDepth[1]<<16)|
													(m_ChannelInfo.m_aChannelDepth[2]<<8)|
													m_ChannelInfo.m_aChannelDepth[3]; }
			//uint32_t ChannelDepths() const { return *((uint32_t*)m_aChannelDepth); }
			int32_t Channel(intr i) const { return m_ChannelInfo.m_aChannels[i]; }
			uint32_t Channels() const { return (  m_ChannelInfo.m_aChannels[0]<<24)|
												(m_ChannelInfo.m_aChannels[1]<<16)|
												(m_ChannelInfo.m_aChannels[2]<<8)|
												m_ChannelInfo.m_aChannels[3]; }

			int32_t Height() const { return m_Height; }
			int32_t Width() const { return m_Width; }
			int32_t Pitch() const { return m_Pitch; }
			int32_t BPP() const { return m_BPP; }
			int32_t DataSize() const { return m_DataSize; }
		};

		/*
		enum
		{
			EFormat_None,
			EFormat_Indexed,
			EFormat_Channels,



			EFormat_Device=-1,
			EFormat_NotSpecified=0,
			EFormat_Indexed4=4,
			EFormat_BW=8,
			EFormat_RGB15=15,
			EFormat_RGB16=16,
			EFormat_RGB24=24,
			EFormat_RGBA32=32,

			EChannel_Red=0,
			EChannel_Green=1,
			EChannel_Blue=2,
			EChannel_Alpha=3,

			EChannel_R=0,
			EChannel_G=1,
			EChannel_B=2,
			EChannel_A=3,

			EChannel_Y=0,
			EChannel_U=1,
			EChannel_V=2
		};
		*/

		//
		// 3x3 Kernel Filter
		//
		class CKernelFilter
		{
		public:
			CKernelFilter();
			CKernelFilter(fp32);

			fp32* operator [] (int32_t _y) { return m_aFilter[_y]; }
			const fp32* operator [] (int32_t _y) const { return m_aFilter[_y]; }

			void SetScale(fp32 _Scale);
			void SetOffset(fp32 _Offset);

			fp32 Scale()	const { return m_Scale;	 }
			fp32 Offset()	const { return m_Offset; }

		private:
			fp32 m_aFilter[3][3];	// YxX
			fp32 m_Scale;			// Sum / Scale
			fp32 m_Offset;
		};

		/*
		//
		// Format
		//
		class CImageDesc
		{
		public:
			// Data
			int32_t m_Format;
			int32_t m_Pitch;
		
			TVector2<int32_t> m_Size;
		
			// Operators
			intr operator == (const CImageDesc &t) { return (MemoryCompare(this, &t, sizeof(CImageDesc)) == 0); }
			intr operator != (const CImageDesc &t) { return (MemoryCompare(this, &t, sizeof(CImageDesc)) != 0); }
		};*/


	private:
		CFormat m_Format;
		uint8 *m_pData;
	public:
	
		//
		CImage();
		CImage(CFormat Format, uint8 *pData);
		CImage(const CImage &);
		virtual ~CImage();

		void operator = (const CImage &);
		uint8 &operator [] (const intr i) { return m_pData[i]; }
	
		const CFormat &Format() const { return m_Format; }
		uint8 *Data() { return m_pData; }
		const uint8 *Data() const { return m_pData; }

		// Basic Operation
		void RemoveData(); // doesn't deallocate data
		void ClearData(); // deallocates and everything
		bool Create(CFormat Format);
		bool Create(const CImage &rImage);
		bool Load(CInputStream &rStream);
		bool Write(COutputStream &rStream);

		// Convertion (ignores width, height in format)
		void Convert(CFormat Format, CImage &rImage, int32_t Flags=0); // colorspace convertion, etc
		void Convert(CFormat Format);

		// operations
		void FlipX();
		void FlipY();
		void Blit(CImage *Src, int32_t _x, int32_t _y);
		void Clear(uint32_t = 0);

		// Not complete! Proposal..
		void SetR(int32_t _x, int32_t _y, uint8 _r);
		void SetG(int32_t _x, int32_t _y, uint8 _g);
		void SetB(int32_t _x, int32_t _y, uint8 _b);
		void SetA(int32_t _x, int32_t _y, uint8 _a);
		void SetColor(int32_t _x, int32_t _y, uint32_t _Color);

		uint8 GetR(int32_t _x, int32_t _y);
		uint8 GetG(int32_t _x, int32_t _y);
		uint8 GetB(int32_t _x, int32_t _y);
		uint8 GetA(int32_t _x, int32_t _y);
		uint32_t GetColor(int32_t _x, int32_t _y);

		//
		void ApplyKernelFilter(const CKernelFilter &_rFilter);
		void ApplyKernelFilter(const CKernelFilter &_rFilter,CImage &_rDst);
		void BuildNormalMap(fp32 _Slope = 255.0f);
		void BuildNormalMap(CImage &_rDst,fp32 _Slope = 255.0f);

		//
		//void ExtractChannel(int32_t iChannel, CImage &rChannel);
		//void AddChannel(int32_t _iChannel, CImage &_rChannel);

		//
		//void Sizeup(int32_t _Steps, CImage &_rDst);

		// Internal operation
		/*
		virtual void Clear(uint32_t Color=0);
		virtual void FlipRB();
		virtual void FlipX();
		virtual void FlipY();
		virtual void RotCCW90();
		virtual void SetFormat(int32_t _Format);
	
		virtual void Darken(const TVector2<int32_t> &_Position, const TVector2<int32_t> &_Size);

		//
		void ExtractChannel(int32_t _iChannel, CImage &_rChannel);
		void AddChannel(int32_t _iChannel, CImage &_rChannel);
		void ConvertRGBtoYUV();
		void ConvertYUVtoRGB();

		//
		void Sizedown(int32_t _Steps, CImage &_rDst);
		void Sizeup(int32_t _Steps, CImage &_rDst);

		int32_t SizeX() const { return m_Desc.m_Size.x; }
		int32_t SizeY() const { return m_Desc.m_Size.y; }
		*/

	private:
		void WrapCoord(int32_t &_rX,int32_t &_rY);
	};

	//
	//
	//
	class CImageFormat : public NClassRegistry::CClass
	{
	public:
		virtual bool CanHandle(const class CMime &rMime) { return false; };
		virtual bool Load(CInputStream &rStream, CImage &rImage) { return false; };
		virtual bool Write(COutputStream &rStream, CImage &rImage) { return false; };
	};

} // namespaces
#endif // __FOO_IMAGE_H__


