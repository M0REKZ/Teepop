#ifndef __FOO_PAIR_H__
#define __FOO_PAIR_H__

#include <foo/base/base.h>

namespace NFoo {
	template<typename TFIRSTTYPE, typename TSECONDTYPE>
	class TPair
	{
	public:
		TPair(){}
		TPair(TFIRSTTYPE _First)
		: m_First(_First)
		{}

		TPair(TFIRSTTYPE _First, TSECONDTYPE _Second)
		: m_First(_First), m_Second(_Second)
		{}

		TFIRSTTYPE m_First;
		TSECONDTYPE m_Second;
	};
} // end of namespace

#endif // __FOO_PAIR_H__


