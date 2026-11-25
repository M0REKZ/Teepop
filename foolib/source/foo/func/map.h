#ifndef __FOO_MAP_H__
#define __FOO_MAP_H__

#include <foo/base/base.h>
#include "dynamicarray.h"
#include "pair.h"

namespace NFoo {
	template<typename TKEY, typename TVALUE>
	class TMap
	{
	public:
		typedef TPair<TKEY, TVALUE> CPair;
	private:
		// this should be something fast like an binary tree
		TDynamicArray<CPair> m_lPairs;
	public:


		// by index access
		CPair& GetPair(int32_t _Index) { return m_lPairs[_Index]; }
		TKEY GetKey(int32_t _Index) { return m_lPairs[_Index].m_First; }
		TVALUE &GetValue(int32_t _Index) { return m_lPairs[_Index].m_Second; }
		int32_t Len(void) { return m_lPairs.Len(); }

		// clears the whole map
		void Clear()
		{
			m_lPairs.Clear();
		}

		// is this function nessasary?
		int32_t Add(const CPair& _Pair)
		{
			int32_t iIndex = Find(_Pair.first);
			if (iIndex == -1)
				return m_lPairs.Add(_Pair);
			m_lPairs[iIndex] = _Pair;
			return iIndex;
		}

		//
		bool Exist(TKEY Key)
		{
			return Find(Key) != -1;
		}

		// returns the value of the key. if the key isn't found, _Default is returned
		TVALUE Find(TKEY Key, TVALUE _Default)
		{
			int32_t iPair = Find(Key);
			if(iPair < 0)
				return _Default;
			return m_lPairs[iPair].m_Second;
		}

		// returns the index of the element, -1 when not found
		int32_t Find(TKEY _Key)
		{
			for(int32_t i = 0; i < m_lPairs.Len(); i++)
			{
				if(m_lPairs[i].m_First == _Key)
					return i;
			}

			return -1;
		}

		// returns true when the key was successfully removed
		bool Remove(TKEY _Key)
		{
			int32_t iPair = Find(_Key);
			if(iPair < 0)
				return false;

			m_lPairs.RemoveByIndexFast(iPair);
			return true;
		}

		// modifies or creates a key
		TVALUE &operator[](TKEY _Key)
		{
			int32_t iPair = Find(_Key);
			if(iPair < 0)
				iPair = m_lPairs.Add(CPair(_Key));

			return m_lPairs[iPair].m_Second;
		}

		//
		TMap &operator = (const TMap &_rMap)
		{
			m_lPairs.Clear();
			for(int32_t i = 0; i < _rMap.m_lPairs.Len(); i++)
				(*this)[_rMap.m_lPairs[i].m_First] = _rMap.m_lPairs[i].m_Second;
			return *this;
		}

		// returns how much memory this map is using
		int32_t MemoryUsage()
		{
			return sizeof(TMap) + m_lPairs.MemoryUsage();
		}

		// removes unnessasary data, returns how many bytes was earned
		int32_t Optimize()
		{
			int32_t Before = MemoryUsage();
			m_lPairs.Optimize();
			return Before - MemoryUsage();
		}
	};
} // end of namespace

#endif // __FOO_MAP_H__


