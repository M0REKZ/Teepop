#ifndef __FOO_SMARTPTR_H__
#define __FOO_SMARTPTR_H__

#include <foo/base/base.h>
#include <foo/base/log.h>
/*
//
//
//
template<class T>
class TPtr
{
	T *m_pObject;
	int32_t *m_pCounter;
	
	void PtrInit()
	{
		m_pObject = 0;
		m_pCounter = 0;
	}

	void PtrRelease()
	{
		if(m_pObject)
		{
			(*m_pCounter)--;
			if(m_pCounter == 0)
			{
				delete m_pObject;
				delete m_pCounter;
				m_pObject = 0;
				m_pCounter = 0;
			}
		}
	}

	void PtrBind(T *pObject, int32_t *pCounter=NULL)
	{
		PtrRelease();
		m_pObject = pObject;
		m_pCounter = pCounter;
		if(m_pObject)
		{
			if(!m_pCounter)
			{
				m_pCounter = new int32_t;
				(*m_pCounter) = 1;
			}
			else
				(*m_pCounter)++;
		}
	}

	void PtrBind(const TPtr &rPtr)
	{
		PtrBind(rPtr.m_pObject, rPtr.m_pCounter);
	}


public:
	TPtr() { PtrInit(); }
	TPtr(T *pPtr) { PtrInit(); PtrBind(pPtr); }
	TPtr(const TPtr &rPtr) { PtrInit(); PtrBind(rPtr); }
	~TPtr() { PtrRelease(); }

	bool operator !=(const TPtr &spPtr) { return m_pObject != spPtr.m_pObject; }

	bool operator !=(const T *pPtr)
	{
		return m_pObject != pPtr;
	}

	bool operator ==(const TPtr &spPtr)
	{
		return m_pObject == spPtr.m_pObject;
	}

	bool operator ==(const T *pPtr)
	{
		return m_pObject == pPtr;
	}

	//operator T*() { return m_pObject; }
	T &operator*() { return *m_pObject; }
	T *operator->() { return m_pObject; }
	TPtr &operator =(TPtr rPtr) { PtrBind(rPtr); return *this; }
	TPtr &operator =(T *pPtr) { PtrBind(pPtr); return *this; }
};
*/


template<class T>
class TPtr
{
	T *m_pObject;

	void ReleaseObject()
	{
		if(m_pObject)
		{
			m_pObject->m_RefCount--;
			if(m_pObject->m_RefCount == 0)
			{
				m_pObject->m_Deleting = true;
				delete m_pObject;
			}
		}
		m_pObject = NULL;
	}

	void BindObject(T *_pObject)
	{
		ReleaseObject();
		if(_pObject)
		{
			_pObject->m_RefCount++;
			//_pObject->IncreaseRef((void**)&m_pObject);
			m_pObject = _pObject;
		}
	}

public:
	TPtr()
	{
		m_pObject = NULL;
	}

	TPtr(const TPtr &_Ptr)
	{
		m_pObject = NULL;
		BindObject(_Ptr.m_pObject);
	}

	TPtr(T *_pObject)
	{
		m_pObject = NULL;
		BindObject(_pObject);
	}

	~TPtr()
	{
		ReleaseObject();
	}

	bool operator !=(const TPtr &_spPtr)
	{
		return m_pObject != _spPtr.m_pObject;
	}

	bool operator !=(const T *_pPtr)
	{
		return m_pObject != _pPtr;
	}

	bool operator ==(const TPtr &_spPtr)
	{
		return m_pObject == _spPtr.m_pObject;
	}

	bool operator ==(const T *_pPtr)
	{
		return m_pObject == _pPtr;
	}

	//operator T*() { return m_pObject; }
	T &operator*() { return *m_pObject; }
	T *operator->() { return m_pObject; }
	TPtr &operator =(TPtr _Ptr)
	{
		BindObject(_Ptr.m_pObject);
		return *this;
	}

	TPtr &operator =(T *_Ptr)
	{
		BindObject(_Ptr);
		return *this;
	}
};

//
//
//
class CGCObject
{
	//friend template<class T> class TPtr<T>;
	
	template<class T> friend class TPtr;
	bool m_Deleting;
	int32_t m_RefCount;

protected:
	virtual ~CGCObject()
	{
		//NFoo::NLog::WriteF("--- deleting %x\n", this);
		if(m_RefCount != 0 || !m_Deleting)
			NFoo::NLog::WriteF("!!! Unautharized deletion of %x\n", this);
	}

public:
	CGCObject()
	{
		m_RefCount = 0;

		//NFoo::NLog::WriteF("+++ created %x\n", this);
		m_Deleting = false;
	}

};

/*
class CSmartRefCounter
{
	TDynamicList<CSmartRefCounter**> m_lPointers;
public:
	CSmartRefCounter()
	{
		m_RefCount = 0;
	}

	~CSmartRefCounter()
	{

	}

	void IncreaseRef(CSmartRefCounter **_pSmartPtr)
	{
		m_lPointers.Add(_pSmartPtr);
		m_RefCount++;
	}

	void DecreaseRef(CSmartRefCounter **_pSmartPtr)
	{
		m_lPointers.Remove(_pSmartPtr);
		m_RefCount--;
		if(m_RefCount <= 0)
			delete this;
	}

	void ReleasePointers

	int32_t m_RefCount;
};*/
/*
template<class T>
T*operator=(T *a, TPtr<T> b)
{
	a->YOU_HAVE_DONE_WRONG = b;
}*/

#endif // __FOO_SMARTPTR_H__
