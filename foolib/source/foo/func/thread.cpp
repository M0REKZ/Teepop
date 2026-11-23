#include "thread.h"

//# Req_Namespace: NFoo
//# Req_Header: func/thread.h
//# Req_Library: 
//
//# Context: Func/CThread
//


using namespace NFoo;


CThread::CThread()
{
	m_ThreadState = EThread_Stopped;
}

//
// The function that is passed to CreateThread
//
void CThread::Main(void *pThis)
{
	CThread *pThread = (CThread*)pThis;

	// make sure that the thread doesn't enter it's loop before Start() returns
	{ CAutoLock Lock(pThread->m_CommandLock); }

	// run the thread main
	pThread->ThreadMain();

	// set running to false when the thread dies
	{
		CAutoLock Lock(pThread->m_CommandLock);
		pThread->m_ThreadState = EThread_Stopped;
	}
}

//# Function: AwaitingTermination
//# Description: Returns true if the thread is awaiting termination
//# Declaration:
bool CThread::AwaitingTermination()
{
	CAutoLock Lock(m_CommandLock);
	return m_ThreadState == EThread_Terminate;
}

//# Function: SendTermination
//# Description: Sends a request to the thread to terminate.
//# Declaration:
void CThread::SendTermination()
{
	CAutoLock Lock(m_CommandLock);
	if(m_ThreadState == EThread_Running)
		m_ThreadState = EThread_Terminate;
}

//# Function: Running
//# Description: Returns true if the thread is running.
//# Declaration:
bool CThread::Running()
{
	CAutoLock Lock(m_CommandLock);
	return m_ThreadState != EThread_Stopped;
}

//# Function: WaitForTermination
//# Description: Stalls the current thread until this thread terminates.
//# Declaration:
void CThread::WaitForTermination()
{
	while(Running())
		Sleep(25);
}

//# Function: StartThread
//# Description: Starts the thread. Returns false if the thread is already running.
//# Declaration:
bool CThread::StartThread()
{
	CAutoLock Lock(m_CommandLock);
	if(m_ThreadState != EThread_Stopped)
		return false;

	CreateThread(Main, this); // main wont continue until this thread returns becuase it need the commandlock
	m_ThreadState = EThread_Running;
	return true; 
}

