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
		TMap<int32_t, int32_t> m_lIntValues;
		TMap<int32_t, fp32> m_lFloatValues;
		TMap<int32_t, CString> m_lStringValues;
		class CMessageQueue *m_pTarget;

	public:
		int32_t m_What; // translated
		int32_t m_OrgMsg; // not translated

		CMessage(int32_t what=0, CMessageQueue *pTarget = NULL);
		CMessage(const CMessage &rMessage);
		CMessage &Duplicate() { return *(new CMessage(*this)); }

		void SetTarget(CMessageQueue *pTarget);

		void SetInt(const char *_pName, int32_t _Value);
		void SetFloat(const char *_pName, fp32 _Value);
		void SetString(const char *_pName, const char *_pValue);

		int32_t GetInt(const char *_pName, int32_t _Default=0);
		fp32 GetFloat(const char *_pName, fp32 _Default=0);
		CString GetString(const char *_pName, const char *_pDefault=NULL);
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
		TMap<int32_t, CMessageTarget> m_MessageMap;
	protected:
		void Signal(int32_t iMsg);
		void Signal(CMessage &rMsg);
	public:
		// Events
		bool Bind(int32_t iMsg, CMessageTarget Target);
		bool Bind(int32_t iMsg, CMessageReceiver *pReceiver, int32_t iTargetMsg);
		bool Bind(int32_t iMsg, CMessageReceiver *pReceiver, CMessage &rMsg);
	};

}

#endif // FOO_FUNC_FILE_MESSAGE_H