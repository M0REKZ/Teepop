//==========================================================================================
// Copyright (c), Clas M. Auvinen 2003
//==========================================================================================

#ifndef FOO_FUNC_FILE_MESSAGE_H
#define FOO_FUNC_FILE_MESSAGE_H

#include <foo/base/base.h>
#include "string.h"
#include "map.h"

namespace NFoo {
	
	//
	// we should replace this with something faster
	//
	class CMessage
	{
		TMap<int32, int32> m_lIntValues;
		TMap<int32, fp32> m_lFloatValues;
		TMap<int32, CString> m_lStringValues;
		class CMessageQueue *m_pTarget;

	public:
		int32 m_What; // translated
		int32 m_OrgMsg; // not translated

		CMessage(int32 what=0, CMessageQueue *pTarget = NULL);
		CMessage(const CMessage &rMessage);
		CMessage &Duplicate() { return *(new CMessage(*this)); }

		void SetTarget(CMessageQueue *pTarget);

		void SetInt(const int8 *_pName, int32 _Value);
		void SetFloat(const int8 *_pName, fp32 _Value);
		void SetString(const int8 *_pName, const int8 *_pValue);

		int32 GetInt(const int8 *_pName, int32 _Default=0);
		fp32 GetFloat(const int8 *_pName, fp32 _Default=0);
		CString GetString(const int8 *_pName, const int8 *_pDefault=NULL);
	};


	//
	//
	//
	class CMessageReceiver
	{
	public:
		virtual ~CMessageReceiver() {}
		virtual void PostMessage(CMessage &rMsg) {}
	};

	//
	// FIFO
	//
	class CMessageQueue : public CMessageReceiver
	{
		TDynamicArray<CMessage*> m_lMessages;
	public:
		~CMessageQueue();
		virtual void PostMessage(CMessage &rMsg); // copies the message

		bool GotMessage();
		CMessage &GetMessage();
		void DispatchMessage();
	};

	//
	//
	//
	class CMessageTarget
	{
	public:
		CMessageTarget(CMessageReceiver *pReceiver = NULL, CMessage Msg = CMessage(-1))
		{
			m_pReceiver = pReceiver;
			m_Msg = Msg;
		}

		CMessageReceiver *m_pReceiver;
		CMessage m_Msg;
	};

	//
	//
	//
	class CMessageSender
	{
		TMap<int32, CMessageTarget> m_MessageMap;
	protected:
		void Signal(int32 iMsg);
		void Signal(CMessage &rMsg);
	public:
		// Events
		bool Bind(int32 iMsg, CMessageTarget Target);
		bool Bind(int32 iMsg, CMessageReceiver *pReceiver, int32 iTargetMsg);
		bool Bind(int32 iMsg, CMessageReceiver *pReceiver, CMessage &rMsg);
	};

}

#endif // FOO_FUNC_FILE_MESSAGE_H