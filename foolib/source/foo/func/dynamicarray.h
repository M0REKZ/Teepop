#ifndef __FOO_DYNAMICARRAY_H__
#define __FOO_DYNAMICARRAY_H__

#include <foo/base/base.h>

namespace NFoo {
	template <class T>
	class TDynamicArray
	{
	public:
		TDynamicArray()
		{
			Init();
		}
		
		//
		TDynamicArray(const TDynamicArray &rArray)
		{
			Init();
			SetLen(rArray.Len());
			for(int32 i = 0; i < Len(); i++)
				(*this)[i] = rArray[i];
		}

	
		// 
		//
		virtual ~TDynamicArray()
		{
			SafeArrayDelete(m_List);
		}
	
		// 
		//
		void Init()
		{
			m_List = NULL;
			Clear();
		}

		//
		//
		void DeleteAll()
		{
			for(int32 i = 0; i < Len(); i++)
				delete m_List[i];

			Clear();
		}
	
	
		//
		//
		void Clear()
		{
			SafeArrayDelete(m_List);
			
			m_ListSize = 1;
			m_List = new T[1];
			m_NumberOfElements = 0;
		}

		int32 Find(T val)
		{
			for(int32 i = 0; i < Len(); i++)
				if((*this)[i] == val)
					return i;
			return -1;
		}
	
		bool Exist(T val)
		{
			return Find() != -1;
		}

		//
		// returns the number of elements in the list
		//
		int32 Len() const
		{
			return m_NumberOfElements;
		}
	
		//
		// This doesn't conserve the order in the list. Be careful
		//
		void RemoveByIndexFast(int32 _Pos)
		{
			//ASSUME(_Pos >= 0 && _Pos < m_NumberOfElements);
			m_List[_Pos] = m_List[m_NumberOfElements-1];
			SetLen(Len()-1);
		}
	
		void RemoveFast(const T& _Elem)
		{
			for(int32 i = 0; i < Len(); i++)
				if(m_List[i] == _Elem)
				{
					RemoveByIndexFast(i);
					return;
				}
		}

		//
		//
		void RemoveByIndex(int32 _Pos)
		{
			//ASSUME(_Pos >= 0 && _Pos < m_NumberOfElements);

			for(int32 i = _Pos+1; i < m_NumberOfElements; i++)
				m_List[i-1] = m_List[i];
			
			SetLen(Len()-1);
		}
	
		bool Remove(const T& _Elem)
		{
			for(int32 i = 0; i < Len(); i++)
				if(m_List[i] == _Elem)
				{
					RemoveByIndex(i);
					return true;
				}
			return false;
		}
	
		// 
		//
		int32 Add(const T& _Elem)
		{
			//if(m_NumberOfElements == m_ListSize)
			SetLen(Len()+1);
			m_List[m_NumberOfElements-1] = _Elem;
			return m_NumberOfElements-1;
		}
	
		// 
		//
		int32 Add(const T& _Elem, int32 _Index)
		{
			//if(m_NumberOfElements == m_ListSize)
			SetLen(Len()+1);
			
			for(int32 i = m_NumberOfElements-1; i > _Index; i--)
				m_List[i] = m_List[i-1];
	
			m_List[_Index] = _Elem;
			
			//m_NumberOfElements++;
			return m_NumberOfElements-1;
		}
	
		// 
		//
		T& operator[] (int32 _Pos)
		{
			return m_List[_Pos];
		}

		//
		//
		const T& operator[] (int32 _Pos) const
		{
			return m_List[_Pos];
		}

		//
		//
		T *GetBasePtr()
		{
			return m_List;
		}
	
		const T *GetBasePtr() const
		{
			return m_List;
		}
	
		// 
		//
		/*
		void SetLen(int32 _Len)
		{
			m_ListSize = _Len;
			T *NewList = new T[m_ListSize];
			
			int32 end = m_NumberOfElements < m_ListSize ? m_NumberOfElements : m_ListSize;
			for(int32 i = 0; i < end; i++)
				NewList[i] = m_List[i];
			
			SafeArrayDelete(m_List);
	
			m_NumberOfElements = m_NumberOfElements < m_ListSize ? m_NumberOfElements : m_ListSize;
			m_List = NewList;
		}*/
	
		void SetLen(int32 _Len)
		{
			if (m_ListSize < _Len)
				AllocLen(_Len);
			m_NumberOfElements = _Len;
		}

		// removes unnessasary data, returns how many bytes was earned
		int32 Optimize()
		{
			int32 Before = MemoryUsage();
			AllocLen(m_NumberOfElements);
			return Before - MemoryUsage();
		}

		// returns how much memory this dynamic array is using
		int32 MemoryUsage()
		{
			return sizeof(TDynamicArray) + sizeof(T)*m_ListSize;
		}

		//
		TDynamicArray &operator = (const TDynamicArray &rArray)
		{
			SetLen(rArray.Len());
			for(int32 i = 0; i < Len(); i++)
				(*this)[i] = rArray[i];
			return *this;
		}		
	private:

		void AllocLen(int32 _Len)
		{
			m_ListSize = _Len;
			T *NewList = new T[m_ListSize];
			
			int32 end = m_NumberOfElements < m_ListSize ? m_NumberOfElements : m_ListSize;
			for(int32 i = 0; i < end; i++)
				NewList[i] = m_List[i];
			
			SafeArrayDelete(m_List);
	
			m_NumberOfElements = m_NumberOfElements < m_ListSize ? m_NumberOfElements : m_ListSize;
			m_List = NewList;
		}

		T *m_List;
		int32 m_ListSize;
		int32 m_NumberOfElements;
	};

} // end of namespace

#endif // __FOO_DYNAMICARRAY_H__


