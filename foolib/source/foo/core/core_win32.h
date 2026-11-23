#if !defined(_WINDOWS_) && !defined(_WINDOWS_H)
#error "You must include windows.h first!"
#endif

#include "core.h"

namespace NFoo { namespace NCore {

class CCore_Win32 : public CCore
{
public:
	static CCore_Win32 *Get() { return (CCore_Win32 *)GetCore(); }
	TDynamicArray<MSG> m_lWinMessages;

	virtual void Init();
	virtual bool Update();
};

} } // name spaces
