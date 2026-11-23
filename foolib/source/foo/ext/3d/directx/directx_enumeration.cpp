
#include <base/base.h>
#include <base/log.h>
#include <func/string.h>

#include "directx.h"

//
using namespace NFoo;
using namespace NFoo::N3D;
using namespace NFoo::N3D::NDirectX;

#define MACRO_CHECKMODE(mode1,mode2,mode)\
	if (!##mode1.m_aParams[##mode].Accaptable(##mode2.m_aParams[##mode]))\
	bCheck = false;

int32 CDirectXDevice::EnumDisplayModes(TDynamicArray<CMode> &_rModes, CMode _Mode)
{
	// No direct3d created
	if (!m_pD3D)
		return 0;

	// Check against mode...
	D3DFORMAT AllowedFormats[] = 
	{
		D3DFMT_X8R8G8B8, 
			D3DFMT_A8R8G8B8, 
			D3DFMT_A2R10G10B10,
			D3DFMT_X1R5G5B5,
			D3DFMT_A1R5G5B5,
			D3DFMT_R5G6B5,
	};

	int32 AllowedFormatsLen = sizeof(AllowedFormats) / sizeof(D3DFORMAT);

	int32 NumAdapters = m_pD3D->GetAdapterCount();

	for (int32 iAdapter = 0; iAdapter < NumAdapters; iAdapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		//TDynamicArray<D3DFORMAT> AdapterFormats;
		m_pD3D->GetAdapterIdentifier(iAdapter, 0, &Identifier);

		// Get list of all display modes on this adapter.  
		// Also build a temporary list of all display adapter formats.
		for (int32 iFormat = 0; iFormat < AllowedFormatsLen; iFormat++)
		{
			D3DFORMAT AdapterFormat = AllowedFormats[iFormat];
			int32 NumAdapterModes = m_pD3D->GetAdapterModeCount(iAdapter, AdapterFormat);
			for (int32 iMode = 0; iMode < NumAdapterModes; iMode++)
			{
				D3DDISPLAYMODE DisplayMode;
				if (m_pD3D->EnumAdapterModes(iAdapter, AdapterFormat, iMode, &DisplayMode) != D3D_OK)
					continue;

				CMode Mode;
				Mode.Set(CMode::EWidth, CMode::EType_Specified, DisplayMode.Width);
				Mode.Set(CMode::EHeight, CMode::EType_Specified, DisplayMode.Height);
				// Refresh 0 == adapter default....
				Mode.Set(CMode::ERefreshRate, CMode::EType_Specified, DisplayMode.RefreshRate);
				Mode.Set(CMode::EColorBits, CMode::EType_Specified, ColorBits(DisplayMode.Format));
				Mode.Set(CMode::EAdapter, CMode::EType_Specified, iAdapter);

				// Beh, test the ones we have...
				bool bCheck = true;
				MACRO_CHECKMODE(_Mode,Mode,CMode::EWidth);
				MACRO_CHECKMODE(_Mode,Mode,CMode::EHeight);
				MACRO_CHECKMODE(_Mode,Mode,CMode::ERefreshRate);
				MACRO_CHECKMODE(_Mode,Mode,CMode::EColorBits);
				MACRO_CHECKMODE(_Mode,Mode,CMode::EAdapter);

				// Just continue if the mode wasn't ok
				if (!bCheck)
					continue;

				// Ok the display mode was ok, now test all possible stencil/z formats as well
				TDynamicArray<SBBufferMode> lBackBufferFormat;

				// Find Backbuffermodes for both windowed and fullscreen
				GetBackBufferFormats(iAdapter, DisplayMode.Format, lBackBufferFormat, false, _Mode);
				GetBackBufferFormats(iAdapter, DisplayMode.Format, lBackBufferFormat, true, _Mode);
				int32 BBufferFormatLen = lBackBufferFormat.Len();
				for (int32 iBBuffer = 0; iBBuffer < BBufferFormatLen; iBBuffer++)
				{
					TDynamicArray<D3DFORMAT> lDepthModes;
					GetDepthModes(iAdapter,DisplayMode.Format,lBackBufferFormat[iBBuffer].m_Format,lDepthModes,_Mode);
					CMode TempMode = Mode;
					TempMode.Set(CMode::EFullscreen, CMode::EType_Specified, !lBackBufferFormat[iBBuffer].m_bWindowed);

					// Hum hum, ok depthmodes can be used for both stencil/z buffer so make a nice 
					int32 DepthModeLen = lDepthModes.Len();
					for (int32 iDepth = 0; iDepth < DepthModeLen; iDepth++)
					{
						TempMode.Set(CMode::EAlphaBits, CMode::EType_Specified, AlphaBits(lBackBufferFormat[iBBuffer].m_Format), lBackBufferFormat[iBBuffer].m_Format);
						TempMode.Set(CMode::EDepthBits,CMode::EType_Specified, DepthBits(lDepthModes[iDepth]),lDepthModes[iDepth]);
						TempMode.Set(CMode::EStencilBits, CMode::EType_Specified, StencilBits(lDepthModes[iDepth]),lDepthModes[iDepth]);
						AddDeviceMode(_rModes,TempMode,_Mode);
						/*for (int32 iStencil = 0; iStencil < DepthModeLen; iStencil++)
						{
						// No way to find d3d acceptable in fullscreen?
						TempMode.Set(CMode::EStencilBits, CMode::EType_Specified,lDepthModes[iStencil]);
						AddDeviceMode(_rModes,TempMode,_Mode);
						}*/
					}
				}
			}
		}
	}

	return _rModes.Len();
}

bool CDirectXDevice::AddDeviceMode(TDynamicArray<CMode>& _rModes, const CMode& _AddMode, const CMode& _ModeRef)
{
	if (_ModeRef.Acceptable(_AddMode))
	{
		int32 CurrentLen = _rModes.Len();
		bool bFound = false;
		for(intr m = 0; m < CurrentLen; m++)
		{
			if(_AddMode.Compare(_rModes[m]))
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			_rModes.Add(_AddMode);
			/*NFoo::CString Hej("Succ: Width: %.4d Height: %.4d Refresh: %.3d Full: %.1d Color: %.2d Stencil: %.2d Depth: %.2d Alpha: %.2d Adapter: %.2d\n",
			_AddMode.m_aParams[0].m_Value, _AddMode.m_aParams[1].m_Value, _AddMode.m_aParams[2].m_Value, _AddMode.m_aParams[3].m_Value,
			_AddMode.m_aParams[4].m_Value, _AddMode.m_aParams[5].m_Value, _AddMode.m_aParams[6].m_Value, _AddMode.m_aParams[7].m_Value,
			_AddMode.m_aParams[8].m_Value);
			NLog::Write(Hej);*/

			return true;
		}
	}
	return false;
}

int32 CDirectXDevice::AlphaBits(D3DFORMAT _Format)
{
	switch (_Format)
	{
	case D3DFMT_R8G8B8:
		return 0;
	case D3DFMT_A8R8G8B8:
		return 8;
	case D3DFMT_X8R8G8B8:
		return 0;
	case D3DFMT_R5G6B5:
		return 0;
	case D3DFMT_X1R5G5B5:
		return 0;
	case D3DFMT_A1R5G5B5:
		return 1;
	case D3DFMT_A4R4G4B4:
		return 4;
	case D3DFMT_R3G3B2:
		return 0;
	case D3DFMT_A8R3G3B2:
		return 8;
	case D3DFMT_X4R4G4B4:
		return 0;
	case D3DFMT_A2B10G10R10:
		return 2;
	case D3DFMT_A2R10G10B10:
		return 2;
	default:
		return 0;
	}
}

int32 CDirectXDevice::DepthBits(D3DFORMAT _Format)
{
	switch (_Format)
	{
	case D3DFMT_D16:
		return 16;
	case D3DFMT_D15S1:
		return 15;
	case D3DFMT_D24X8:
		return 24;
	case D3DFMT_D24S8:
		return 24;
	case D3DFMT_D24X4S4:
		return 24;
	case D3DFMT_D32:
		return 32;
	default:
		return 0;
	}
}

int32 CDirectXDevice::StencilBits(D3DFORMAT _Format)
{
	switch (_Format)
	{
	case D3DFMT_D16:
		return 0;
	case D3DFMT_D15S1:
		return 1;
	case D3DFMT_D24X8:
		return 0;
	case D3DFMT_D24S8:
		return 8;
	case D3DFMT_D24X4S4:
		return 4;
	case D3DFMT_D32:
		return 0;
	default:
		return 0;
	}
}

void CDirectXDevice::GetBackBufferFormats(uint32 _iAdapter, D3DFORMAT _DisplayFormat, 
										  TDynamicArray<SBBufferMode>& _BBModes, bool _bWindowed, const CMode& _Mode)
{
	// Just assume that we always want hardware acc
	D3DFORMAT BBFormats[] = 
	{ 
		D3DFMT_A2R10G10B10,
			D3DFMT_A8R8G8B8,
			D3DFMT_X8R8G8B8,
			D3DFMT_A1R5G5B5,
			D3DFMT_X1R5G5B5,
			D3DFMT_R5G6B5,
	};

	int32 FormatLen = sizeof(BBFormats) / sizeof(D3DFORMAT);

	// Check all stencil modes
	for (int32 i = 0; i < FormatLen; i++)
	{
		// Check format
		if (SUCCEEDED(m_pD3D->CheckDeviceType(_iAdapter, D3DDEVTYPE_HAL, _DisplayFormat, 
			BBFormats[i],_bWindowed)))
		{
			// Add depth mode
			SBBufferMode Mode;
			Mode.m_Format = BBFormats[i];
			Mode.m_bWindowed = _bWindowed;
			_BBModes.Add(Mode);
		}
	}
}

// Finds surface types for given diplayformat
void CDirectXDevice::GetDepthModes(uint32 _iAdapter, D3DFORMAT _DisplayFormat, D3DFORMAT _BBufferFormat,
								   TDynamicArray<D3DFORMAT>& _DepthModes, const CMode& _Mode)
{
	// Just assume that we always want hardware acc
	D3DFORMAT DepthFormats[] = 
	{ 
		D3DFMT_D16_LOCKABLE,
			D3DFMT_D32,
			D3DFMT_D15S1,
			D3DFMT_D24S8,
			D3DFMT_D24X8,
			D3DFMT_D24X4S4,
			D3DFMT_D32F_LOCKABLE,
			D3DFMT_D24FS8,
			D3DFMT_D16,
			D3DFMT_VERTEXDATA,
			D3DFMT_INDEX16,
			D3DFMT_INDEX32,
	};

	int32 FormatLen = sizeof(DepthFormats) / sizeof(D3DFORMAT);

	// Check all stencil modes
	for (int32 i = 0; i < FormatLen; i++)
	{
		// Check format
		if (SUCCEEDED(m_pD3D->CheckDeviceFormat(_iAdapter, D3DDEVTYPE_HAL, _DisplayFormat, 
			D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DepthFormats[i])))
		{
			if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch(_iAdapter, D3DDEVTYPE_HAL, _DisplayFormat,
				_BBufferFormat, DepthFormats[i])))
			{
				// Add depth mode
				_DepthModes.Add(DepthFormats[i]);
			}
		}
	}
}
