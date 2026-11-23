#ifndef __FOO_CORE_H__
#define __FOO_CORE_H__

#include <foo/base/base.h>
#include <foo/func/dynamicarray.h>
#include <foo/func/classregistry.h>
#include <foo/func/string.h>
#include "messages.h"

//# Context: Core
//# Req_Namespace: NFoo::NCore
//# Req_Header: core/core.h
//# Req_Library: foocore.lib

namespace NFoo { namespace NCore {

	//
	//
	//
	class CMessage
	{
	public:
		CMessage() { m_MessageID = EMsg_Empty; }
		CMessage(int32 _ID) { m_MessageID = _ID; }
		int32 m_MessageID;
		int32 m_aParameters[4];
	};

	//
	//
	//
	class CSubCore : public NClassRegistry::CClass
	{
	public:
		CSubCore();
		virtual ~CSubCore();
	
		int32 m_iCore;
		
		virtual bool Update() { return true; };
		//virtual bool OnMessage();

		// passes message to the logger. adds some formatting
		void Log(int8 *_pFormat, ...); 
	};

	//
	//
	//
	class CCore
	{
	public:
		virtual ~CCore();

		virtual void Init();
		virtual bool Update();
		int32 m_iCurrent;

		// Objects
		NFoo::TDynamicArray<CSubCore *> m_lSubCore;
		
		// application messages
		NFoo::TDynamicArray<CMessage> m_lMessageQueue;
	};
	extern CCore *g_pCore; // DONT TOUCH THIS
	inline CCore *GetCore() { return g_pCore; }

	//# Function: Update
	//# Description: Refreshes the core and all it's subsystems.
	//# Returns: Returns true if no error occurd, else, false.
	//# Declaration:
	bool Update();

	//# Function: UpdateAndPump
	//# Description: Runs Update() and pumps all messages
	//# Returns: Returns false when the core has quit flag set.
	//# SeeAlso: Update
	//# Declaration:
	bool UpdateAndPump(); // , returns false when its time to quit
	bool GotMessages();
	CMessage GetMessage();

	// implementation
	void InitCore(CCore *_pCore);
	void AddMessage(CMessage _msg);


} } // name spaces

// Entry Point for applications
int32 FooMain(char *_pCmdLine);

#endif // __FOO_CORE_H__

