#include "string.h"

#ifdef FOO_LOWLEVEL_STRING_STD
	#error "Standard string implemetation not done"
#endif // FOO_LOWLEVEL_STRING_STD

int32_t NFoo::StringGetHash(const int8 *_pName)
{
	int32_t h = 0;
	for (; *_pName; _pName++)
		h = 31*h + *_pName;
	return h;
}
