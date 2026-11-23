#if defined(FOO_PLATFORM_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <winsock.h>
#elif defined(FOO_PLATFORM_POSIX)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <errno.h>
	#include <unistd.h>
	#include <netinet/in.h>
#else
	#error
#endif

#include <foo/base/base.h>
#include <foo/base/log.h>
#include <foo/base/memory.h>
#include "network.h"

using namespace NFoo;
using namespace NFoo::NNetwork;

#if defined(FOO_PLATFORM_WIN32)
	class CNetworkIniter
	{
	public:
		CNetworkIniter()
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);
		}

		~CNetworkIniter()
		{
			WSACleanup();
		}
	};

	CNetworkIniter g_NetworkIniter;
#endif

//
//
//
#if defined(FOO_PLATFORM_WIN32)
	sockaddr AddrToSockAddr(CAddr &_Addr)
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
//		addr.sin_port = Swap16CPUtoBE(_Addr.m_Port);
		addr.sin_port = ByteSwapCPUtoBE(_Addr.m_Port);
		addr.sin_addr.S_un.S_un_b.s_b1 = _Addr.m_aAddress[0];
		addr.sin_addr.S_un.S_un_b.s_b2 = _Addr.m_aAddress[1];
		addr.sin_addr.S_un.S_un_b.s_b3 = _Addr.m_aAddress[2];
		addr.sin_addr.S_un.S_un_b.s_b4 = _Addr.m_aAddress[3];

		return *((sockaddr*)&addr);
	}

	CAddr SockAddrToAddr(sockaddr _Addr)
	{
		sockaddr_in &saddr = *((sockaddr_in*)&_Addr);
		CAddr Addr;
//		Addr.m_Port =  Swap16BEtoCPU(saddr.sin_port);
		Addr.m_Port = ByteSwapBEtoCPU(saddr.sin_port);
		Addr.m_aAddress[0] = saddr.sin_addr.S_un.S_un_b.s_b1;
		Addr.m_aAddress[1] = saddr.sin_addr.S_un.S_un_b.s_b2;
		Addr.m_aAddress[2] = saddr.sin_addr.S_un.S_un_b.s_b3;
		Addr.m_aAddress[3] = saddr.sin_addr.S_un.S_un_b.s_b4;

		return Addr;
	}
#else
	sockaddr AddrToSockAddr(CAddr &_Addr)
	{
		sockaddr_in addr;
		MemorySet(&addr, 0, sizeof(sockaddr_in));
		addr.sin_family = AF_INET;
//		addr.sin_port = Swap16CPUtoBE(_Addr.m_Port);
		addr.sin_port = ByteSwapCPUtoBE(_Addr.m_Port);
//		addr.sin_addr.s_addr = Swap32CPUtoBE(_Addr.m_aAddress[0]<<24|_Addr.m_aAddress[1]<<16|_Addr.m_aAddress[2]<<8|_Addr.m_aAddress[3]);
		addr.sin_addr.s_addr = ByteSwapCPUtoBE(_Addr.m_aAddress[0]<<24|_Addr.m_aAddress[1]<<16|_Addr.m_aAddress[2]<<8|_Addr.m_aAddress[3]);
		return *((sockaddr*)&addr);
	}

	CAddr SockAddrToAddr(sockaddr _Addr)
	{
		sockaddr_in &saddr = *((sockaddr_in*)&_Addr);
		CAddr Addr;
//		Addr.m_Port =  Swap16BEtoCPU(saddr.sin_port);
		Addr.m_Port = ByteSwapBEtoCPU(saddr.sin_port);
//		uint32 ip = Swap32BEtoCPU(saddr.sin_addr.s_addr);
		uint32 ip = ByteSwapBEtoCPU(saddr.sin_addr.s_addr);
		Addr.m_aAddress[0] = (uint8)((ip>>24)&0xFF);
		Addr.m_aAddress[1] = (uint8)((ip>>16)&0xFF);
		Addr.m_aAddress[2] = (uint8)((ip>>8)&0xFF);
		Addr.m_aAddress[3] = (uint8)(ip&0xFF);

		return Addr;
	}
#endif

//
//
//
CSocket::CSocket()
{
	m_hSocket = -1;
}

CSocket::CSocket(int32 _hSocket)
{
	m_hSocket = _hSocket;
}

//
//
//
CSocket::~CSocket()
{
	Close();
}

//
//
//
bool CSocket::Create(int32 _Type)
{
	intr SockType = -1;

	if(_Type == EType_TCP)
		SockType = SOCK_STREAM;
	else if(_Type == EType_UDP)
		SockType = SOCK_DGRAM;

	if(SockType == -1)
		return false;

	m_hSocket = socket(AF_INET, SockType, 0);

	if(m_hSocket <= 0)
		return false;

	//int size = 256*1024;
	//setsockopt(m_hSocket, SOL_SOCKET, SO_SNDBUF, (int8*)&size, 4);
	//size = 128*1024;
	//setsockopt(m_hSocket, SOL_SOCKET, SO_RCVBUF, (int8*)&size, 4);

	return true;
}

//
//
//
bool CSocket::Valid()
{
	return m_hSocket >= 0;
}

//
//
//
void CSocket::SetBlocking(bool _Block)
{
	uint32 mode = _Block == false;
	#if defined(FOO_PLATFORM_WIN32)
		ioctlsocket(m_hSocket, FIONBIO, (unsigned long *)&mode);
	#else
		if(ioctl(m_hSocket, FIONBIO, (unsigned long *)&mode) == -1)
			NLog::WriteF("Socket: error setting socket blocking mode\n");
	#endif
}

//
// TCP Only
//
bool CSocket::Connect(CAddr _Address)
{
	//m_BoundAddress = _Address;
	sockaddr Address = AddrToSockAddr(_Address);
	if(connect(m_hSocket, (sockaddr *)&Address, sizeof(sockaddr)) != 0)
		return false;
	
	return true;
}

//
//
//
bool CSocket::Close()
{
	if(m_hSocket >= 0)
	{
		#if defined(FOO_PLATFORM_WIN32)
			closesocket(m_hSocket);
		#else
			close(m_hSocket);
		#endif
		
		shutdown(m_hSocket, 0);

		return true;
	}

	return false;
}

//
//
//
bool CSocket::Listen()
{
	if(listen(m_hSocket, SOMAXCONN) != 0)
		return false;
	return true;
}

//
//
//
CSocket *CSocket::Accept()
{
	sockaddr Address;
#if defined(FOO_PLATFORM_POSIX)
	socklen_t Len = sizeof(sockaddr);
	int32 hSocket = accept(m_hSocket, (sockaddr *)&Address, &Len);
#else
	intr Len = sizeof(sockaddr);
	int32 hSocket = accept(m_hSocket, (sockaddr *)&Address, &Len);
#endif

	if(hSocket < 0)
		return NULL;

	return new CSocket(hSocket);
}

//
//
//
bool CSocket::Bind(CAddr _Address)
{
	m_BoundAddress = _Address;
	sockaddr Address = AddrToSockAddr(_Address);
	if(bind(m_hSocket, (sockaddr *)&Address, sizeof(sockaddr)) != 0)
	{
		NLog::WriteF("Socket: Failed to bind\n");
		return false;
	}
	
	NLog::WriteF("Socket: bind=%s ok\n", m_BoundAddress.Str());
	return true;
}

//
//
//
bool CSocket::SendTo(const void *_pData, int32 _DataSize, CAddr &_Target)
{
	if(!_pData)
		return true;

	if(_DataSize < 1)
		return true;

	sockaddr Address = AddrToSockAddr(_Target);
	if(sendto(m_hSocket, (int8 *)_pData, _DataSize, 0, (sockaddr *)&Address, sizeof(sockaddr)) < 0)
		return false;

	return true;
}

//
//
//
int32 CSocket::Recv(void *_pData, int32 _DataSize)
{
	return recv(m_hSocket, (int8 *)_pData, _DataSize, 0);
}

//
//
//
bool CSocket::Send(const void *_pData, int32 _DataSize)
{
	int32 Ret = send(m_hSocket, (int8 *)_pData, _DataSize, 0);
	return Ret!=0;
}

//
//
//
int32 CSocket::RecvFrom(void *_pData, int32 _DataSize, CAddr &_Source)
{
	int32 Bytes;
	sockaddr Address;
	//if((Bytes = recvfrom(m_SocketHandle, (int8 *)_pData, _DataSize, 0, (sockaddr *)&Address, &AddrLen)) != 0)
	//	return Bytes;

#if defined(FOO_PLATFORM_POSIX)
	socklen_t AddrLen = sizeof(sockaddr);
	Bytes = recvfrom(m_hSocket, (int8 *)_pData, _DataSize, 0, (sockaddr *)&Address, &AddrLen);
#else
	intr AddrLen = sizeof(sockaddr);
	Bytes = recvfrom(m_hSocket, (int8 *)_pData, _DataSize, 0, (sockaddr *)&Address, &AddrLen);
#endif

	CAddr Addr = SockAddrToAddr(Address);
	_Source = Addr;

	return Bytes;
}

