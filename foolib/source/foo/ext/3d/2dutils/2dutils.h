#ifndef __2DUTILS_H__
#define __2DUTILS_H__

#include <foo/ext/3d/3d.h>

namespace NFoo { namespace N3D {
	
	/*
	class C2DClip
	{
	public:
		enum
		{
			E2DCLIP_MASK_1 = 1 << 0,
			E2DCLIP_MASK_2 = 1 << 1,
			E2DCLIP_MASK_3 = 1 << 2,
			E2DCLIP_MASK_4 = 1 << 3,
			E2DCLIP_MASK_ALL = (E2DCLIP_MASK_1 | E2DCLIP_MASK_2 | E2DCLIP_MASK_3 | E2DCLIP_MASK_4)
		};
	public:

		C2DClip(bool _bClip = false, bool _bClipAny = true,
			const TVector2<fp32>& _ClipTL = TVector2<fp32>(0.0f, 0.0f),
			const TVector2<fp32>& _ClipBR = TVector2<fp32>(0.0f, 0.0f));


		bool Clip(const TVector2<fp32>& _P1, const TVector2<fp32>& _P2, 
			const TVector2<fp32>& _P3, const TVector2<fp32>& _P4);
		bool Clip(const TVector2<fp32>& _P1, const TVector2<fp32>& _P2, 
			const TVector2<fp32>& _P3);
		bool Clip(const TVector2<fp32>& _P1, const TVector2<fp32>& _P2);
		bool Clip(const TVector2<fp32>& _P1);

		TVector2<fp32> m_ClipTL;
		TVector2<fp32> m_ClipBR;
		bool m_bClip;
		bool m_bClipAny;
	};*/

	
	class C2DUtils
	{
	public:
		class CFont
		{
		public:
			uint8 m_aTranslationTable[256];
			int32_t m_iTexture;
			int32_t m_XCells;
			int32_t m_YCells;
			fp32 m_Spacing;
			fp32 m_Height;
			fp32 m_Width;

			void BuildNormalAscii();
			void BuildTranslationTable(const uint8 *_pString);
		};

		class CClipRect
		{
		public:
			TVector2<fp32> m_Position;
			TVector2<fp32> m_Size;
		};

	private:
		//TDynamicArray<C2DClip> m_lClipStack;
		NFoo::N3D::CDevice *m_pDevice;
		int32_t m_iVertexBuffer;
		NFoo::N3D::CVertexBuffer *m_pVertexBuffer;
		CFont *m_pFont;
		int32_t m_VertexCount;
		//int32_t m_AreaX;
		//int32_t m_AreaY;
		
		bool m_aWriteMask[4];

		fp32 m_aColors[4][4];
		fp32 m_aTexCoords[4][2];
		fp32 m_QuadRotation; // in rad
		int32_t m_iTexture;
		
		int32_t m_Mode;
		int32_t m_Blend;

		enum
		{
			EMode_NotInited=0,
			EMode_Idle,
			EMode_Quads,
			EMode_Lines,
			EMode_Text,
			EMode_Triangles
		};
		
		bool LockVB(int32_t _iMode);
		void UnlockVB();
		void SetState();

		CClipRect m_ClipRect;
		bool m_UseClip;
		TVector2<fp32> m_GlobalTranslate;
		
	public:
		C2DUtils();
		~C2DUtils();
	
		enum 
		{
			EBlend_None=0,
			EBlend_Alpha,
			EBlend_Add,
			EBlend_DstAlpha,


			ETextDraw_TL = 1 << 0
		};

		int32_t m_ScreenWidth, m_ScreenHeight;
		fp32 m_NudgeX;
		fp32 m_NudgeY;

		// Defaults:
		// Color = 1,1,1,1
		// UV = 0,0 1,1
		// Rot = 0
		// Tex = -1
		
		// global
		bool Init(NFoo::N3D::CDevice *_pDevice, int32_t _BufferSize=1024*8);
		void Shutdown();
		void SetDefaults();

		void SetScreenSize(int32_t _AreaX, int32_t _AreaY);

		/*
		void PushClip(C2DClip _Clip);
		void PopClip();
		C2DClip GetClip() const;
		*/
		void SetClipping(bool _Use, fp32 _x=0, fp32 _y=0, fp32 _w=0, fp32 _h=0);
		void SetGlobalTranslate(fp32 x, fp32 y);
		
		// color
		void SetColor(int32_t _Index, fp32 _r, fp32 _g, fp32 _b, fp32 _a);
		void SetColor(fp32 _r, fp32 _g, fp32 _b, fp32 _a);
		void setColor(TVector4<fp32> color); //[0] = r, [1] = g, [2] = b, [3] = a
		void SetBlend(int32_t _iBlend);
		void SetWriteMask(bool r, bool g, bool b, bool a);

		// texture
		void SetTexture(int32_t _iTex);
		void SetTexCoord(int32_t _Index, fp32 _u, fp32 _v);
		void SetTexCoord(fp32 _u0, fp32 _v0, fp32 _u1, fp32 _v1);
		
		// Lines
		//
		// 0-------1
		//
		bool Line_Begin();
		void Line_Draw(fp32 _x0, fp32 _y0, fp32 _x1, fp32 _y1);
		void Line_End();

		// Triangles
		//
		//
		//
		bool Triangle_Start();
		void Triangle_DrawTriangle(fp32 _X0, fp32 _Y0, fp32 _X1, fp32 _Y1, fp32 _X2, fp32 _Y2);
		void Triangle_End();
		
		// Quad
		//
		// 0--1
		// | /|
		// |/ |
		// 2--3
		//
		bool Quad_Begin(); // locks VB, (sets default values?)
		void Quad_SetRotation(fp32 _r); // in rad
		void Quad_Draw(fp32 _x, fp32 _y, fp32 _w, fp32 _h);
		void Quad_DrawTL(fp32 _x, fp32 _y, fp32 _w, fp32 _h);
		void Quad_End(); // unlocks VB and throws it to the device

		//
		// Text
		//
		void Text_Begin(CFont *_pFont);
		void Text_Draw(fp32 _x, fp32 _y, int32_t _Flags, const int8 *_pFormat, ...);
		void Text_End();
	};
} }

#endif // __2DUTILS_H__


