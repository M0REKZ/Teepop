// Part of foolib. Check docs/license.txt for licensing information.

#ifndef __FOO_LOG_H__
#define __FOO_LOG_H__

#include "base.h"
#include "sys.h"

//# Context: Base/Log
//# Req_Namespace: NFoo::NLog
//# Req_Header: base/log.h
//# Req_Library: foobase.lib

namespace NFoo { namespace NLog {
	class CLogger;

	extern CLock m_Lock;
	
	//# Function: Write
	//# Description: Passes a zero-terminated string to the loggers.
	//# Parameters:
	//#		Msg, String to pass.
	//# SeeAlso: WriteF
	//# Declaration:
	void Write(const char *_pMsg);

	//# Function: WriteF
	//# Description: Passes a zero-terminated string to the loggers. Performs printf styled formatting.
	//# Parameters:
	//#		Format, Format string.
	//#		..., Parameters for the format string.
	//# Remarks: Look at your documentation for the printf function for more information.
	//# SeeAlso: WriteF
	//# Declaration:
	void WriteF(const char *_pFormat, ...);

	//# Function: Attach
	//# Description: Adds a logger.
	//# Parameters:
	//#		Logger, Logger to add.
	//# Remarks: When a logger is attached it will recive all messages passed by Write and WriteF
	//# SeeAlso: Detach, Write, WriteF
	//# Declaration:
	void Attach(CLogger *_pLogger);
	
	//# Function: Detach
	//# Description: Removes a logger.
	//# Parameters:
	//#		Logger, Logger to remove.
	//# SeeAlso: Attach
	//# Declaration:
	void Detach(CLogger *_pLogger);
	
	class CLogger
	{
		CLogger *m_pNext;
		friend void NFoo::NLog::Attach(CLogger *_pLogger);
		friend void NFoo::NLog::Detach(CLogger *_pLogger);
		friend void Write(const char *_pBuf);
	public:
		virtual void Write(const char *pBuffer) pure;
	};

	extern CLogger *m_pFirst;
} }

#endif // __FOO_LOG_H__

