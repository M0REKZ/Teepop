#ifndef __FOO_CLASSREGISTRY_H__
#define __FOO_CLASSREGISTRY_H__

#include <foo/base/base.h>
#include <foo/func/dynamicarray.h>

namespace NFoo { namespace NClassRegistry {
	enum
	{
		EInvalid=0xFFFF
	};

	class CClass
	{
	public:
		class CHandle *m_pClassHandle;
//		uint16_t m_iClass;
		virtual ~CClass() {}
	};
	typedef CClass *(*CLASSCREATEFUNC)();

	//
	//
	//
	class CHandle
	{
	public:
		CHandle(){};
		CHandle(const char *_pName, const char *_pTag, CLASSCREATEFUNC _pfnCreateFunc);
		int32_t m_iClass;  // 0 = invalid
		char m_aName[64]; // should be replaced by the string class
		char m_aTag[64]; // should be replaced by the string class
		CLASSCREATEFUNC m_pfnCreate;
		CHandle *m_pNext;
	};

	// list with all the registed classes
	extern CHandle *m_pFirstClass;

	void Register(const char *_pName, const char *_pTag, CLASSCREATEFUNC _pfnFunc);
	void Clear();
	
	int32_t FindHandleByTag(const char *_pTag, TDynamicArray<CHandle*> &rlHandles);
	CHandle *FindHandleByTag(const char *_pTag);
	CHandle *FindHandle(const char *_pName);
	int32_t GetClassID(const char *_pName);
	CClass *CreateByTag(const char *_pTag);
	CClass *CreateByName(const char *_pName);
	CClass *CreateByHandle(CHandle *_pHandle);
	
	void DumpClasses();

	//
	// Use this macro to implement the class regestring rutines
	//
	#define MACRO_REGISTER_CLASS(cl,tag) NFoo::NClassRegistry::CClass *ClassCreate_##cl() { return new cl(); } class CClassRegisterer_##cl { public: CClassRegisterer_##cl(int32_t reg) { if(reg) NFoo::NClassRegistry::Register(#cl, tag, ClassCreate_##cl); } }; CClassRegisterer_##cl g_ClassRegistrer_##cl(1); void Touch_##cl() { CClassRegisterer_##cl foo(0); }
	
	#define MACRO_TOUCH_CLASS(cl) extern void Touch_##cl(); class CClassTouch_##cl { public: CClassTouch_##cl(){Touch_##cl();} }; CClassTouch_##cl g_ClassTouch_##cl;
		
} }

#endif // __FOO_CLASSREGISTRY_H__

