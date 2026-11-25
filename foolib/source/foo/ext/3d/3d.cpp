
#include <foo/base/log.h>
#include "3d.h"

using namespace NFoo;
using namespace NFoo::N3D;

//
//
//
void CState::Stream_Clear()
{
	Stream_Vertex(-1);
	Stream_Index(-1);
	Stream_Normal(-1);
	Stream_Color(-1);
	
	for(intr i = 0; i < EMaxTextureUnits; i++)
		Stream_Texture(i, -1, -1);
}

//
//
//
void CState::Stream_Set(int32_t _iVB, int32_t _What)
{
	if(_What&EVB_Vertex) Stream_Vertex(_iVB);
	if(_What&EVB_Normal) Stream_Normal(_iVB);
	if(_What&EVB_Color) Stream_Color(_iVB);

	for(intr i = 0; i < EMaxTextureUnits; i++)
		if(_What&(EVB_TextureBase<<i))
			Stream_Texture(i, _iVB, i);

//	NLog::WriteF("3d: warning, call to unimplemented function, Stream_Set\n");
}


//
//
//
void CState::State_Set(const CState &_rState, bool _Force)
{
	if(_Force)
	{
		// flags
		for(intr f = 0; f < ENumFlags; f++)
			Flag_Set(f, _rState.m_aFlags[f]>0);

		// modes
		Mode_BlendFunc(_rState.m_BlendSrc, _rState.m_BlendDst);
		Mode_DepthTest(_rState.m_DepthTest);
		Mode_Cull(_rState.m_Cull);
		Mode_StencilOp(_rState.m_StencilOpFail, _rState.m_StencilOpZFail, _rState.m_StencilOpZPass);
		Mode_StencilFunc(_rState.m_StencilTest, _rState.m_StencilRef, _rState.m_StencilMask);
		Mode_PrimaryColor(_rState.m_PrimaryColor.x, _rState.m_PrimaryColor.y, _rState.m_PrimaryColor.z, _rState.m_PrimaryColor.w);
		Mode_WriteMask(	_rState.m_WriteMask[0],_rState.m_WriteMask[1],
						_rState.m_WriteMask[2],_rState.m_WriteMask[3]);

		
		// streams
		Stream_Vertex(_rState.m_iVertexStream);
		Stream_Index(_rState.m_iIndexStream);
		Stream_Normal(_rState.m_iNormalStream);
		Stream_Color(_rState.m_iColorStream);

		// textures and texture coord stream
		for(intr i = 0; i < EMaxTextureUnits; i++)
		{
			Stream_Texture(i, _rState.m_aiTextureStreamVB[i], _rState.m_aiTextureStreamVBIndex[i]);
			Texture_Set(i, _rState.m_iaTexture[i]);
		}

		// shaders
		Shader_Vertex(_rState.m_iVertexShader);
		Shader_Pixel(_rState.m_iPixelShader);
	}
	else
	{
		// Flags
		for(intr f = 0; f < ENumFlags; f++)
			if(m_aFlags[f] != _rState.m_aFlags[f])
				Flag_Set(f, _rState.m_aFlags[f]>0);

		// modes
		if(m_BlendSrc != _rState.m_BlendSrc || m_BlendDst != _rState.m_BlendDst)
			Mode_BlendFunc(_rState.m_BlendSrc, _rState.m_BlendDst);

		if(m_DepthTest != _rState.m_DepthTest)
			Mode_DepthTest(_rState.m_DepthTest);

		if(m_Cull != _rState.m_Cull)
			Mode_Cull(_rState.m_Cull);

		if(	m_StencilOpFail != _rState.m_StencilOpFail ||
			m_StencilOpZFail != _rState.m_StencilOpZFail ||
			m_StencilOpZPass != _rState.m_StencilOpZPass)
		{
			Mode_StencilOp(_rState.m_StencilOpFail, _rState.m_StencilOpZFail, _rState.m_StencilOpZPass);
		}

		Mode_WriteMask(	_rState.m_WriteMask[0],_rState.m_WriteMask[1],
						_rState.m_WriteMask[2],_rState.m_WriteMask[3]);

		if(	m_StencilTest != _rState.m_StencilTest ||
			m_StencilRef != _rState.m_StencilRef ||
			m_StencilMask != _rState.m_StencilMask)
		{
			Mode_StencilFunc(_rState.m_StencilTest, _rState.m_StencilRef, _rState.m_StencilMask);
		}
		
		if(m_PrimaryColor != _rState.m_PrimaryColor)
			Mode_PrimaryColor(_rState.m_PrimaryColor.x, _rState.m_PrimaryColor.y, _rState.m_PrimaryColor.z, _rState.m_PrimaryColor.w);

		// pointers
		if(m_iVertexStream != _rState.m_iVertexStream)
			Stream_Vertex(_rState.m_iVertexStream);

		if(m_iIndexStream != _rState.m_iIndexStream)
			Stream_Index(_rState.m_iIndexStream);

		if(m_iNormalStream != _rState.m_iNormalStream)
			Stream_Normal(_rState.m_iNormalStream);

		if(m_iColorStream != _rState.m_iColorStream)
			Stream_Color(_rState.m_iColorStream);
		
		// textures and texture pointers
		for(intr i = 0; i < EMaxTextureUnits; i++)
		{
			if(	m_aiTextureStreamVB[i] != _rState.m_aiTextureStreamVB[i] ||
				m_aiTextureStreamVBIndex[i] != _rState.m_aiTextureStreamVBIndex[i])
				Stream_Texture(i, _rState.m_aiTextureStreamVB[i], _rState.m_aiTextureStreamVBIndex[i]);

			if(m_iaTexture[i] != _rState.m_iaTexture[i])
				Texture_Set(i, _rState.m_iaTexture[i]);
		}

		// shaders
		if(m_iVertexShader != _rState.m_iVertexShader)
			Shader_Vertex(_rState.m_iVertexShader);

		if(m_iPixelShader != _rState.m_iPixelShader)
			Shader_Pixel(_rState.m_iPixelShader);
	}

	// matrics
	Matrix_Model(_rState.m_ModelMatrix);
	Matrix_Projection(_rState.m_ProjectionMatrix);
}

//
//
//
void CState::State_SetDefaults(bool _Force)
{
	if(_Force)
	{
		// flags
		Flag_Set(EFlag_Blend, false);
		Flag_Set(EFlag_Cull, true);
		//Flag_Set(EFlag_ColorWrite, true);
		Flag_Set(EFlag_DepthTest, true);
		Flag_Set(EFlag_DepthWrite, true);
		Flag_Set(EFlag_StencilTest, false);
		Flag_Set(EFlag_StencilWrite, false);
		Flag_Set(EFlag_ScissorTest, false);

		// mode
		Mode_BlendFunc(EBlend_SrcColor, EBlend_Zero);
		Mode_DepthTest(ETest_Less);
		Mode_Cull(ECull_CW);
		Mode_StencilFunc(ETest_Equal, 0, 0xFF);
		Mode_StencilOp(EStencilOp_Keep, EStencilOp_Keep, EStencilOp_Keep);
		Mode_PrimaryColor(1,1,1,1);
		Mode_WriteMask(true,true,true,true);

		// streams
		Stream_Clear();

		for(intr i = 0; i < EMaxTextureUnits; i++)
			Texture_Set(i, -1);

		// shaders
		Shader_Vertex(-1);
		Shader_Pixel(-1);

		// matrics
		TMatrix4<fp32> Matrix;
		Matrix.Unit();
		Matrix_Model(Matrix);
		Matrix_Projection(Matrix);
	}
	else
	{
		CState State;
		State.State_SetDefaults(true);
		State_Set(State);
	}
}

// ------------------------------------------------------------------------------------------

bool CMode::SParameter::Accaptable(SParameter _Param) const
{
	if(m_Type == EType_Unspecified)
		return true;
	else if(m_Type == EType_Enable && _Param.m_Value > 0)
		return true;
	else if(m_Type == EType_Specified && m_Value == _Param.m_Value)
		return true;
	return false;
}

//
//
//
CMode::CMode()
{
	Clear();
}

//
//
//
CMode::CMode(int32_t _Width, int32_t _Height, bool _Fullscreen, int32_t _Flags)
{
	Clear();
	Set(EWidth, EType_Specified, _Width);
	Set(EHeight, EType_Specified, _Height);
	Set(EFullscreen, EType_Specified, _Fullscreen ? 1 : 0);
}

//
//
//
void CMode::Clear()
{
	for(intr i = 0; i < ENumParams; i++)
	{
		m_aParams[i].m_Type = EType_Unspecified;
		m_aParams[i].m_Value = 0;
	}

	Set(EAdapter, EType_Specified, EAdapter_Default);
}

//
//
//
bool CMode::Compare(CMode _Mode) const
{
	for(intr i = 0; i < ENumParams; i++)
	{
		if(	m_aParams[i].m_Type != _Mode.m_aParams[i].m_Type ||
			m_aParams[i].m_Value != _Mode.m_aParams[i].m_Value)
			return false;
	}

	return true;
}

//
//
//
bool CMode::Acceptable(CMode _Mode) const
{
	for(intr i = 0; i < ENumParams; i++)
	{
		if(!m_aParams[i].Accaptable(_Mode.m_aParams[i]))
			return false;
	}

	return true;
}

// ------------------------------------------------------------------------------------------

//
//
//
int32_t CIndexHandler::GetFree(int32_t _Type)
{
	if(m_lFreeNumbers.Len())
	{
		int32_t id = m_lFreeNumbers[0];
		m_lFreeNumbers.RemoveByIndexFast(0);
		m_lIDNumbers[id].m_Type = _Type;
		m_lIDNumbers[id].m_pData = NULL;
		return id;
	}

	SID id;
	id.m_Type = _Type;
	id.m_pData = NULL;
	return m_lIDNumbers.Add(id);
}

void CIndexHandler::Remove(int32_t _ID)
{
	if(_ID >= m_lIDNumbers.Len())
		return;

	m_lIDNumbers[_ID].m_Type = -1;
	m_lIDNumbers[_ID].m_pData = NULL;
	m_lFreeNumbers.Add(_ID);
}

// ------------------------------------------------------------------------------------------

//
//
//
bool CDevice::GetBestMode(CMode _Mode, CMode &_rBestMode)
{
	const int32_t aPriority[] = {	CMode::ERefreshRate,
								CMode::EColorBits,
								CMode::EDepthBits,
								CMode::EWidth,
								CMode::EHeight,
								CMode::EFullscreen,
								CMode::EAlphaBits,
								CMode::EStencilBits};

	TDynamicArray<N3D::CMode> lModes;
	EnumDisplayModes(lModes, _Mode);

	if(!lModes.Len())
		return NULL;

	//
	int32_t Disables = 0;
	for(intr i = 0; i < lModes.Len(); i++)
	{
		int32_t thisone = 0;
		for(int32_t p = 0; p < CMode::ENumParams; p++)
			if(!lModes[i].m_aParams[p].m_Value)
				thisone++;

		if(thisone > Disables)
			Disables = thisone;
	}

	for(intr i = 0; i < lModes.Len(); i++)
	{
		int32_t thisone = 0;
		for(int32_t p = 0; p < CMode::ENumParams; p++)
			if(!lModes[i].m_aParams[p].m_Value)
				thisone++;

		if(thisone > Disables)
		{
			lModes.RemoveByIndexFast(i);
			i--;
		}
	}

	//
	for(intr p = 0; p < sizeof(aPriority)/sizeof(int32_t); p++)
	{
		int32_t Highest = -1;
		for(intr i = 0; i < lModes.Len(); i++)
		{
			if(lModes[i].Get(aPriority[p]) > Highest)
				Highest = lModes[i].Get(aPriority[p]);
		}

		for(intr i = 0; i < lModes.Len(); i++)
			if(lModes[i].Get(aPriority[p]) < Highest)
			{
				lModes.RemoveByIndexFast(i);
				i--;
			}
	}

	//
	/*
	for(intr i = 0; i < lModes.Len(); i++)
	{
		for(intr p = 0; p < N3D::CMode::ENumParams; p++)
			NLog::WriteF("%4d ", lModes[i].Get(p));
		NLog::WriteF("\n");
	}*/

	// this should never happen
	if(!lModes.Len())
		return false;
	
	_rBestMode = lModes[0];
	
	NLog::WriteF("3ddevice: Best mode: ");
	for(intr p = 0; p < N3D::CMode::ENumParams; p++)
		NLog::WriteF("%4d ", _rBestMode.Get(p));
	NLog::WriteF("\n");

	return true;
}

//
//
//
int32_t CDevice::CheckMode(CMode _Params)
{
	// IMPLEMENT ME
	return true;
}


//
//
//
int32_t CDevice::CreateIndexBuffer(CIndexBuffer *_pIB, int32_t _Count)
{
	if(_Count < 0)
		_Count = _pIB->m_Count;

	int32_t iIndexBuffer = CreateIndexBuffer(_pIB->m_Count, 0);
	CIndexBuffer *pIB = LockIndexBuffer(iIndexBuffer);
	MemoryCopy(pIB->m_pIndex16, _pIB->m_pIndex16, _pIB->m_Count*sizeof(uint16));
	UnlockIndexBuffer(iIndexBuffer);
	return iIndexBuffer;
}

//
//
//
int32_t CDevice::CreateVertexBuffer(CVertexBuffer *_pVB, int32_t _Count, int32_t _Mask)
{
	if(_Count < 0)
		_Count = _pVB->m_Count;

	int32_t What = _pVB->What()&_Mask;
	int32_t iVertexBuffer = CreateVertexBuffer(_Count, What, 0);
	CVertexBuffer *pVB = LockVertexBuffer(iVertexBuffer);

	for(int32_t i = 0; i < _pVB->GetNumPointers(); i++, What>>=1)
	{
		if(What&1 && _pVB->GetPointer(i))
			MemoryCopy(pVB->GetPointer(i), _pVB->GetPointer(i), _Count*_pVB->GetPointerElementSize(i));
	}

	UnlockVertexBuffer(iVertexBuffer);
	return iVertexBuffer;
}


//
//
//
void CDevice::ClearAllAllocatedIndexes()
{
	for(int32_t i = 0; i < m_IndexHandler.m_lIDNumbers.Len(); i++)
	{
		switch(m_IndexHandler.m_lIDNumbers[i].m_Type)
		{
		case EID_Texture: DestroyTexture(i); break;
		case EID_VertexShader: DestroyVertexShader(i); break;
		case EID_PixelShader: DestroyPixelShader(i); break;
		case EID_VertexBuffer: DestroyVertexBuffer(i); break;
		case EID_IndexBuffer: DestroyIndexBuffer(i); break;
		}
	}
}

//
//
//
/*
int8 *LoadStorageAsString(CStorage &_rStorage)
{
	int32_t Size = _rStorage.GetSize();
	int8 *pString = new int8[Size+1];
	_rStorage.Read(pString, Size);
	pString[Size] = 0;
	return pString;
}
*/
//
//
//
CString CDevice::LoadShader(CInputStream &_rStream)
{
	CString File;

	// read file
	{
		int32_t Size = _rStream.GetSize();
		File.SetSize(Size+1);
		_rStream.Read(File.GetData(), Size);
		File.GetData()[Size] = 0;
	}
		
	CString Shader;
	CString Version;
	int32_t Mode = 0; // 0 = base, 1 = in shader
	
	while(File.Len())
	{
		CString Line = File.GetStrSep("\n");

		if(Mode)
		{
			if(Line.GetData()[0] == '}')
			{
				Mode = 0;
				for(int32_t i = 0; i < m_lShaderVersion.Len(); i++)
					if(m_lShaderVersion[i] == Version)
						return Shader;
			}
			else
			{
				Line.TrimWhitespaces();
				Shader += Line;
				Shader += "\n";
			}
		}
		else
		{
			if(Line.GetData()[0] == '{')
			{
				Mode = 1;
				Shader = "";
			}
			else
			{
				Version = Line;
			}
			
		}
	}

	return CString("");
}




//
//
//
int32_t CVertexBuffer::What()
{
	int32_t Content = 0;
	Content |= m_pVertexfp32 ? EVB_Vertex : 0;
	Content |= m_pNormalfp32 ? EVB_Normal : 0;
	Content |= m_pColorfp32 ? EVB_Color : 0;

	for(int32_t i = 0; i < EMaxTextureUnits; i++)
		Content |= m_pTexturefp32[i] ? (EVB_TextureBase<<i) : 0;

	return Content;
}

//
//
//
void *CVertexBuffer::GetPointer(int32_t i)
{
	switch(i)
	{
		case 0: return m_pVertexfp32;
		case 1: return m_pNormalfp32;
		case 2: return m_pColorfp32;
		case 3: return NULL; // reserved
		case 4: return m_pTexturefp32[0];
		case 5: return m_pTexturefp32[1];
		case 6: return m_pTexturefp32[2];
		case 7: return m_pTexturefp32[3];
		case 8: return m_pTexturefp32[4];
		case 9: return m_pTexturefp32[5];
		case 10: return m_pTexturefp32[6];
		case 11: return m_pTexturefp32[7];
	}
	return NULL;
}

//
//
//
int32_t CVertexBuffer::GetPointerElementSize(int32_t i)
{
	switch(i)
	{
		case 0: return sizeof(TVector3<fp32>);
		case 1: return sizeof(TVector3<fp32>);
		case 2: return sizeof(TVector4<fp32>);
	}
	return sizeof(TVector2<fp32>);
}

//
//
//
class CVertexBufferSW : public CVertexBuffer
{
public:
	CVertexBufferSW()
	{
		m_pVertexfp32 = NULL;
		m_pNormalfp32 = NULL;
		m_pColorfp32 = NULL;
		for(int32_t i = 0; i < EMaxTextureUnits; i++)
			m_pTexturefp32[i] = NULL;
		m_Count = 0;
	}
	~CVertexBufferSW()
	{
		SafeArrayDelete(m_pVertexfp32);
		SafeArrayDelete(m_pNormalfp32);
		SafeArrayDelete(m_pColorfp32);
		for(int32_t i = 0; i < EMaxTextureUnits; i++)
			SafeArrayDelete(m_pTexturefp32[i]);
	}
};

//
//
//
class CIndexBufferSW : public CIndexBuffer
{
public:
	CIndexBufferSW()
	{
		m_Count = 0;
		m_pIndex16 = NULL;
	}
	~CIndexBufferSW()
	{
		SafeArrayDelete(m_pIndex16);
	}
};

//
//
//
CVertexBuffer *N3D::CreateVertexBufferSW(int32_t _Count, int32_t _What, int32_t _Flags)
{
	CVertexBufferSW *pBuffer = new CVertexBufferSW;
	pBuffer->m_Count = _Count;

	for(int32_t i = 0; i < EMaxTextureUnits; i++)
	{
		if(_What&(EVB_TextureBase<<i))
			pBuffer->m_pTexturefp32[i] = new TVector2<fp32>[_Count];
		else
			pBuffer->m_pTexturefp32[i] = NULL;
	}

	pBuffer->m_pVertexfp32 = NULL;
	pBuffer->m_pNormalfp32 = NULL;
	pBuffer->m_pColorfp32 = NULL;
	if(_What&EVB_Vertex) pBuffer->m_pVertexfp32 = new TVector3<fp32>[_Count];
	if(_What&EVB_Normal) pBuffer->m_pNormalfp32 = new TVector3<fp32>[_Count];
	if(_What&EVB_Color) pBuffer->m_pColorfp32 = new TVector4<fp32>[_Count];
	return pBuffer;
}

//
//
//
N3D::CIndexBuffer *N3D::CreateIndexBufferSW(int32_t _Count, int32_t _Flags)
{
	CIndexBuffer *pIB = new CIndexBufferSW;
	pIB->m_Count = _Count;
	pIB->m_pIndex16 = new uint16[_Count];
	return pIB;
}

//
//
//
TMatrix4<fp32> NMatrix::Unit()
{
	TMatrix4<fp32> mat;
	mat.Unit();
	return mat;
}

TMatrix4<fp32> NMatrix::Scale(fp32 x, fp32 y, fp32 z)
{
	TMatrix4<fp32> Matrix;
	Matrix.Unit();
	Matrix.k[0][0] = x;
	Matrix.k[1][1] = y;
	Matrix.k[2][2] = z;
	return Matrix;
}

TMatrix4<fp32> NMatrix::Rotation(fp32 _X, fp32 _Y, fp32 _Z)
{
	TMatrix4<fp32> X, Y, Z;
	X.RotX(_X);
	Y.RotY(_Y);
	Z.RotZ(_Z);
	TMatrix4<fp32> Temp = X*Y;
	TMatrix4<fp32> Res = Temp*Z;
	return Res;
}

TMatrix4<fp32> NMatrix::Projection(fp32 _Near, fp32 _Far, fp32 _Right, fp32 _Left, fp32 _Top, fp32 _Bottom)
{
	TMatrix4<fp32> Matrix;
	Matrix.Unit();
	Matrix.k[0][0] = (2*_Near)/(_Right-_Left);
	Matrix.k[1][1] = (2*_Near)/(_Top-_Bottom);
	Matrix.k[2][3] = -1;
	Matrix.k[2][0] = (_Right+_Left)/(_Right-_Left);
	Matrix.k[2][1] = (_Top+_Bottom)/(_Top-_Bottom);
	Matrix.k[2][2] = (_Near+_Far)/(_Near-_Far);
	Matrix.k[3][2] = (2*_Far*_Near)/(_Near-_Far);

	return Matrix;
}


TMatrix4<fp32> NMatrix::Ortho(fp32 _Left, fp32 _Right, fp32 _Top, fp32 _Bottom, fp32 _Far, fp32 _Near)
{
	TMatrix4<fp32> Matrix;
	Matrix.Unit();

	Matrix.k[0][0] = 2/(_Right-_Left);
	Matrix.k[1][1] = 2/(_Top-_Bottom);
	Matrix.k[2][2] = -2/(_Far-_Near);
	Matrix.k[3][3] = 1;

	Matrix.k[3][0] = -((_Right+_Left)/(_Right-_Left));
	Matrix.k[3][1] = -((_Top+_Bottom)/(_Top-_Bottom));
	Matrix.k[3][2] = -((_Far+_Near)/(_Far-_Near));

	return Matrix;
}


TMatrix4<fp32> NMatrix::Translation(fp32 x, fp32 y, fp32 z)
{
	TMatrix4<fp32> Matrix;
	Matrix.Unit();
	Matrix.k[3][0] = x;
	Matrix.k[3][1] = y;
	Matrix.k[3][2] = z;
	return Matrix;
}


TMatrix4<fp32> NMatrix::LookAt(TVector3<fp32> _At, TVector3<fp32> _Eye, TVector3<fp32> _Up)
{
	TVector3<fp32> f = (_At - _Eye);
	f.Normalize();
	TVector3<fp32> Up = _Up;
	Up.Normalize();

	TVector3<fp32> s = f.CrossProduct(Up);
	TVector3<fp32> u = s.CrossProduct(f);
	TMatrix4<fp32> Out;

	Out.k[0][0] = s.x;
	Out.k[1][0] = s.y;
	Out.k[2][0] = s.z;
	Out.k[3][0] = 0.0f;

	Out.k[0][1] = u.x;
	Out.k[1][1] = u.y;
	Out.k[2][1] = u.z;
	Out.k[3][1] = 0.0f;

	Out.k[0][2] =-f.x;
	Out.k[1][2] =-f.y;
	Out.k[2][2] =-f.z;
	Out.k[3][2] = 0.0f;

	Out.k[0][3] = 0;
	Out.k[1][3] = 0;
	Out.k[2][3] = 0;
	Out.k[3][3] = 1.0f;

	return Out* N3D::NMatrix::Translation(-_Eye.x,-_Eye.y,-_Eye.z);
}

//////////////////////////////////////////////////////////////////////////
//////////////////  CShaderManager  //////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



/*
CShaderManager::CShaderManager()
{
	m_pDevice = NULL;
	m_MaxVSVersion = 0;
	m_MaxPSVersion = 0;
	
}

CShaderManager::~CShaderManager()
{
	//	Go through each shader in indexhandler and delete them
	int32_t Len = m_IndexHandler.Len();
	for (int32_t i = 0; i < Len; i++)
	{
		CShaderEntry* pEntry = (CShaderEntry*)m_IndexHandler.GetSIDData(i);
		if (pEntry)
		{	
			delete pEntry;
		}
	}
}

void CShaderManager::Create(CDevice* _pDevice)
{
	m_pDevice = _pDevice;
	FindMaxVertexShaderVersion();
	FindMaxPixelShaderVersion();
}

int32_t CShaderManager::CreateVertexShader(const NFoo::CString& _Name)
{
	CFile ShaderFile;
	int32_t VersionReceived = OpenVertexShader(m_MaxVSVersion,_Name,ShaderFile);
	if (VersionReceived != 0)
	{
		if(!ShaderFile.IsReady())
			return false;

		int8* pFileAsString = new int8[ShaderFile.GetSize()+1];
		ShaderFile.Read(pFileAsString, ShaderFile.GetSize());
		pFileAsString[ShaderFile.GetSize()] = 0;
		ShaderFile.Close();
		int32_t iVertexShader = m_pDevice->CreateVertexShader(pFileAsString);
		delete pFileAsString;

		if (iVertexShader != -1)
		{
			// Add it to the index handler
			CShaderEntry *pShader = new CShaderEntry(_Name,VersionReceived,iVertexShader);
			int32_t iShader = m_IndexHandler.GetFree(EID_SMVertexShader);
			m_IndexHandler.SetData(iShader, pShader);
			return iShader;
		}
	}
	return -1;
}

int32_t CShaderManager::CreatePixelShader(const NFoo::CString& _Name)
{
	CFile ShaderFile;
	int32_t VersionReceived = OpenPixelShader(m_MaxPSVersion,_Name,ShaderFile);
	if (VersionReceived != 0)
	{
		if(!ShaderFile.IsReady())
			return false;

		int8* pFileAsString = new int8[ShaderFile.GetSize()+1];
		ShaderFile.Read(pFileAsString, ShaderFile.GetSize());
		pFileAsString[ShaderFile.GetSize()] = 0;
		ShaderFile.Close();
		int32_t iPixelShader = m_pDevice->CreatePixelShader(pFileAsString);
		delete pFileAsString;

		if (iPixelShader != -1)
		{
			// Add it to the index handler
			CShaderEntry *pShader = new CShaderEntry(_Name,VersionReceived,iPixelShader);
			int32_t iShader = m_IndexHandler.GetFree(EID_SMPixelShader);
			m_IndexHandler.SetData(iShader, pShader);
			return iShader;
		}
	}
	return -1;
}

int32_t CShaderManager::GetShader(int32_t _iShader)
{
	CShaderEntry* pEntry = m_IndexHandler.GetObject<CShaderEntry*>(_iShader,EID_SMVertexShader);
	if (pEntry)
	{
		return pEntry->GetShader();
	}
	else
	{
		pEntry = m_IndexHandler.GetObject<CShaderEntry*>(_iShader,EID_SMPixelShader);
		return (pEntry ? pEntry->GetShader() : -1);
	}
}

void CShaderManager::DestroyShader(int32_t _iShader)
{
	CShaderEntry* pEntry = m_IndexHandler.GetObject<CShaderEntry*>(_iShader,EID_SMVertexShader);
	if (pEntry)
	{
		m_pDevice->DestroyVertexShader(pEntry->GetShader());
		m_IndexHandler.Remove(_iShader);
	}
	else
	{
		pEntry = m_IndexHandler.GetObject<CShaderEntry*>(_iShader,EID_SMPixelShader);
		if (pEntry)
		{
			m_pDevice->DestroyPixelShader(pEntry->GetShader());
			m_IndexHandler.Remove(_iShader);
		}
	}
}

bool CShaderManager::DirFromVSVersion(int32_t _VSVersion, NFoo::CString& _DirName)
{
	switch (_VSVersion)
	{
	case EShaderManager_VertexVersion_1_1:
		{
			_DirName += "vsh/1_1/";
			return true;
		}
	case EShaderManager_VertexVersion_2_0:
		{
			_DirName += "vsh/2_0/";
			return true;
		}
	case EShaderManager_VertexVersion_3_0:
		{
			_DirName += "vsh/3_0/";
			return true;
		}
	default:
		return false;
	}
}

bool CShaderManager::DirFromPSVersion(int32_t _PSVersion, NFoo::CString& _DirName)
{
	switch (_PSVersion)
	{
	case EShaderManager_PixelVersion_1_1:
		{
			_DirName += "psh/1_1/";
			return true;
		}
	case EShaderManager_PixelVersion_1_2:
		{
			_DirName += "psh/1_2/";
			return true;
		}
	case EShaderManager_PixelVersion_1_3:
		{
			_DirName += "psh/1_3/";
			return true;
		}
	case EShaderManager_PixelVersion_1_4:
		{
			_DirName += "psh/1_4/";
			return true;
		}
	case EShaderManager_PixelVersion_2_0:
		{
			_DirName += "psh/2_0/";
			return true;
		}
	case EShaderManager_PixelVersion_3_0:
		{
			_DirName += "psh/3_0/";
			return true;
		}
	default:
		return false;
	}
}

int32_t CShaderManager::OpenVertexShader(int32_t _VSVersion, const NFoo::CString& _Name, NFoo::CFile& _File)
{
	NFoo::CString BaseDir;
	m_pDevice->GetShaderBaseDir(BaseDir);
	int32_t Version = OpenVertexShader(_VSVersion,BaseDir,_Name,_File);
	if (Version == EShaderManager_VertexVersion_Undefined)
	{
		BaseDir = "shared/";
		Version = OpenVertexShader(_VSVersion,BaseDir,_Name,_File);
	}
	return Version;
}

int32_t CShaderManager::OpenVertexShader(int32_t _VSVersion, const NFoo::CString& _BaseDir, const NFoo::CString& _Name, NFoo::CFile& _File)
{
	for (int32_t Version = _VSVersion; Version >= EShaderManager_VertexVersion_1_1; Version--)
	{
		NFoo::CString FileName = _BaseDir;
		if (DirFromVSVersion(Version,FileName))
		{
			FileName += _Name;
			FileName += ".vsh";

			_File.Open(FileName);
			if (_File.IsReady())
				return Version;
		}
	}

	return EShaderManager_VertexVersion_Undefined;
}

int32_t CShaderManager::OpenPixelShader(int32_t _VSVersion, const NFoo::CString& _Name, NFoo::CFile& _File)
{
	// First try device specific then shared
	NFoo::CString BaseDir;
	m_pDevice->GetShaderBaseDir(BaseDir);
	int32_t Version = OpenPixelShader(_VSVersion,BaseDir,_Name,_File);
	if (Version == EShaderManager_VertexVersion_Undefined)
	{
		BaseDir = "shared/";
		Version = OpenPixelShader(_VSVersion,BaseDir,_Name,_File);
	}
	return Version;
}

int32_t CShaderManager::OpenPixelShader(int32_t _PSVersion, const NFoo::CString& _BaseDir, const NFoo::CString& _Name, NFoo::CFile& _File)
{
	for (int32_t Version = _PSVersion; Version >= EShaderManager_PixelVersion_1_1; Version--)
	{
		NFoo::CString FileName = _BaseDir;
		if (DirFromPSVersion(Version,FileName))
		{
			FileName += _Name;
			FileName += ".psh";

			_File.Open(FileName);
			if (_File.IsReady())
				return Version;
		}
	}
	return EShaderManager_PixelVersion_Undefined;
}
#define VS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))
#define PS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))

int32_t CShaderManager::FindMaxVertexShaderVersion()
{
	int32_t VSCaps = m_pDevice->GetVertexShaderCapabilities();

	int32_t Max = 0;
	if (VSCaps >= VS_VERSION(3,0))
		Max = EShaderManager_VertexVersion_3_0;
	else if (VSCaps >= VS_VERSION(2,0))
		Max = EShaderManager_VertexVersion_2_0;
	else if (VSCaps >= VS_VERSION(1,1))
		Max = EShaderManager_VertexVersion_1_1;

	m_MaxVSVersion = Max;
	return m_MaxVSVersion;
}

int32_t CShaderManager::FindMaxPixelShaderVersion()
{
	int32_t PSCaps = m_pDevice->GetPixelShaderCapabilities();

	int32_t Max = 0;
	if (PSCaps >= PS_VERSION(3,0))
		Max = EShaderManager_PixelVersion_3_0;
	else if (PSCaps >= PS_VERSION(2,0))
		Max = EShaderManager_PixelVersion_2_0;
	else if (PSCaps >= PS_VERSION(1,4))
		Max = EShaderManager_PixelVersion_1_4;
	else if (PSCaps >= PS_VERSION(1,3))
		Max = EShaderManager_PixelVersion_1_3;
	else if (PSCaps >= PS_VERSION(1,2))
		Max = EShaderManager_PixelVersion_1_2;
	else if (PSCaps >= PS_VERSION(1,1))
		Max = EShaderManager_PixelVersion_1_1;

	m_MaxPSVersion = Max;
	return m_MaxPSVersion;
}
*/
