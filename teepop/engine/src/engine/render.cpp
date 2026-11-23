#include <foo/ext/3d/3d.h>
#include <foo/ext/3d/2dutils/2dutils.h>
#include <foo/ext/sound/device.h>
#include "render.h"

using namespace NEngine;
using namespace NFoo;
//using namespace NFoo::N3D;

bool CRender::Init()
{
	CModule::Init();

	m_pDevice = (N3D::CDevice*)NClassRegistry::CreateByTag(N3D::aDeviceClassTag);
	Assert(m_pDevice);
	Assert(m_pUtils = new N3D::C2DUtils);
	return true;
}

CRender::~CRender()
{
	m_pUtils->Shutdown();
	SafeDelete(m_pUtils);
	SafeDelete(m_pDevice);
}
MACRO_REGISTER_CLASS(CRender, "EngineModule")


//
//
//
CSound::~CSound()
{
	if(m_pDevice)
	{
		m_pDevice->Shutdown();
		SafeDelete(m_pDevice);
	}
}

bool CSound::Init()
{
	CModule::Init();

	m_pDevice = (NSound::CSoundDevice*)NClassRegistry::CreateByTag(NSound::aDeviceClassTag);
	return true;
}


MACRO_REGISTER_CLASS(CSound, "EngineModule")