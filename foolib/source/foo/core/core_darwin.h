#ifndef __FOO_CORE_DARWIN_H__
#define __FOO_CORE_DARWIN_H__

#include <SDL3/SDL_events.h>

#include "core.h"

namespace NFoo { namespace NCore {
    class CCore_OSX : public CCore
    {
    public:
        static CCore_OSX *Get() { return (CCore_OSX *)GetCore(); }
	    TDynamicArray<SDL_Event> m_lSDLEvents;

        virtual void Init();
        virtual bool Update();

    };
} } // name spaces

#endif // __FOO_CORE_DARWIN_H__
