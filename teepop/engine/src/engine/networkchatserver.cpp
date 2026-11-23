#include <foo/base/log.h>
#include "networkchatserver.h"

using namespace NEngine;

MACRO_REGISTER_CLASS(CNetworkChatServer, "EngineModule")

//
//
//
bool CNetworkChatServer::Init()
{
	Assert(CModule::Init());
	m_pCore->RegisterCommand("broadcast", this);
	return true;
}

//
//
//
void CNetworkChatServer::OnAttach(CNetworkConnection *pConnection)
{
	pConnection->RegisterNetMessage(NETMSG("svSetChannel"), this);
	pConnection->RegisterNetMessage(NETMSG("svChatMessage"), this);

	CUser *pUser = new CUser;
	pUser->m_pChannel = NULL;
	pUser->m_pConnection = pConnection;
	pConnection->SetData(this, pUser);
	//m_lUsers[pConnection] = pUser;
}

//
//
//
void CNetworkChatServer::OnDetach(CNetworkConnection *pConnection)
{
	DestroyUser(pConnection->GetData<CUser>(this));
}

//
//
//
void CNetworkChatServer::DestroyUser(CUser *pUser)
{
	if(!pUser)
		return;

	NLog::WriteF("ChatServer: user=%s destroyed\n", pUser->m_Name.GetData());
	if(pUser->m_pChannel)
		LeaveChannel(pUser);

	//m_lUsers.Remove(pUser->m_pConnection);
	delete pUser;
}

//
//
//
void CNetworkChatServer::SendMessageToChannel(CChannel *pChannel, CNetMessage &rMsg)
{
	if(!pChannel)
		return;

	for(int32 i = 0; i < pChannel->m_lMembers.Len(); i++)
		pChannel->m_lMembers[i]->m_pConnection->SendMessage(rMsg);
}


//
//
//
bool CNetworkChatServer::LeaveChannel(CUser *pUser)
{
	if(!pUser || !pUser->m_pChannel)
		return false;

	CChannel *pChannel = pUser->m_pChannel;
	pUser->m_pChannel->m_lMembers.RemoveFast(pUser);
	pUser->m_pChannel = NULL;

	if(pChannel->m_lMembers.Len() == 0)
	{
		// remove channel
		NLog::WriteF("ChatServer: '%s' destroyed. No more users\n", pChannel->m_Name.GetData());
		m_lChannels.RemoveFast(pChannel);
		delete pChannel;
	}
	else
	{
		// send user left
		CNetMessage Msg(NETMSG("clUserLeft"), true);
		Msg.WriteBE<int32>(pUser->m_pConnection->ClientID());
		SendMessageToChannel(pChannel, Msg);
	}
	

	return true;
}

//
//
//
bool CNetworkChatServer::SetChannel(CUser *pUser, const int8 *pName)
{
	CChannel *pChannel = FindChannel(pName);
	if(!pChannel)
	{
		pChannel = new CChannel;
		m_lChannels.Add(pChannel);
		pChannel->m_Name = pName;
	}
	return SetChannel(pUser, pChannel);
}

//
//
//
bool CNetworkChatServer::SetChannel(CUser *pUser, CChannel *pChannel)
{
	// leave current channel
	if(pUser->m_pChannel)
		LeaveChannel(pUser);

	pUser->m_pChannel = pChannel;
	pChannel->m_lMembers.Add(pUser);

	NLog::WriteF("ChatServer: user='%s' changed channel to '%s'\n", pUser->m_Name.GetData(), pChannel->m_Name.GetData());

	// send inital join message with all users
	{
		CNetMessage Msg(NETMSG("clChannelSwitched"), true);
		Msg.Write_p8(pChannel->m_Name);
		Msg.WriteBE<uint16>(pChannel->m_lMembers.Len());
		for(int32 i = 0; i < pChannel->m_lMembers.Len(); i++)
		{
			Msg.WriteBE<int32>(pChannel->m_lMembers[i]->m_pConnection->ClientID());
			Msg.Write_p8(pChannel->m_lMembers[i]->m_Name);
		}
		pUser->m_pConnection->SendMessage(Msg);
	}

	// send user joined
	{
		CNetMessage Msg(NETMSG("clUserJoined"), true);
		Msg.WriteBE<int32>(pUser->m_pConnection->ClientID());
		Msg.Write_p8(pUser->m_Name);
		SendMessageToChannel(pChannel, Msg);
	}

	return true;
}

//
//
//
CNetworkChatServer::CChannel *CNetworkChatServer::FindChannel(const int8 *pName)
{
	for(int32 i = 0; i < m_lChannels.Len(); i++)
	{
		if(m_lChannels[i]->m_Name == pName)
			return m_lChannels[i];
	}
	return NULL;
}

void CNetworkChatServer::Chat(CUser *pUser, const int8 *pMessage)
{
	if(!pUser || !pMessage)
		return;
	if(!pUser->m_pChannel)
		return;
	if(!StringLength(pMessage))
		return;

	CNetMessage Msg(NETMSG("clChatMessage"), true);
	Msg.WriteBE<int32>(pUser->m_pConnection->ClientID());
	Msg.Write_p16(pMessage);
	SendMessageToChannel(pUser->m_pChannel, Msg);

	NLog::WriteF("ChatServer (%s) %s: %s\n", pUser->m_pChannel->m_Name.GetData(), pUser->m_Name.GetData(), pMessage);
}


void CNetworkChatServer::Broadcast(const int8 *pMessage)
{
	if(!StringLength(pMessage))
		return;

	CNetMessage Msg(NETMSG("clChatMessage"), true);
	Msg.WriteBE<int32>(-1); // server
	Msg.Write_p16(pMessage);

	for(int32 i = 0; i < m_lChannels.Len(); i++)
		SendMessageToChannel(m_lChannels[0], Msg);

	NLog::WriteF("ChatServer (BROADCAST) ADMIN: %s\n", pMessage);
}
void CNetworkChatServer::OnNetMessage(CNetworkConnection *pConnection, NetMsgID iMsg, CInputStream &rStream)
{
	CUser *pUser = pConnection->GetData<CUser>(this); //m_lUsers.Find(pConnection, NULL);
	if(!pUser)
		return;
	
	if(iMsg == NETMSG("svChatMessage"))
	{
		CString message = rStream.Read_p16();
		Chat(pUser, message);
	}
	else if(iMsg == NETMSG("svSetChannel"))
	{
		CString channel = rStream.Read_p8();
		SetChannel(pUser, channel);
	}
}

void CNetworkChatServer::OnCommand(CString &rCmd, CString &rArgs)
{
	if(rCmd == "broadcast")
		Broadcast(rArgs);
}


bool CNetworkChatServer::SetUserName(CNetworkConnection *pConnection, const int8 *pName)
{
	if(!pConnection || !pConnection->GetData(this))
		return false;

	pConnection->GetData<CUser>(this)->m_Name = pName;
	return true;
}

bool CNetworkChatServer::SetChannel(CNetworkConnection *pConnection, const int8 *pName)
{
	CUser *pUser = pConnection->GetData<CUser>(this);
	if(!pUser)
		return false;
	return SetChannel(pUser, pName);
}
