#include <foo/base/log.h>
#include "networkauthclient.h"

using namespace NEngine;

MACRO_REGISTER_CLASS(CNetworkAuthClient, "EngineModule")

//
//
//
bool CNetworkAuthClient::Init()
{
	Assert(CModule::Init());
	m_pCore->RegisterCommand("auth", this);
	m_pCore->RegisterCommand("newuser", this);
	m_pCore->RegisterCommand("r", this); // remote
	m_pConnection = NULL;
	return true;
}

//
//
//
void CNetworkAuthClient::OnAttach(CNetworkConnection *pConnection)
{
	m_pConnection = pConnection;
	pConnection->RegisterNetMessage(NETMSG("clAuthChallange"), this);
	pConnection->RegisterNetMessage(NETMSG("clAuthResult"), this);
}

//
//
//
void CNetworkAuthClient::OnDetach(CNetworkConnection *pConnection)
{
	m_pConnection = NULL;
}

//
//
//
void CNetworkAuthClient::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(iMsg == NETMSG("clAuthResult"))
	{
		int32_t Result = rStream.ReadBE<uint8_t>();
		m_ErrorMessage = rStream.Read_p8();
		m_LoggedOn = Result;
		if(Result)
			Signal(EEvent_Success);
		else
			Signal(EEvent_Failed);
	}
}

//
//
//
void CNetworkAuthClient::OnCommand(CString &rCmd, CString &rArgs)
{
	if(rCmd == "r")
	{
		if(!m_pConnection)
			return;

		CNetMessage Msg(NETMSG("svExecuteCommand"), true);
		Msg.Write_p16(rArgs);
		m_pConnection->SendMessage(Msg);

	}
	else if(rCmd == "auth")
	{
		CString Username = rArgs.GetStrSep(" ");
		Logon(Username, rArgs);
	}
	else if(rCmd == "newuser")
	{
		if(!m_pConnection)
			return;

		CString username = rArgs.GetStrSep(" ");
		CString password = rArgs.GetStrSep(" ");
		int32_t level = rArgs.GetStrSep(" ").GetInt();
		CNetMessage Msg(NETMSG("svNewUser"), true);
		Msg.Write_p8(username);
		Msg.Write_p8(password);
		Msg.WriteBE<int32_t>(level);
		m_pConnection->SendMessage(Msg);
	}
}

//
//
//
void CNetworkAuthClient::Logon(const char *pUsername, const char *pPassword)
{
	if(!m_pConnection)
		return;

	m_Username = pUsername;
	m_Password = pPassword;

	CNetMessage Msg(NETMSG("svAuthLogon"), true);
	Msg.WriteBE<uint8_t>(0);
	Msg.Write_p8(m_Username);
	Msg.Write_p8(m_Password);
	m_pConnection->SendMessage(Msg);
}
