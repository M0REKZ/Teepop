#include <foo/func/string.h>
#include <foo/base/string.h>
#include "engine.h"

using namespace NEngine;

CCore::~CCore()
{
	while(m_lModules.Len())
	{
		delete m_lModules[m_lModules.Len()-1];
		m_lModules.RemoveByIndexFast(m_lModules.Len()-1);
	}
	
}

//# Function: Update
//# Description: Updates all modules.
//# Declaration:
bool CCore::Update()
{
	for(int32 i = 0; i < m_lModules.Len(); i++)
		m_lModules[i]->Update();
	return true;
}

bool CCore::AddModule(CModule *pModule)
{
	Assert(pModule);
	m_lModules.Add(pModule);
	return true;
}

//# Function: LoadModule
//# Description: Loads a module and adds it to the core.
//# Parameters:
//#		pName, Name of the module to add.
//# Returns: Returns a pointer to the module.
//# Declaration:
CModule *CCore::LoadModule(const int8 *pName)
{
	Assert(pName);
	CModule *pModule = (CModule *)NClassRegistry::CreateByName(CString("C%s", pName));
	Assert(pModule);
	pModule->m_pCore = this;
	Assert(pModule->Init());
	Assert(AddModule(pModule));
	return pModule;
}

//# Function: GetModule
//# Description: Retrives a pointer to a loaded module.
//# Parameters:
//#		pName, Name of the module to retrive.
//# Returns: Returns a pointer to the module.
//# Declaration:
CModule *CCore::GetModule(const int8 *pName)
{
	for(int32 i = 0; i < m_lModules.Len(); i++)
	{
		if(StringCompare(pName, m_lModules[i]->m_pClassHandle->m_aName+1) == 0)
			return m_lModules[i];
	}
	return NULL;
}

void CCore::RegisterCommand(const int8 *pCmd, CConsoleClient *pConsoleClient)
{
	m_mCommandMap[pCmd] = pConsoleClient;
	
}

bool CCore::DispatchCommand(const int8 *pCmdLine)
{
	CString Args = pCmdLine;
	CString Cmd = Args.GetStrSep(" ");

	CConsoleClient *pConsoleClient = m_mCommandMap.Find(Cmd, NULL);
	if(!pConsoleClient)
		return false;
	pConsoleClient->OnCommand(Cmd, Args);
	return true;
}

