
#include "instance.h"
#include "device.h"

using namespace NFoo;
using namespace NFoo::NSound;

CInstance::CInstance(CSource *pSource, CSoundDevice *pDevice)
{
	m_pSource = pSource;
	m_pDevice = pDevice;
	m_Start = -1;
	//m_StartTime = NFoo::GetTickCount();
	m_Flags = 0;
	m_Volume = 1;
	m_Rate = 1;
}

CInstance::~CInstance()
{
	if(m_pPuller)
		m_pPuller->m_pInstance = NULL;
}

void CInstance::OnEnd()
{
	if(m_Flags&ESoundFlag_AutoDestroy)
		m_pDevice->Destroy(m_iSound);
}