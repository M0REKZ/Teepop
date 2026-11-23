#ifndef FOO_FILE_ENGINE_ENGINE_H
#define FOO_FILE_ENGINE_ENGINE_H

#include <foo/func/dynamicarray.h>
#include <foo/func/map.h>
#include <foo/func/classregistry.h>
#include <foo/func/string.h>

namespace NEngine
{
	using namespace NFoo;

	class CCore;
	class CModule;

	class CConsoleClient
	{
	public:
		virtual void OnCommand(CString &rCmd, CString &rArgs) {}
	};

	//
	//
	//
	class CModule : public NClassRegistry::CClass
	{
		friend class CCore;
	protected:
		CCore *m_pCore;
		virtual bool Init() { Assert(m_pCore); return true; }
	public:
		CModule() : m_pCore(NULL) {}
		virtual ~CModule() {}
		
		//virtual void RegisterCommand(const int8 *pCmd);
		//virtual void OnCommand(CString &rCmd, CString &rArgs) {}

		virtual const int8 *GetStatusString() { return "Unknown"; }
		virtual void Update() {}
	};

	//
	//
	//
	class CCore
	{
		TMap<CString, CConsoleClient*> m_mCommandMap;
		TDynamicArray<CModule*> m_lModules;
		bool AddModule(CModule *pModule);
	public:
		virtual ~CCore();

		//# Function: LoadModule (T)
		//# Description: Loads a module, adds it to the core and sets a pointer.
		//# Parameters:
		//#		rpModule, Pointer that will be set to the load module.
		//#		pName, Name of the module to load.
		//# Declaration:
		template<class T> bool LoadModule(T *&rpModule, const int8 *pName)
		{
			return (rpModule = (T*)LoadModule(pName)) != 0;
		}

		CModule *LoadModule(const int8 *pName);
		CModule *GetModule(const int8 *pName);

		void RegisterCommand(const int8 *pCmd, CConsoleClient *pModule);
		bool DispatchCommand(const int8 *pCmdLine);

		bool Update();
	};
};

#endif // FOO_FILE_ENGINE_ENGINE_H
