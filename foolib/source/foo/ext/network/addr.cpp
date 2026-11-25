#if defined(FOO_PLATFORM_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <winsock.h>
#elif defined(FOO_PLATFORM_POSIX)
	//#ifdef FOO_COMPILER_CW
	//	typedef	u_int32_t	in_addr_t;	/* base type for internet address */
	//#endif

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <netdb.h>
	#include <netinet/in.h>
#else
	#error
#endif


#include <string.h>
#include <stdio.h>
#include "network.h"

using namespace NFoo;
using namespace NFoo::NNetwork;

CAddr::CAddr()
{
	SetAny(0);
}

CAddr::CAddr(uint16 _Port)
{
	SetAny(_Port);
}

CAddr::CAddr(uint8 _1, uint8 _2, uint8 _3, uint8 _4, uint16 _Port)
{
	uint8 Addr[4];
	Addr[0] = _1;
	Addr[1] = _2;
	Addr[2] = _3;
	Addr[3] = _4;
	Set((int8*)Addr, _Port);
}

CAddr::CAddr(const int8 *_pAddr, uint16 _Port)
{
	Set(_pAddr, _Port);
}

const int8 *CAddr::Str()
{
	static int8 aString[32];
	sprintf(aString, "%d.%d.%d.%d:%d", m_aAddress[0], m_aAddress[1], m_aAddress[2], m_aAddress[3], m_Port);
	return aString;
}

CAddr NNetwork::AddrFromHostname(const char* _Name, uint16 _Port)
{
	CAddr Addr;
	struct hostent* pAddr = gethostbyname(_Name);

	// Returns address in network byte order
	if (pAddr && (pAddr->h_length > 0))
	{
		memcpy(Addr.m_aAddress, pAddr->h_addr, sizeof(int8)*4);
		Addr.m_Port = _Port;
	}
	return Addr;
}

void CAddr::Set(const int8 *_pAddr, uint16 _Port)
{
	memcpy(m_aAddress, _pAddr, sizeof(int8)*4);
	m_Port = _Port;
}

void CAddr::SetAny(uint16 _Port)
{
	m_Port = _Port;
	m_aAddress[0] = m_aAddress[1] = m_aAddress[2] = m_aAddress[3] = 0;
}

int32_t CAddr::operator=(CAddr &_pAddr)
{
	m_Port = _pAddr.m_Port;
	memcpy(m_aAddress, _pAddr.m_aAddress, sizeof(int8)*4);
	return 0;
}

int32_t CAddr::operator==(CAddr &_pAddr)
{
	if(m_aAddress[0] == _pAddr.m_aAddress[0] &&	m_aAddress[1] == _pAddr.m_aAddress[1] &&
		m_aAddress[2] == _pAddr.m_aAddress[2] &&m_aAddress[3] == _pAddr.m_aAddress[3] &&
		m_Port == _pAddr.m_Port)
	{
		return true;
	}

	return false;
}

