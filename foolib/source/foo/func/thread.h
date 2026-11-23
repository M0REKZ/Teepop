#ifndef FOO_FILE_FUNC_THREAD
#define FOO_FILE_FUNC_THREAD

#include <foo/base/base.h>

namespace NFoo
{

	//
	// Handles a thread
	//
	class CThread
	{
		enum
		{
			EThread_Stopped=0,
			EThread_Running,
			EThread_Terminate
		};

		int32 m_ThreadState; // Lock: m_CommandLock
		
		//
		// The function that is passed to CreateThread
		//
		static void Main(void *pThis);
		void SetRunning(bool Running);

	protected:
		CLock m_CommandLock;


	public:
		CThread();

		//
		// The thread main function.
		//
		virtual int32 ThreadMain() pure;

		bool AwaitingTermination();
		bool Running();
		void WaitForTermination();
		void SendTermination();
		bool StartThread(); // Starts the thread. Returns false if the thread is already running
	};
}

#endif // FOO_FILE_FUNC_THREAD