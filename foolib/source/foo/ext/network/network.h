//==========================================================================================
// Copyright (c), Clas M. Auvinen 2002
//==========================================================================================

#ifndef __FOO_NETWORK_H__
#define __FOO_NETWORK_H__

//
// Includes
//
#include <foo/base/base.h>

namespace NFoo { namespace NNetwork {

	class CAddr
	{
	public:
		friend class CSocket;

		//
		uint8 m_aAddress[4];
		uint16 m_Port;

		// 
		CAddr();
		CAddr(uint16 _Port);
		CAddr(uint8 _1, uint8 _2, uint8 _3, uint8 _4, uint16 _Port);
		CAddr(const int8 *_pAddr, uint16 _Port);

		//
		const int8 *Str();


		//
		void Set(const int8 *_pAddr, uint16 _Port);
		void SetAny(uint16 _Port);

		//
		int32_t operator=(CAddr &_pAddr);
		int32_t operator==(CAddr &_pAddr);

	};
	
	CAddr AddrFromHostname(const char* _Name, uint16 _Port);

	class CSocket
	{
		int32_t m_hSocket;
		CAddr m_BoundAddress;
	
	public:
		enum
		{
			EType_DGRAM=1,
			EType_Stream=2,
			EType_UDP=1,
			EType_TCP=2
		};

		CSocket();
		CSocket(int32_t _hSocket);
		virtual ~CSocket();
		
		//
		bool Valid();

		bool Create(int32_t _Type);
		void SetBlocking(bool _Block);
		bool Bind(CAddr _Address);

		// TCP
		bool Close();
		bool Connect(CAddr _Address);
		bool Listen();
		CSocket *Accept();

		bool Send(const void *_pData, int32_t _DataSize);
		int32_t Recv(void *_pData, int32_t _DataSize);

		// UDP
		bool SendTo(const void *_pData, int32_t _DataSize, CAddr &_Target);
		int32_t RecvFrom(void *_pData, int32_t _DataSize, CAddr &_Source);
	};
} } // namespaces

#endif // __FOO_NETWORK_H__


