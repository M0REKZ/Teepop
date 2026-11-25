#ifndef __FOO_SYS_H__
#define __FOO_SYS_H__

#include "base.h"
#include "datatypes.h"

//# Context: Base/System
//# Req_Namespace: NFoo
//# Req_Header: base/sys.h
//# Req_Library: foobase.lib

namespace NFoo
{
	//# Function: GetTickFrequency
	//# Description: Gets the high precision counter frequency.
	//# Returns: Frequency in hz of the counter
	//# Remarks: Under Win32 this value is often equal to the processorspeed.
	//# SeeAlso: GetTickCount
	//# Declaration:
	int64_t GetTickFrequency();

	//# Function: GetTickCount
	//# Description: Gets the high precision counter.
	//# Returns: Tick since system boot.
	//# SeeAlso: GetTickFrequency
	//# Declaration:
	int64_t GetTickCount();

	//# Function: MessageBox
	//# Description: Displays a message box on platforms that support it.
	//# Parameters:
	//#		Message, Message to display
	//#		Caption, Message box caption
	//# Declaration:
	void MessageBox(const char *_pMsg,const char *_pCaption);

	//# Function: Sleep
	//# Description: Pauses the program execution.
	//# Parameters:
	//#		Time, Time to sleep in milliseconds.
	//# Remarks: This function usualy forces the OS to make a context switch.
	//#			 Could be usefull to run before profiling some important function.
	//# Declaration:
	void Sleep(int32_t _Time);

	//# Function: DebugBreak
	//# Description: Breaks program execution for debugging purpose.
	//# Remarks: Make sure that you have a debugger attached to the process.
	//# Declaration:
	void DebugBreak();

	// threading
	class CLock
	{
		void *m_pPrivateData;
	public:
		CLock();
		~CLock();

		void Aquire();
		void Release();
	};

	class CAutoLock
	{
		CLock &m_rLock;
	public:
		CAutoLock(CLock &rLock)
		: m_rLock(rLock)
		{
			m_rLock.Aquire();
		}

		~CAutoLock()
		{
			m_rLock.Release();
		}
	};
	/*
	typedef void *CLock;

	CLock CreateLock();
	void DeleteLock(CLock lock);
	void AquireLock(CLock lock);
	void ReleaseLock(CLock lock);
	*/

	bool CreateThread(void (*_pfn)(void*), void *_Arg);
}

#endif // __FOO_SYS_H__


