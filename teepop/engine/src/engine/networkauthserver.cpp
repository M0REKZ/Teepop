#include <foo/base/log.h>
#include "networkauthserver.h"

using namespace NEngine;

MACRO_REGISTER_CLASS(CNetworkAuthServer, "EngineModule")

//
//
//
CNetworkAuthServer::~CNetworkAuthServer()
{
	m_lUsers.DeleteAll();
}

//
//
//
bool CNetworkAuthServer::Init()
{
	m_AllowMultipleLogons = false;
	m_AllowGuests = true;
	Assert(CModule::Init());
	return true;
}

//
//
//
void CNetworkAuthServer::OnAttach(CNetworkConnection *pConnection)
{
	pConnection->RegisterNetMessage("svAuthLogon", this);
	pConnection->RegisterNetMessage("svNewUser", this);
	pConnection->RegisterNetMessage("svAuthChallangeResponse", this);
	pConnection->RegisterNetMessage("svExecuteCommand", this);
}

//
//
//
void CNetworkAuthServer::OnDetach(CNetworkConnection *pConnection)
{
	CUser *pUser = pConnection->GetData<CUser>(this);
	if(pUser)
	{
		pUser->m_LoggedOns--;
		if(pUser->m_GuestAccount && pUser->m_LoggedOns == 0)
		{
			// delete account
			NLog::WriteF("AuthServer: Removing guest account '%s'\n", pUser->m_Name.GetData());
			m_lUsers.Remove(pUser);
			delete pUser;
		}
	}
}

//
//
//
CNetworkAuthServer::CUser *CNetworkAuthServer::GetUser(const int8 *pName)
{
	for(int32_t i = 0; i < m_lUsers.Len(); i++)
		if(m_lUsers[i]->m_Name == pName)
			return m_lUsers[i];
	return NULL;
}

//
//
//
int32_t CNetworkAuthServer::GetAuthLevel(CNetworkConnection *pConnection)
{
	CUser *pUser = pConnection->GetData<CUser>(this);
	if(!pUser)
		return -1;
	return pUser->m_UserLevel;
}

//
//
//
bool CNetworkAuthServer::NewUser(CString Name, CString Password, int32_t Level, bool Guest)
{
	NLog::WriteF("AuthServer: Creating new user, name=%s pw=%s level=%d guest=%d\n",
						Name.GetData(), Password.GetData(), Level, Guest);
	if(GetUser(Name))
		return false;
	if(Name.Len() < 1)
		return false;
	if(Password.Len() < 1 && !Guest)
		return false;
	for(int32_t i = 0; i < Name.Len(); i++)
		if((Name[i] >= 'a' && Name[i] <= 'z') || (Name[i] >= 'A' && Name[i] <= 'Z') ||
			(Name[i] >= '0' && Name[i] <= '9') || Name[i] == '_')
		{
		}
		else
			return false;
	m_lUsers.Add(new CUser(Name, Password, Level, Guest));
	return true;
}

//
//
//
void CNetworkAuthServer::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(iMsg == NETMSG("svAuthLogon"))
	{
		uint8 Version = rStream.ReadBE<uint8>();
		if(Version == 0)
		{
			CString Username = rStream.Read_p8();
			CString Password = rStream.Read_p8();
			CNetMessage Msg(NETMSG("clAuthResult"), true);

			CUser *pUser = GetUser(Username);
			bool IsGuest = (pUser == NULL || pUser->m_GuestAccount);
			bool IsPassCorrect = (pUser && pUser->m_Password == Password);
			bool IsDuplicateLogon = (pUser && pUser->m_LoggedOns);

			bool Logon = false;

			if(IsDuplicateLogon && !m_AllowMultipleLogons)
			{
				NLog::WriteF("AuthServer: Failed, Multiple logons, %s\n", Username.GetData());
				Msg.WriteBE<uint8>(0);
				Msg.Write_p8("You are already logged on\n");

				CMessage Msg(EEvent_AuthFailed);
				Msg.SetInt("client", pConnection->ClientID());
				Signal(Msg);
			}
			else if((IsGuest && m_AllowGuests))
			{
				// Guest logon
				if(!pUser)
				{
					if(NewUser(Username, "", 1, true))
					{
						pUser = GetUser(Username);
						Logon = true;
					}
					else
					{
						// account creation failed
						NLog::WriteF("AuthServer: Failed, Couldn't create guest account, %s\n", Username.GetData());
						Msg.WriteBE<uint8>(0);
						Msg.Write_p8("Couldn't create guest account\n");

						CMessage Msg(EEvent_AuthFailed);
						Msg.SetInt("client", pConnection->ClientID());
						Signal(Msg);
					}
				}
			
			}
			else if(IsPassCorrect)
			{
				// Registered logon
				Logon = true;
			}
			else
			{
				NLog::WriteF("AuthServer: Auth Failed, %s, Invalid username and/or passwords\n", Username.GetData());
				Msg.WriteBE<uint8>(0);
				Msg.Write_p8("Invalid username and/or password");

				CMessage Msg(EEvent_AuthFailed);
				Msg.SetInt("client", pConnection->ClientID());
				Signal(Msg);
			}

			if(Logon)
			{
				if(pUser->m_UserLevel > 0)
				{
					NLog::WriteF("AuthServer: Auth successful, %s\n", Username.GetData());
					Msg.WriteBE<uint8>(1);
					Msg.Write_p8("Success");
					pConnection->SetData(this, pUser);
					//m_lAuths[pConnection] = pUser;
					pUser->m_LoggedOns++;
					
					CMessage Msg(EEvent_AuthSuccess);
					Msg.SetInt("client", pConnection->ClientID());
					Msg.SetString("username", Username);
					Msg.SetInt("guest", pUser->m_GuestAccount);
					Msg.SetInt("level", pUser->m_UserLevel);
					Signal(Msg);
				}
				else
				{
					NLog::WriteF("AuthServer: Failed, access level to low, %s\n", Username.GetData());
					Msg.WriteBE<uint8>(0);
					Msg.Write_p8("You are not allowed to logon");

					CMessage Msg(EEvent_AuthFailed);
					Msg.SetInt("client", pConnection->ClientID());
					Signal(Msg);
				}
			}
			pConnection->SendMessage(Msg);
		}
		else
			Assert(0);
	}
	else if(iMsg == NETMSG("svExecuteCommand"))
	{
		if(GetAuthLevel(pConnection) >= 100)
		{
			CString cmd = rStream.Read_p16();
			m_pCore->DispatchCommand(cmd);
		}
	}
	else if(iMsg == NETMSG("svNewUser"))
	{
		if(GetAuthLevel(pConnection) >= 100)
		{
			CString name = rStream.Read_p8();
			CString pw = rStream.Read_p8();
			int32_t level = rStream.ReadBE<int32_t>();
			NewUser(name, pw, level, false);
		}
	}
}

//
//
//
void CNetworkAuthServer::OnCommand(CString &rCmd, CString &rArgs)
{
}
