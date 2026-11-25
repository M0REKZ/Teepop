#include <foo/base/log.h>
#include "networkchatclient.h"

using namespace NEngine;

MACRO_REGISTER_CLASS(CNetworkChatClient, "EngineModule")

//
//
//
bool CNetworkChatClient::Init()
{
	Assert(CModule::Init());
	m_pConnection = NULL;
	return true;
}

//
//
//
void CNetworkChatClient::OnAttach(CNetworkConnection *pConnection)
{
	pConnection->RegisterNetMessage(NETMSG("clChatMessage"), this);
	pConnection->RegisterNetMessage(NETMSG("clUserJoined"), this);
	pConnection->RegisterNetMessage(NETMSG("clUserLeft"), this);
	pConnection->RegisterNetMessage(NETMSG("clChannelSwitched"), this);
	m_pConnection = pConnection;
}

//
//
//
void CNetworkChatClient::OnDetach(CNetworkConnection *pConnection)
{
	m_pConnection = NULL;
}

//
//
//
void CNetworkChatClient::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	if(iMsg == NETMSG("clChatMessage"))
	{
		int32_t UserID = rStream.ReadBE<int32_t>();
		//CString user = rStream.Read_p16();
		CString message = rStream.Read_p16();

		CMessage Msg(EEvent_ChatMessage);
		if(UserID == -1)
			Msg.SetString("user", "\af8ffADMIN");
		else
			Msg.SetString("user", GetUserName(UserID));
		Msg.SetString("message", message);
		Signal(Msg);
	}
	else if(iMsg == NETMSG("clUserJoined"))
	{
		int32_t UserID = rStream.ReadBE<int32_t>();
		CString user = rStream.Read_p8();
		//if(!m_lUsers.Exist(user))
		m_lUsers[UserID] = user;
	}
	else if(iMsg == NETMSG("clUserLeft"))
	{
		int32_t UserID = rStream.ReadBE<int32_t>();
		//CString user = rStream.Read_p16();
		m_lUsers.Remove(UserID);
	}
	else if(iMsg == NETMSG("clChannelSwitched"))
	{
		m_lUsers.Clear();

		m_Channel = rStream.Read_p8();
		int32_t nUsers = rStream.ReadBE<uint16_t>();

		for(int32_t i = 0; i < nUsers; i++)
		{
			int32_t id = rStream.ReadBE<int32_t>();
			m_lUsers[id] = rStream.Read_p8();
		}

		Signal(EEvent_ChannelSwitched);

		NLog::WriteF("ChatClient: Switched channel to '%s'\n", m_Channel.GetData());
		for(int32_t i = 0; i < m_lUsers.Len(); i++)
			NLog::WriteF("\t%s\n", m_lUsers.GetValue(i).GetData());
	}
}

//
//
//
void CNetworkChatClient::OnCommand(CString &rCmd, CString &rArgs)
{
}

//
//
//
void CNetworkChatClient::SetChannel(const char *pChannel)
{
	if(!m_pConnection)
		return;

	CNetMessage Msg(NETMSG("svSetChannel"), true);
	Msg.Write_p8(pChannel);
	m_pConnection->SendMessage(Msg);
}

//
//
//
void CNetworkChatClient::Chat(const char *pMsg)
{
	if(!m_pConnection)
		return;

	CNetMessage Msg(NETMSG("svChatMessage"), true);
	Msg.Write_p16(pMsg);
	m_pConnection->SendMessage(Msg);
}

//
//
//
const char *CNetworkChatClient::GetUserName(int32_t id)
{
	int32_t pos = m_lUsers.Find(id);
	if(pos >= 0)
		return m_lUsers.GetValue(pos).GetData();
	return "(null)";
}

