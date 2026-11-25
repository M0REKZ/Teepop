//==========================================================================================
// Copyright (c), Clas M. Auvinen 2002
//==========================================================================================

#if defined(FOO_PLATFORM_LINUX) || defined(FOO_PLATFORM_OPENBSD) || defined(FOO_PLATFORM_FREEBSD)

#include <stdio.h>
#include <signal.h>

#include <foo/base/log.h>
#include <foo/func/filestream.h>
#include <foo/base/string.h>
// #include <ni/sys/posix.h>

#include "core.h"

using namespace NFoo;
using namespace NFoo::NCore;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class CKeyboard_Linux : public CKeyboard
{
	friend class CCore_Linux;
public:
};
MACRO_REGISTER_CLASS(CKeyboard_Linux, aKeyboardClassTag);
*/
//
class CCore_Linux : public CCore
{
public:
	//CKeyboard_Linux *m_pLinuxKeyboard;
	
public:
	virtual void Init()
	{ 
		//m_pLinuxKeyboard = new CKeyboard_Linux;
		//m_pLinuxKeyboard->m_pClassHandle = NClassRegistry::FindHandle("CKeyboard_Linux");
		//m_lKeyboard.Add(m_pLinuxKeyboard);
	}

	virtual bool Update() { return CCore::Update(); }
};


class CPrintFLogger : public NLog::CLogger
{
public:
	CPrintFLogger() { NLog::Attach(this); NLog::Write("CPrintFLogger attached\n"); }
	virtual ~CPrintFLogger() { NLog::Write("CPrintFLogger deattching\n"); NLog::Detach(this); }
	virtual void Write(const char *_pBuffer) { printf("%s", _pBuffer); }
};


void InterruptSignal(int)
{
	AddMessage(CMessage(EMsg_Quit));
}

//
// file logger
//
class CFileLogger : public NLog::CLogger
{
public:
	CFileWriter m_File;
	CFileLogger()
	{
		m_File.Open("corelog.txt");
		NLog::Attach(this);
		NLog::Write("CFileLogger attached\n");
	}

	~CFileLogger()
	{
		NLog::Write("CFileLogger deattching\n");
		NLog::Detach(this);
		m_File.Close();
	}

	virtual void Write(const char *_pBuffer)
	{
		m_File.Write(_pBuffer, StringLength(_pBuffer));
		m_File.Flush();
	}
};

//
// Main
//
int main(int argc, char* argv[])
{
	//CPrintFLogger Logger;
	CFileLogger FileLogger;
	
	//
	// Create The Core
	//
	InitCore(new CCore_Linux());
	
	signal(SIGINT, InterruptSignal);

	NLog::Write("MAIN:Initing\n");
	
	//char aDir[512];
//	int32_t Size = 512;
	//NPosix::getwd(aDir);
	//NLog::Write("MAIN: Working directory is %s\n", aDir);

	NLog::Write("MAIN:Calling NiMain()\n");
	FooMain("Empty");

	SafeDelete(g_pCore);

	return 1;
}

#endif

