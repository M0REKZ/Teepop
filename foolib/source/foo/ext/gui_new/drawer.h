#ifndef __FOO_DRAWER_H__
#define __FOO_DRAWER_H__

#include <foo/ext/3d/2dutils/2dutils.h>

namespace NFoo {
namespace NGUI {
	
	class CPainter
	{
	public:
		virtual void Init() pure;
		virtual void Begin() {};
		virtual void End() {};


		virtual void SetGlobalTranslate(int32_t _x, int32_t _y) pure;
		virtual void SetClipping(bool _Use, int32_t _x=-1, int32_t _y=-1, int32_t _w=-1, int32_t _h=-1) pure;
		virtual int32_t CreateImage(CImage &_Image) pure;
		virtual void SetImage(int32_t _iImage) pure;
		virtual void SetUV(int32_t _u, int32_t _v, int32_t _w, int32_t _h) pure;
		virtual void DrawRect(int32_t _x, int32_t _y, int32_t _w=-1, int32_t _h=-1) pure;
		virtual void DrawText(int32_t _x, int32_t _y, const char *_pText, fp32 _r=1, fp32 _g=1, fp32 _b=1) pure;
	};

	//
	//
	//
	class CDrawer
	{
	public:
		//
		//
		//
		class CPart
		{
		public:
			int32_t x, y;
			int32_t w, h;
			void Set(int32_t _x, int32_t _y, int32_t _w, int32_t _h);
		};
		
		//
		//
		//
		class CRectParts
		{
		public:
			enum
			{
				ECornerTL=0,
				ECornerTR,
				ECornerBL,
				ECornerBR,
		
				EBorderT,
				EBorderB,
				EBorderL,
				EBorderR,

				EFill,

				ENum
			};

			CPart m_aParts[ENum];

			void Init(int32_t _x, int32_t _y);

		};

		enum
		{
			EOrigo_TL=0,
			EOrigo_TR,
			EOrigo_BL,
			EOrigo_BR
		};

		//

		CRectParts m_aRectTypes[2];

		CPart m_ScrollbarButtons;
		CPart m_ScrollbarHandle;

		CPart m_aCheckBoxStates[2];
		CPart m_aSeparatorParts[3];

		CPart m_WindowCloseButton;

		CPart m_Cursor;
		CPart m_TextCursor;

		CPainter *m_pPainter; 

		void Init();

		void DrawPart(CPart &_rPart, int32_t _Origo, int32_t _x, int32_t _y, int32_t _w=-1, int32_t _h=-1);
		void DrawRect(CRectParts &_rRect, int32_t _x, int32_t _y, int32_t _w, int32_t _h);

		//
		//
		//
		void SetGlobalTranslate(int32_t _x, int32_t _y);
		void SetClipping(bool _Use, int32_t _x=-1, int32_t _y=-1, int32_t _w=-1, int32_t _h=-1);

		
		void SetUV(int32_t _u, int32_t _v, int32_t _w, int32_t _h);
		void Draw(int32_t _x, int32_t _y, int32_t _w, int32_t _h);

		void DrawText(int32_t _x, int32_t _y, const char *_pText, fp32 _r=1, fp32 _g=1, fp32 _b=1);
		//void DrawTextWhite(int32_t _x, int32_t _y, const int8_t *_pText);

		//void Begin(int32_t _Blend = NFoo::N3D::C2DUtils::EBlend_Alpha, fp32 r=1.0f, fp32 g=1.0f, fp32 b=1.0f, fp32 a=1.0f);
		//void End();

	};

	//
	//
	//
	class CPainter_2DUtils : public CPainter
	{
	public:
		//N3D::C2DUtils m_UtilsRect;
		//N3D::C2DUtils m_UtilsText;
		N3D::C2DUtils *m_pUtils;
		fp32 m_TextureWidth;
		fp32 m_TextureHeight;
		int32_t m_iTexture;

		NFoo::N3D::C2DUtils::CFont m_Font;

		virtual void Begin();
		virtual void End();
		virtual void Init();
		virtual int32_t CreateImage(CImage &_Image);
		virtual void SetImage(int32_t _iImage);
		virtual void SetGlobalTranslate(int32_t _x, int32_t _y);
		virtual void SetClipping(bool _Use, int32_t _x=-1, int32_t _y=-1, int32_t _w=-1, int32_t _h=-1);
		virtual void SetUV(int32_t _u, int32_t _v, int32_t _w, int32_t _h);
		virtual void DrawRect(int32_t _x, int32_t _y, int32_t _w, int32_t _h);
		virtual void DrawText(int32_t _x, int32_t _y, const char *_pText, fp32 _r=1, fp32 _g=1, fp32 _b=1);
	};

}
}

#endif // __FOO_DRAWER_H__
