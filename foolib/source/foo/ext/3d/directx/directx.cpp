
#include <base/base.h>
#include <base/log.h>
#include <base/string.h>
#include <base/memory.h>
#include "directx.h"

#include <d3dx9.h>

//
using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NDirectX;

const D3DPRIMITIVETYPE g_aDrawTable[] = 
{
	D3DPT_POINTLIST,
	D3DPT_LINELIST,
	D3DPT_LINESTRIP,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN,
	(D3DPRIMITIVETYPE)0 /*Quads not supported*/
};


const int32_t g_aBaseTable[] = 
{
	1,//D3DPT_POINTLIST,
	2,//D3DPT_LINELIST,
	2,//D3DPT_LINESTRIP,
	3,//D3DPT_TRIANGLELIST,
	3,//D3DPT_TRIANGLESTRIP,
	3,//D3DPT_TRIANGLEFAN,
	0,//(D3DPRIMITIVETYPE)0 /*Quads not supported*/
};

const int32_t g_aExtraTable[] = 
{
	1,//D3DPT_POINTLIST,
	2,//D3DPT_LINELIST,
	1,//D3DPT_LINESTRIP,
	3,//D3DPT_TRIANGLELIST,
	1,//D3DPT_TRIANGLESTRIP,
	1,//D3DPT_TRIANGLEFAN,
	0,//(D3DPRIMITIVETYPE)0 /*Quads not supported*/
};


CDirectXDevice::CDirectXDevice()
{
	// Must find window handle
	m_hWindow = 0;
	m_p3DDevice = NULL;
	m_pVertexDeclaration = NULL;
	ZeroCaps(m_DeviceCaps);

	// Zero the streamflag and set streamflag
	m_StreamFlags = 0;
	m_StreamDeclSet = 0;
	
	// Create d3d(?)
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
}

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CDirectXDevice::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return MsgProc( hWnd, uMsg, wParam, lParam );
}

LRESULT CDirectXDevice::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

bool CDirectXDevice::Init(CMode _Mode)
{
	// Create window...
	if (m_hWindow == NULL)
	{
		// Register the windows class
		WNDCLASS WndClass = { 0, CDirectXDevice::WndProc, 0, 0, 0/*hInstance*/,
			NULL, NULL, NULL, NULL, ("Foo D3D") };
		RegisterClass(&WndClass );

		// Set the window's initial style
		uint32_t WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | 
			WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;

		// Set the window's initial width
		RECT Rc;
		SetRect(&Rc, 0, 0, _Mode.Get(CMode::EWidth), _Mode.Get(CMode::EHeight));        

		// Create the render window
		m_hWindow = CreateWindow(("Foo D3D"), ("Foo D3D"), WindowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT,(Rc.right - Rc.left), (Rc.bottom - Rc.top), 0,
			0, 0, 0);
	}
	// Create device from given display mode
	D3DPRESENT_PARAMETERS Params;
	Params.BackBufferWidth = _Mode.Get(CMode::EWidth);
	Params.BackBufferHeight = _Mode.Get(CMode::EHeight);
	Params.BackBufferFormat = VerifyColorFormat(_Mode.m_aParams[CMode::EAlphaBits].m_DeviceParams[EMODE_DEVICEPARAMINDEX_COLOR]);//,_Mode.Get(CMode::EAlphaBits)); //D3DFORMAT
	Params.BackBufferCount = 1; //? 1 + frontbuffer or implied frontbuffer + x backbufffer?
	Params.MultiSampleType = D3DMULTISAMPLE_NONE;//D3DMULTISAMPLE_TYPE 
	Params.MultiSampleQuality = 0;//DWORD
	Params.SwapEffect = D3DSWAPEFFECT_DISCARD;//D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT
	Params.hDeviceWindow = m_hWindow;
	Params.Windowed = !_Mode.Get(CMode::EFullscreen);
	Params.EnableAutoDepthStencil = ((_Mode.Get(CMode::EDepthBits) != 0) || (_Mode.Get(CMode::EStencilBits) != 0));
	Params.AutoDepthStencilFormat = VerifyDepthStencil(_Mode.m_aParams[CMode::EDepthBits].m_DeviceParams[EMODE_DEVICEPARAMINDEX_DEPTHSTENCIL]);
	Params.Flags = 0;//DWORD
	Params.FullScreen_RefreshRateInHz = (Params.Windowed ? 0 : _Mode.Get(CMode::ERefreshRate));
	// Set to immediate if we don't want vsync
	Params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	if (SUCCEEDED(m_pD3D->CreateDevice(_Mode.Get(CMode::EAdapter),D3DDEVTYPE_HAL,m_hWindow,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,&Params,&m_p3DDevice)))
	{
		// Get device caps
		m_pD3D->GetDeviceCaps(_Mode.Get(CMode::EAdapter),D3DDEVTYPE_HAL,&m_DeviceCaps);

		// Set viewport
		SetViewport(0,0,Params.BackBufferWidth,Params.BackBufferHeight);

		// Set world matrix to unit
		TMatrix4<fp32> WorldMatrix;
		WorldMatrix.Unit();
		m_p3DDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&WorldMatrix);

		// Turn off lighting
		m_p3DDevice->SetRenderState(D3DRS_LIGHTING,false);

		// Create shader manager
		m_ShaderManager.Create(this);
		
		return true;
	}

	return false;
}

CDirectXDevice::~CDirectXDevice()
{
	Shutdown();
}

// Texture handling
int32_t CDirectXDevice::Internal_CreateTexture(int32_t _Width, int32_t _Height, int32_t _Format, 
											 uint8 *_pData)
{
	if (!m_p3DDevice)
		return -1;

	CDirectXTexture* pTextureInfo = new CDirectXTexture();
	if (!pTextureInfo)
		return -1;
	IDirect3DTexture9* pTexture = NULL;
	IDirect3DTexture9* pVideoMemTexture = NULL;
	D3DFORMAT TextureFormat = GetTextureFormat(_Format);
	if (TextureFormat == 0)
		return -1;
	// Create a texture in system memory, copy the data to this buffer, then create texture in 
	// video memory and then copy system memory version to video mem
	m_p3DDevice->CreateTexture(_Width,_Height,0/*Autogenerates mipmap*/,
		/*D3DUSAGE_AUTOGENMIPMAP*/0,TextureFormat,
		D3DPOOL_SYSTEMMEM, &pTexture,NULL);
	m_p3DDevice->CreateTexture(_Width,_Height,0/*Autogenerates mipmap*/,
		/*D3DUSAGE_AUTOGENMIPMAP?*/0,TextureFormat,
		D3DPOOL_DEFAULT, &pVideoMemTexture,NULL);

	if (!pTexture || !pVideoMemTexture)
	{
		SafeRelease(pTexture);
		SafeRelease(pVideoMemTexture);
		return -1;
	}

	// Lock texture and copy data

	D3DLOCKED_RECT LockedRect;
	if (!SUCCEEDED(pTexture->LockRect(0,&LockedRect,NULL,D3DLOCK_DISCARD)))
	{
		SafeRelease(pTexture);
		SafeRelease(pVideoMemTexture);
		return -1;
	}
	// Copy the source image to system mem texture
	CopyTextureData(_Width,_Height,LockedRect,TextureFormat,_pData,_Format);
	if (!SUCCEEDED(pTexture->UnlockRect(0)))
	{
		SafeRelease(pTexture);
		SafeRelease(pVideoMemTexture);
		return -1;
	}

	// Update systemmem texture to videomem texture
	if (!SUCCEEDED(m_p3DDevice->UpdateTexture(pTexture,pVideoMemTexture)))
	{
		SafeRelease(pTexture);
		SafeRelease(pVideoMemTexture);
		return -1;
	}

	// Wee successfully uploaded the image to videomem, free system mem texture
	SafeRelease(pTexture);
	
	// Add it to the index
	int32_t iTexture = m_IndexHandler.GetFree(EID_Texture);
	m_IndexHandler.SetData(iTexture, pTextureInfo);
	pTextureInfo->m_pTexture = pVideoMemTexture;
	pTextureInfo->m_Info.m_Format = _Format;
	pTextureInfo->m_Info.m_iIndex = iTexture;
	pTextureInfo->m_Info.m_Size[0] = _Width;
	pTextureInfo->m_Info.m_Size[1] = _Height;

	// All done
	return iTexture;
}

void SetTextureData(D3DFORMAT _DestFormat, uint8* _pDest, const uint8 _RGBA[4])
{
	switch(_DestFormat)
	{
	case D3DFMT_R8G8B8:
		{
			//20 24-bit RGB pixel format with 8 bits per channel. 
			_pDest[2] = _RGBA[3];
			_pDest[1] = _RGBA[2];
			_pDest[0] = _RGBA[1];
			break;
		}
	case D3DFMT_A8R8G8B8:
		{
			//32-bit ARGB pixel format with alpha, using 8 bits per channel. 
			_pDest[3] = _RGBA[0];
			_pDest[2] = _RGBA[3];
			_pDest[1] = _RGBA[2];
			_pDest[0] = _RGBA[1];
			break;
		}
	case D3DFMT_X8R8G8B8:
		{
			//32-bit RGB pixel format, where 8 bits are reserved for each color. 
			_pDest[3] = 0;
			_pDest[2] = _RGBA[3];
			_pDest[1] = _RGBA[2];
			_pDest[0] = _RGBA[1];
			break;
		}
	case D3DFMT_R5G6B5:
		{
			//16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
			uint16* pDest = (uint16*)_pDest;
			*pDest = (_RGBA[3]<<11) | (_RGBA[2] << 5) | (_RGBA[1]);
			break;
		}
	case D3DFMT_X1R5G5B5:
		{
			//16-bit pixel format where 5 bits are reserved for each color. 
			uint16* pDest = (uint16*)_pDest;
			*pDest = 0;
			*pDest = (_RGBA[3]<<10) | (_RGBA[2] << 5) | (_RGBA[1]);
			break;
		}
	case D3DFMT_A1R5G5B5:
		{
			//16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
			uint16* pDest = (uint16*)_pDest;
			pDest[0] = (_RGBA[0]<<15) | (_RGBA[3] << 10) | (_RGBA[2] << 5) | (_RGBA[1]);
			break;
		}
	/*case D3DFMT_A4R4G4B4:
		return 2;//16-bit ARGB pixel format with 4 bits for each channel. 
	case D3DFMT_R3G3B2:
		return 1;//8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
	case D3DFMT_A8:
		return 1;//8-bit alpha only. 
	case D3DFMT_A8R3G3B2:
		return 2;//16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
	case D3DFMT_X4R4G4B4:
		return 2;//16-bit RGB pixel format using 4 bits for each color. 
	case D3DFMT_A2B10G10R10:
		return 4;//32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
	case D3DFMT_A8B8G8R8:
		return 4;//32-bit ARGB pixel format with alpha, using 8 bits per channel. 
	case D3DFMT_X8B8G8R8:
		return 4;//32-bit RGB pixel format, where 8 bits are reserved for each color. 
	case D3DFMT_G16R16:
		return 4;//32-bit pixel format using 16 bits each for green and red. 
	case D3DFMT_A2R10G10B10:
		return 4;//32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
	case D3DFMT_A16B16G16R16:
		return 8;//64-bit pixel format using 16 bits for each component. 
	case D3DFMT_A8P8:
		return 1;//8-bit color indexed with 8 bits of alpha. 
	case D3DFMT_P8:
		return 1;//8-bit color indexed. 
	case D3DFMT_L8:
		return 1;//8-bit luminance only. 
	case D3DFMT_L16:
		return 2;//16-bit luminance only. 
	case D3DFMT_A8L8:
		return 2;//16-bit using 8 bits each for alpha and luminance. 
	case D3DFMT_A4L4:
		return 1;//8-bit using 4 bits each for alpha and luminance. 
	case D3DFMT_V8U8:
		return 2;//16-bit bump-map format using 8 bits each for u and v data. 
	case D3DFMT_Q8W8V8U8:
		return 4;//32-bit bump-map format using 8 bits for each channel. 
	case D3DFMT_V16U16:
		return 4;//32-bit bump-map format using 16 bits for each channel. 
	case D3DFMT_Q16W16V16U16:
		return 8;//64-bit bump-map format using 16 bits for each component. 
	case D3DFMT_CxV8U8:
		return 2;//16-bit normal compression format. The texture sampler computes the C channel from: C = sqrt(1 - U2 - V2). 
	case D3DFMT_L6V5U5:
		return 2;//16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u. 
	case D3DFMT_X8L8V8U8:
		return 4;//32-bit bump-map format with luminance using 8 bits for each channel. 
	case D3DFMT_A2W10V10U10:
		return 4;//32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u. */
	default:
		break;
	}
}

void GetTextureData(int32_t _ImageFormat, uint8* _pData, uint8 _RGBA[4])
{
	switch (_ImageFormat)
	{
	case CImage::EFormat_RGB16:
		{
			//16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
			uint16 Value = *(uint16*)_pData;
			_RGBA[3] = (Value&0xF800) >> 11;
			_RGBA[2] = (Value&0x07E0) >> 5;
			_RGBA[1] = (Value&0x001F);
			_RGBA[0] = 0;
			break;
		}
	case CImage::EFormat_RGB24:
		{
			_RGBA[3] = _pData[2];
			_RGBA[2] = _pData[1];
			_RGBA[1] = _pData[0];
			_RGBA[0] = 0;
			break;
		}
	case CImage::EFormat_RGBA32:
		{
			_RGBA[3] = _pData[3];
			_RGBA[2] = _pData[2];
			_RGBA[1] = _pData[1];
			_RGBA[0] = _pData[0];
			break;
		}
	case CImage::EFormat_Device:
	case CImage::EFormat_NotSpecified:
	case CImage::EFormat_Indexed4:
	case CImage::EFormat_BW:
	default:
		{
			// Were fucked now
			_RGBA[0] = 0;
			_RGBA[1] = 0;
			_RGBA[2] = 0;
			_RGBA[3] = 0;
			break;
		}
	}
}

bool CDirectXDevice::CopyTextureData(int32_t _Width, int32_t _Height, const D3DLOCKED_RECT& _LockedRect, 
									 D3DFORMAT _TexFormat, uint8* _pData, int32_t _Format)
{
	int32_t TexelSize = GetTexelSize(_TexFormat);
	// Don't support 64 bit textures at this time
	if (TexelSize <= 0 || TexelSize > 4)
		return false;

	for (int32_t iY = 0; iY < _Height; iY++)
	{
		uint8* pCurrentPos = (uint8*)(_LockedRect.pBits) + iY * _LockedRect.Pitch;
		for (int32_t iX = 0; iX < _Width; iX++)
		{
			uint8 RGBA[4];
			GetTextureData(_Format, _pData, RGBA);
			SetTextureData(_TexFormat, pCurrentPos, RGBA);
			pCurrentPos += TexelSize;
		}
	}
	return true;
}

int32_t CDirectXDevice::CreateTexture(int32_t _Width, int32_t _Height, int32_t _Format)
{
	uint8 *pData = new uint8 [_Width*_Height*4]; // Worst case, temporary data
	int32_t iTex = Internal_CreateTexture(_Width, _Height, _Format, pData);
	SafeArrayDelete(pData);
	return iTex;
}

int32_t CDirectXDevice::CreateTexture(CImage &_Image)
{
	return Internal_CreateTexture(_Image.m_Desc.m_Size.x, _Image.m_Desc.m_Size.y, _Image.m_Desc.m_Format, _Image.m_pData);
}

bool CDirectXDevice::DestroyTexture(int32_t _iTexture)
{
	CDirectXTexture *pTexture = m_IndexHandler.GetObject<CDirectXTexture*>(_iTexture, EID_Texture);
	if(!pTexture)
		return false;

	SafeRelease(pTexture->m_pTexture);
	SafeDelete(pTexture);
	m_IndexHandler.Remove(_iTexture);
	return true;
}

CTextureInfo CDirectXDevice::GetTextureInfo(int32_t _iTexture)
{
	CDirectXTexture *pTexture = m_IndexHandler.GetObject<CDirectXTexture*>(_iTexture, EID_Texture);
	if (pTexture)
		return pTexture->m_Info;
	return CTextureInfo();
}

int32_t CDirectXDevice::CreateVertexBuffer(int32_t _Count, int32_t _What, int32_t _Flags)
{
	CDirectXVertexBuffer* pVertexBuffer = new CDirectXVertexBuffer();
	if (!pVertexBuffer || !m_p3DDevice)
		return -1;
	
	// Create the vertex buffer, in managed memory for now
	D3DPOOL MemPlacement = D3DPOOL_MANAGED;// GetMemPlacement(_Flags);
	bool bCreateOk = true;
	if (_What & EVB_Vertex)
	{
		// Assume fp32 for now
		int32_t BufferSize = GetBufferElementSize(EFormat_fp32) * _Count * ED3D_ELEMENTSPERPOSITION;
		if (!SUCCEEDED(m_p3DDevice->CreateVertexBuffer(BufferSize,0/*Usage*/,0/*FVF*/,MemPlacement,
			&pVertexBuffer->m_pD3DPositionBuffer,NULL)))
		{
			NLog::Write("Error: CDirectXDevice::CreateVertexBuffer: Positionbuffer failed\n");
			bCreateOk = false;
		}
	}
	if (_What & EVB_Color)
	{
		// Must have uint32_t colors?
		int32_t ColorBufferSize = GetBufferElementSize(EFormat_int32) * _Count * ED3D_ELEMENTSPERCOLOR;
		if (!SUCCEEDED(m_p3DDevice->CreateVertexBuffer(ColorBufferSize,0/*Usage*/,0/*FVF*/,MemPlacement,
			&pVertexBuffer->m_pD3DColorBuffer,NULL)))
		{
			NLog::Write("Error: CDirectXDevice::CreateVertexBuffer: ColorBuffer failed\n");
			bCreateOk = false;
		}
	}
	if (_What & EVB_Normal)
	{
		// Must have uint32_t colors?
		int32_t NormalBufferSize = GetBufferElementSize(EFormat_fp32) * _Count * ED3D_ELEMENTSPERNORMAL;
		if (!SUCCEEDED(m_p3DDevice->CreateVertexBuffer(NormalBufferSize,0/*Usage*/,0/*FVF*/,MemPlacement,
			&pVertexBuffer->m_pD3DNormalBuffer,NULL)))
		{
			NLog::Write("Error: CDirectXDevice::CreateVertexBuffer: NormalBuffer failed\n");
			bCreateOk = false;
		}
	}
	if (_What & EVB_TextureMask)
	{
		for (int32_t iTex = 0; iTex < EMaxTextureUnits; iTex++)
		{
			// Assume float2?
			int32_t TextureBufferSize = GetBufferElementSize(EFormat_fp32) * _Count * ED3D_ELEMENTSPERTEXCOORD;
			if (_What & (EVB_TextureBase << iTex))
			{
				if (!SUCCEEDED(m_p3DDevice->CreateVertexBuffer(TextureBufferSize,0/*Usage*/,0/*FVF*/,MemPlacement,
					&pVertexBuffer->m_lpD3DTexCoordbuffer[iTex],NULL)))
				{
					NLog::WriteF("Error: CDirectXDevice::CreateVertexBuffer: TextureBuffer: %d failed", iTex);
					bCreateOk = false;
				}
			}
		}
	}
	// Check if creation was ok
	if (!bCreateOk)
	{
		SafeDelete(pVertexBuffer);
		return -1;
	}
	pVertexBuffer->m_Count = _Count;
	
	// Add it to the index
	int32_t iVertexBuffer = m_IndexHandler.GetFree(EID_VertexBuffer);
	m_IndexHandler.SetData(iVertexBuffer, pVertexBuffer);
	
	return iVertexBuffer;
}

// Get vertexbuffer from id
CVertexBuffer* CDirectXDevice::LockVertexBuffer(int32_t _iBuffer, int32_t _What)
{
	CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iBuffer, EID_VertexBuffer);
	if (!pVertexBuffer)
		return NULL;

	// Set pointers to NULL
	pVertexBuffer->ClearBasePointers();

	bool bResult = true;
	if ((_What & EVB_Vertex) && pVertexBuffer->m_pD3DPositionBuffer)
	{
		//m_pVertexfp32
		if (!SUCCEEDED(pVertexBuffer->m_pD3DPositionBuffer->Lock(0,0,(void**)&pVertexBuffer->m_pVertexfp32,0)))
		{
			NLog::Write("Error: CDirectXDevice::LockVertexBuffer: Positionbuffer failed\n");
			bResult = false;
		}
	}

	if ((_What & EVB_Color) && pVertexBuffer->m_pD3DColorBuffer)
	{
		//m_pColorint32
		if (!SUCCEEDED(pVertexBuffer->m_pD3DColorBuffer->Lock(0,0,(void**)&pVertexBuffer->m_pColorint8,0)))
		{
			NLog::Write("Error: CDirectXDevice::LockVertexBuffer: Colorbuffer failed\n");
			bResult = false;
		}
	}

	if ((_What & EVB_Normal) && pVertexBuffer->m_pD3DNormalBuffer)
	{
		//m_pNormalfp32
		if (!SUCCEEDED(pVertexBuffer->m_pD3DNormalBuffer->Lock(0,0,(void**)&pVertexBuffer->m_pNormalfp32,0)))
		{
			NLog::Write("Error: CDirectXDevice::LockVertexBuffer: Normalbuffer failed\n");
			bResult = false;
		}
	}

	if (_What & EVB_TextureMask)
	{
		// Assume float2?
		for (int32_t iTex = 0; iTex < EMaxTextureUnits; iTex++)
		{
			if ((_What & (EVB_TextureBase << iTex)) && pVertexBuffer->m_lpD3DTexCoordbuffer[iTex])
			{
				//m_pTexturefp32[iTex]
				if (!SUCCEEDED(pVertexBuffer->m_lpD3DTexCoordbuffer[iTex]->Lock(0,0,(void**)&pVertexBuffer->m_pTexturefp32[iTex],0)))
				{
					NLog::WriteF("Error: CDirectXDevice::LockVertexBuffer: Texturebuffer: %d failed",iTex);
					bResult = false;
				}
			}
		}
	}

	return pVertexBuffer;
}

// Return control of vertexbuffer to device
void CDirectXDevice::UnlockVertexBuffer(int32_t _iBuffer, int32_t _What)
{
	CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iBuffer, EID_VertexBuffer);
	if (!pVertexBuffer)
		return;

	bool bResult = true;
	if ((_What & EVB_Vertex) && pVertexBuffer->m_pD3DPositionBuffer)
	{
		if (!SUCCEEDED(pVertexBuffer->m_pD3DPositionBuffer->Unlock()))
		{
			NLog::Write("Error: CDirectXDevice::UnLockVertexBuffer: PositionBuffer failed\n");
			bResult = false;
		}
		pVertexBuffer->m_pVertexfp32 = NULL;
	}

	if ((_What & EVB_Color) && pVertexBuffer->m_pD3DColorBuffer)
	{
		//m_pVertexfp32
		if (!SUCCEEDED(pVertexBuffer->m_pD3DColorBuffer->Unlock()))
		{
			NLog::Write("Error: CDirectXDevice::UnLockVertexBuffer: ColorBuffer failed\n");
		}
		pVertexBuffer->m_pColorint8 = NULL;
	}

	if ((_What & EVB_Normal) && pVertexBuffer->m_pD3DNormalBuffer)
	{
		//m_pNormalfp32
		if (!SUCCEEDED(pVertexBuffer->m_pD3DNormalBuffer->Unlock()))
		{
			NLog::Write("Error: CDirectXDevice::UnLockVertexBuffer: NormalBuffer failed\n");
		}
		pVertexBuffer->m_pNormalfp32 = NULL;
	}

	if (_What & EVB_TextureMask)
	{
		for (int32_t iTex = 0; iTex < EMaxTextureUnits; iTex++)
		{
			if ((_What & (EVB_TextureBase << iTex)) && pVertexBuffer->m_lpD3DTexCoordbuffer[iTex])
			{
				//m_pTexturefp32[iTex]
				if (!SUCCEEDED(pVertexBuffer->m_lpD3DTexCoordbuffer[iTex]->Unlock()))
				{
					NLog::WriteF("Error: CDirectXDevice::UnLockVertexBuffer: TextureBuffer: %d failed", iTex);
				}
				pVertexBuffer->m_pTexturefp32[iTex] = NULL;
			}
		}
	}
}

bool CDirectXDevice::DestroyVertexBuffer(int32_t _iBuffer)
{
	CDirectXVertexBuffer *pVertexBuffer = m_IndexHandler.GetObject<CDirectXVertexBuffer*>(_iBuffer, EID_VertexBuffer);
	// Let destructor handle it
	SafeDelete(pVertexBuffer);

	return true;
}

// Index buffers
int32_t CDirectXDevice::CreateIndexBuffer(int32_t _Count, int32_t _Flags)
{
	CDirectXIndexBuffer* pIndexBuffer = new CDirectXIndexBuffer();
	if (!pIndexBuffer || !m_p3DDevice)
		return -1;

	// Create the index buffer, in managed memory for now
	D3DPOOL MemPlacement = D3DPOOL_MANAGED;// GetMemPlacement(_Flags);
	// Assume uint16 for now
	D3DFORMAT IndexFormat = D3DFMT_INDEX16;
	// Assume fp32 for now
	int32_t BufferSize = GetBufferElementSize(EFormat_int16) * _Count * ED3D_ELEMENTSPERINDEX;
	if (!SUCCEEDED(m_p3DDevice->CreateIndexBuffer(BufferSize,0/*Usage*/,IndexFormat,MemPlacement,
		&pIndexBuffer->m_pD3DIndexBuffer,NULL)))
	{
		NLog::Write("Error: CDirectXDevice::CreateIndexBuffer: Indexbuffer failed\n");
		SafeDelete(pIndexBuffer);
		return -1;
	}

	pIndexBuffer->m_Count = _Count;

	// Add it to the index
	int32_t iIndexBuffer = m_IndexHandler.GetFree(EID_IndexBuffer);
	m_IndexHandler.SetData(iIndexBuffer, pIndexBuffer);

	return iIndexBuffer;
}

bool CDirectXDevice::DestroyIndexBuffer(int32_t _iBuffer)
{
	CDirectXIndexBuffer *pIndexBuffer = m_IndexHandler.GetObject<CDirectXIndexBuffer*>(_iBuffer, EID_IndexBuffer);
	// Let destructor handle it
	SafeDelete(pIndexBuffer);

	return true;
}

// Get indexbuffer from id, Return control of indexbuffer to device
CIndexBuffer* CDirectXDevice::LockIndexBuffer(int32_t _iBuffer)
{
	CDirectXIndexBuffer *pIndexBuffer = m_IndexHandler.GetObject<CDirectXIndexBuffer*>(_iBuffer, EID_IndexBuffer);
	if (!pIndexBuffer)
		return NULL;

	// Set pointers to NULL
	pIndexBuffer->m_pIndexint16 = NULL;

	if (pIndexBuffer->m_pD3DIndexBuffer)
	{
		//m_pIndexint16
		MACRO_DXCHECKERROR(pIndexBuffer->m_pD3DIndexBuffer->Lock(0,0,
			(void**)&pIndexBuffer->m_pIndexint16,0),"Error: CDirectXDevice::LockIndexBuffer:\n");
	}

	return pIndexBuffer;
}

void CDirectXDevice::UnlockIndexBuffer(int32_t _iBuffer)
{
	CDirectXIndexBuffer *pIndexBuffer = m_IndexHandler.GetObject<CDirectXIndexBuffer*>(_iBuffer, EID_IndexBuffer);
	if (!pIndexBuffer)
		return;

	if (pIndexBuffer->m_pD3DIndexBuffer)
	{
		//m_pIndexint16
		MACRO_DXCHECKERROR(pIndexBuffer->m_pD3DIndexBuffer->Unlock(),"Error: CDirectXDevice::UnlockIndexBuffer:\n");
	}
}

// Shaders
int32_t CDirectXDevice::CreateVertexShader(const int8 *_pString)
{
	if((m_DeviceCaps.VertexShaderVersion < D3DVS_VERSION(1,1)) || !_pString)
		return -1;
	// Assemble shader
	ID3DXBuffer* pShaderCode = NULL;
	ID3DXBuffer* pErrorMsgs = NULL;
	if (SUCCEEDED(D3DXAssembleShader(_pString,NFoo::StringLength(_pString),NULL,NULL, 0, 
			&pShaderCode, &pErrorMsgs)))
	{
		IDirect3DVertexShader9* pVertexShader = NULL;
		if (SUCCEEDED(m_p3DDevice->CreateVertexShader((DWORD*)pShaderCode->GetBufferPointer(),
			&pVertexShader)))
		{
			// Add it to the index handler
			CDirectXVertexShader *pShader = new CDirectXVertexShader;
			pShader->m_pVertexShader = pVertexShader;
			int32_t iShader = m_IndexHandler.GetFree(EID_VertexShader);
			m_IndexHandler.SetData(iShader, pShader);
			// Release code/errormsgs
			SafeRelease(pShaderCode);
			SafeRelease(pErrorMsgs);

			return iShader;
		}
	}

	SafeRelease(pShaderCode);
	SafeRelease(pErrorMsgs);
	return -1;
}

int32_t CDirectXDevice::CreatePixelShader(const int8 *_pString)
{
	if((m_DeviceCaps.PixelShaderVersion < D3DPS_VERSION(1,1)) || !_pString)
		return -1;
	// Assemble shader
	ID3DXBuffer* pShaderCode = NULL;
	ID3DXBuffer* pErrorMsgs = NULL;
	if (SUCCEEDED(D3DXAssembleShader(_pString,NFoo::StringLength(_pString),NULL,NULL, 0, 
		&pShaderCode, &pErrorMsgs)))
	{
		IDirect3DPixelShader9* pPixelShader = NULL;
		if (SUCCEEDED(m_p3DDevice->CreatePixelShader((DWORD*)pShaderCode->GetBufferPointer(),
			&pPixelShader)))
		{
			// Add it to the index handler
			CDirectXPixelShader *pShader = new CDirectXPixelShader;
			pShader->m_pPixelShader = pPixelShader;
			int32_t iShader = m_IndexHandler.GetFree(EID_PixelShader);
			m_IndexHandler.SetData(iShader, pShader);
			// Release code/error msgs
			SafeRelease(pShaderCode);
			SafeRelease(pErrorMsgs);
			return iShader;
		}
	}

	SafeRelease(pShaderCode);
	SafeRelease(pErrorMsgs);
	return -1;
}

bool CDirectXDevice::DestroyVertexShader(int32_t _iShader)
{
	if (_iShader == -1)
		return false;
	
	CDirectXVertexShader *pShader = m_IndexHandler.GetObject<CDirectXVertexShader*>(_iShader, EID_VertexShader);
	if (pShader)
	{
		SafeDelete(pShader);
		return true;
	}
	return false;
}

bool CDirectXDevice::DestroyPixelShader(int32_t _iShader)
{
	if (_iShader == -1)
		return false;

	CDirectXPixelShader *pShader = m_IndexHandler.GetObject<CDirectXPixelShader*>(_iShader, EID_PixelShader);
	if (pShader)
	{
		SafeDelete(pShader);
		return true;
	}
	return false;
}

void CDirectXDevice::SetViewport(int32_t _X, int32_t _Y, int32_t _Width, int32_t _Height)
{
	// Set viewport
	D3DVIEWPORT9 ViewPort;
	ViewPort.X = _X;
	ViewPort.Y = _Y;
	ViewPort.Width = _Width;
	ViewPort.Height = _Height;
	ViewPort.MinZ = 0.0f; // Default
	ViewPort.MaxZ = 1.0f; // Default
	MACRO_DXCHECKERROR(m_p3DDevice->SetViewport(&ViewPort),"Error: CDirectXDevice::SetViewport\n");
}

// Clearing
void CDirectXDevice::Clear(intr _What)
{
	if (m_p3DDevice)
	{
		int32_t ClearFlags = 0;
		if (_What & EBuffer_Color)
			ClearFlags |= D3DCLEAR_TARGET;
		if (_What & EBuffer_Depth)
			ClearFlags |= D3DCLEAR_ZBUFFER;
		if (_What & EBuffer_Stencil)
			ClearFlags |= D3DCLEAR_STENCIL;

		int32_t ClearColor = (int32_t)(255.0f * m_ClearColor[3]) << 24; //A
		ClearColor |= (int32_t)(255.0f * m_ClearColor[0]) << 16; // R
		ClearColor |= (int32_t)(255.0f * m_ClearColor[1]) << 8; // G
		ClearColor |= (int32_t)(255.0f * m_ClearColor[2]) << 0; // B
		m_p3DDevice->Clear(0,NULL,ClearFlags,ClearColor,m_ClearDepth,m_ClearStencil);
	}
}

bool CDirectXDevice::SwapBuffers()
{
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->Present(NULL,NULL,NULL,NULL),"Error: CDirectXDevice::SwapBuffers\n");
		return true;
	}
	return false;
}

bool CDirectXDevice::Shutdown()
{
	// Release devices
	SafeRelease(m_pVertexDeclaration);
	SafeRelease(m_p3DDevice);
	SafeRelease(m_pD3D);

	// destroy window and unregister window class
	DestroyWindow(m_hWindow);
	//UnregisterClass("Foo D3D", m_hInstance); 
	return true;
}

bool CDirectXDevice::Update()
{
	return true;
}

void CDirectXDevice::Draw(int32_t _Mode, int32_t _Start, int32_t _Count)
{
	// Mmkay then, if the stream declaration isn't the same as the current stream status
	// renew declaration
	if (m_StreamFlags != m_StreamDeclSet)
		SetStreamDeclaration();

	// Can't draw quads
	if (_Mode == EDraw_Quads)
	{
		NLog::Write("Error: CDirectXDevice::Draw: Warning Quads not supported yet\n");
		return;
	}

	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->BeginScene(),"Error: CDirectXDevice::Draw: Begin Scene\n");
		MACRO_DXCHECKERROR(m_p3DDevice->DrawPrimitive(g_aDrawTable[_Mode],_Start,_Count),"Error: CDirectXDevice::Draw\n");
		MACRO_DXCHECKERROR(m_p3DDevice->EndScene(),"Error: CDirectXDevice::Draw: End Scene\n");
	}
}

void CDirectXDevice::DrawIndexed(int32_t _Mode, int32_t _Start, int32_t _Count)
{
	// Mmkay then, if the stream declaration isn't the same as the current stream status
	// renew declaration
	if (m_StreamFlags != m_StreamDeclSet)
		SetStreamDeclaration();
	
	// Can't draw quads
	if (_Mode == EDraw_Quads)
	{
		NLog::Write("Error: CDirectXDevice::DrawIndexed: Warning Quads not supported yet\n");
		return;
	}
	if (m_p3DDevice)
	{
		MACRO_DXCHECKERROR(m_p3DDevice->BeginScene(),"Error: CDirectXDevice::DrawIndexed: Begin Scene\n");
		int32_t NumVertices = g_aBaseTable[_Mode] + g_aExtraTable[_Mode] * (_Count - 1);
		MACRO_DXCHECKERROR(m_p3DDevice->DrawIndexedPrimitive(g_aDrawTable[_Mode],0,0,NumVertices,_Start,_Count),"Error: CDirectXDevice::DrawIndexed: DrawIndexed\n");
		MACRO_DXCHECKERROR(m_p3DDevice->EndScene(),"Error: CDirectXDevice::DrawIndexed: End Scene\n");
	}
}

D3DFORMAT CDirectXDevice::VerifyColorFormat(uint32_t _Format)
{
	D3DFORMAT Format = (D3DFORMAT)_Format;
	switch (Format)
	{
	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
	case D3DFMT_R3G3B2:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
		{
			return Format;
		}
	default:
		return (D3DFORMAT)0;
	}
}

D3DFORMAT CDirectXDevice::VerifyDepthStencil(uint32_t _Format)
{
	D3DFORMAT Format = (D3DFORMAT)_Format;
	switch(Format)
	{
	case D3DFMT_D16:
	case D3DFMT_D15S1:
	case D3DFMT_D24X8:
	case D3DFMT_D24S8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D32:
		return Format;
	default:
		return (D3DFORMAT)0;
	}
}

// D3d Texture format from CImage::Eformat...
D3DFORMAT CDirectXDevice::GetTextureFormat(int32_t _Format)
{
	switch (_Format)
	{
	case CImage::EFormat_Device:
	case CImage::EFormat_NotSpecified:
	case CImage::EFormat_Indexed4:
	case CImage::EFormat_BW:
		return (D3DFORMAT)0;
	case CImage::EFormat_RGB16:
		return D3DFMT_R5G6B5;
	case CImage::EFormat_RGB24:
		return D3DFMT_R8G8B8;
	case CImage::EFormat_RGBA32:
		return D3DFMT_A8R8G8B8;
	default:
		return (D3DFORMAT)0;
	}
}

int32_t CDirectXDevice::ColorBits(D3DFORMAT _Format)
{
	switch( _Format )
	{
	case D3DFMT_R8G8B8:
		return 24;
	case D3DFMT_A8R8G8B8:
		return 24;
	case D3DFMT_X8R8G8B8:
		return 24;
	case D3DFMT_R5G6B5:
		return 16;
	case D3DFMT_X1R5G5B5:
		return 15;
	case D3DFMT_A1R5G5B5:
		return 15;
	case D3DFMT_A4R4G4B4:
		return 12;
	case D3DFMT_R3G3B2:
		return 8;
	case D3DFMT_A8R3G3B2:
		return 8;
	case D3DFMT_X4R4G4B4:
		return 12;
	case D3DFMT_A2B10G10R10:
		return 30;
	case D3DFMT_A2R10G10B10:
		return 30;
	default:
		return 0;
	}
}

int32_t CDirectXDevice::GetTexelSize(D3DFORMAT _Format)
{
	switch( _Format )
	{
	case D3DFMT_R8G8B8:
		return 3;//20 24-bit RGB pixel format with 8 bits per channel. 
	case D3DFMT_A8R8G8B8:
		return 4;//32-bit ARGB pixel format with alpha, using 8 bits per channel. 
	case D3DFMT_X8R8G8B8:
		return 4;//32-bit RGB pixel format, where 8 bits are reserved for each color. 
	case D3DFMT_R5G6B5:
		return 2;//16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 
	case D3DFMT_X1R5G5B5:
		return 2;//16-bit pixel format where 5 bits are reserved for each color. 
	case D3DFMT_A1R5G5B5:
		return 2;//16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
	case D3DFMT_A4R4G4B4:
		return 2;//16-bit ARGB pixel format with 4 bits for each channel. 
	case D3DFMT_R3G3B2:
		return 1;//8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 
	case D3DFMT_A8:
		return 1;//8-bit alpha only. 
	case D3DFMT_A8R3G3B2:
		return 2;//16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
	case D3DFMT_X4R4G4B4:
		return 2;//16-bit RGB pixel format using 4 bits for each color. 
	case D3DFMT_A2B10G10R10:
		return 4;//32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
	case D3DFMT_A8B8G8R8:
		return 4;//32-bit ARGB pixel format with alpha, using 8 bits per channel. 
	case D3DFMT_X8B8G8R8:
		return 4;//32-bit RGB pixel format, where 8 bits are reserved for each color. 
	case D3DFMT_G16R16:
		return 4;//32-bit pixel format using 16 bits each for green and red. 
	case D3DFMT_A2R10G10B10:
		return 4;//32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha. 
	case D3DFMT_A16B16G16R16:
		return 8;//64-bit pixel format using 16 bits for each component. 
	case D3DFMT_A8P8:
		return 1;//8-bit color indexed with 8 bits of alpha. 
	case D3DFMT_P8:
		return 1;//8-bit color indexed. 
	case D3DFMT_L8:
		return 1;//8-bit luminance only. 
	case D3DFMT_L16:
		return 2;//16-bit luminance only. 
	case D3DFMT_A8L8:
		return 2;//16-bit using 8 bits each for alpha and luminance. 
	case D3DFMT_A4L4:
		return 1;//8-bit using 4 bits each for alpha and luminance. 
	case D3DFMT_V8U8:
		return 2;//16-bit bump-map format using 8 bits each for u and v data. 
	case D3DFMT_Q8W8V8U8:
		return 4;//32-bit bump-map format using 8 bits for each channel. 
	case D3DFMT_V16U16:
		return 4;//32-bit bump-map format using 16 bits for each channel. 
	case D3DFMT_Q16W16V16U16:
		return 8;//64-bit bump-map format using 16 bits for each component. 
	case D3DFMT_CxV8U8:
		return 2;//16-bit normal compression format. The texture sampler computes the C channel from: C = sqrt(1 - U2 - V2). 
	case D3DFMT_L6V5U5:
		return 2;//16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u. 
	case D3DFMT_X8L8V8U8:
		return 4;//32-bit bump-map format with luminance using 8 bits for each channel. 
	case D3DFMT_A2W10V10U10:
		return 4;//32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u. 
	default:
		return 0;
	}
}

int32_t CDirectXDevice::GetBufferElementSize(int32_t _What)
{
	switch (_What)
	{
	case EFormat_int8:
		return 1;
	case EFormat_int16:
		return 2;
	case EFormat_int32:
		return 4;
	case EFormat_fp32:
		return 4;
	case EFormat_fp64:
		return 8;
	default:
		return 0;
	}
}

void CDirectXDevice::ZeroCaps(D3DCAPS9& _Caps)
{
	memset(&_Caps,0,sizeof(_Caps));
}

D3DVERTEXELEMENT9 GetDecl(WORD Stream,WORD Offset,BYTE Type,BYTE Method,BYTE Usage,BYTE UsageIndex)
{
	D3DVERTEXELEMENT9 Decl;
	Decl.Stream = Stream;
	Decl.Offset = Offset;
	Decl.Type = Type;
	Decl.Method = Method;
	Decl.Usage = Usage;
	Decl.UsageIndex = UsageIndex;
	return Decl;
}

void CDirectXDevice::SetStreamDeclaration()
{
	if (!m_p3DDevice)
		return;
	// Max declarations
	// StreamId, Offset,Type,Method,Usage,Usageindex
	D3DVERTEXELEMENT9 aDecl[ED3D_STREAMSOURCE_NUMSTREAMS];
	int32_t CurrentIndex = 0;
	if (m_StreamFlags & ED3D_STREAMFLAG_POSITION)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_POSITION, 0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_NORMAL)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_NORMAL, 0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_COLOR)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_COLOR, 0,D3DDECLTYPE_D3DCOLOR,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_COLOR,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_TEXCOORD0)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_TEXCOORD0, 0,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_TEXCOORD1)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_TEXCOORD1, 0,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_TEXCOORD2)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_TEXCOORD2, 0,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_TEXCOORD3)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_TEXCOORD3, 0,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0);
		CurrentIndex++;
	}
	if (m_StreamFlags & ED3D_STREAMFLAG_TEXCOORD4)
	{
		aDecl[CurrentIndex] = GetDecl(ED3D_STREAMSOURCEINDEX_TEXCOORD4, 0,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0);
		CurrentIndex++;
	}
	
	// Set end of declaration
	aDecl[CurrentIndex] = GetDecl(0xFF,0,D3DDECLTYPE_UNUSED,0,0,0);
	// Release previous declaration
	SafeRelease(m_pVertexDeclaration);
	MACRO_DXCHECKERROR(m_p3DDevice->CreateVertexDeclaration(aDecl, &m_pVertexDeclaration),"Error: CDirectXDevice::SetStreamDeclaration\n");
	MACRO_DXCHECKERROR(m_p3DDevice->SetVertexDeclaration(m_pVertexDeclaration),"Error: CDirectXDevice::SetStreamDeclaration\n");
	// Indicate updated vertex declaration
	m_StreamDeclSet = m_StreamFlags;
}

CDirectXDevice::CDirectXVertexBuffer::CDirectXVertexBuffer()
{
	m_pD3DPositionBuffer = NULL;
	m_pD3DNormalBuffer = NULL;
	m_pD3DColorBuffer = NULL;
	for (int32_t iTex = 0; iTex < EMaxTextureUnits; iTex++)
		m_lpD3DTexCoordbuffer[iTex] = NULL;
}
CDirectXDevice::CDirectXVertexBuffer::~CDirectXVertexBuffer()
{
	SafeRelease(m_pD3DPositionBuffer);
	SafeRelease(m_pD3DNormalBuffer);
	SafeRelease(m_pD3DColorBuffer);
	for (int32_t iTex = 0; iTex < EMaxTextureUnits; iTex++)
		SafeRelease(m_lpD3DTexCoordbuffer[iTex]);
}

void CDirectXDevice::CDirectXVertexBuffer::ClearBasePointers()
{
	m_pVertexfp32 = NULL;
	m_pNormalfp32 = NULL;
	m_pColorint8 = NULL;
	for (int32_t i = 0; i < EMaxTextureUnits; i++)
		m_pTexturefp32[i] = NULL;
}

CDirectXDevice::CDirectXIndexBuffer::CDirectXIndexBuffer()
{
	m_pD3DIndexBuffer = NULL;
}

CDirectXDevice::CDirectXIndexBuffer::~CDirectXIndexBuffer()
{
	SafeRelease(m_pD3DIndexBuffer);
}

MACRO_REGISTER_CLASS(CDirectXDevice, N3D::aDeviceClassTag)
