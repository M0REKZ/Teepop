
	typedef uint16 ObjectIndex;
	enum
	{
		EInvalidObject = sizeof(ObjectIndex)-1
	};

	//
	//
	//
	class CObject
	{
		ObjectIndex m_iObject;
	protected:
		CCore *m_pCore;
	public:
		ObjectIndex Index() { return m_iObject; }

		virtual void OnRefresh() {}
		virtual void OnRender() {}

		virtual void OnPack(COutputStream &rStream) {}; // used for network and saveing?
		virtual void OnUnpack(CInputStream &rStream) {};
	};

	//
	//
	//
	class CObjects : public CModule
	{
		TDynamicArray<ObjectIndex> m_lFreeIndecies;
		TDynamicArray<CObject*> m_lObjects;
		int32 m_NumObjects;

		ObjectIndex NewObject()
		{
			ObjectIndex iObject = EInvalidObject;
			if(m_lFreeIndecies.Len())
			{
				iObject = m_lFreeIndecies[0];
				m_lFreeIndecies.RemoveByIndexFast(0);
			}
			else
				iObject = m_lObjects.Add(NULL);

			Assert(iObject < m_lObjects.Len());
			return iObject;
		}

	public:
		//# Function: AddObject
		//# Description: Adds an object into the core.
		//# Declaration:
		ObjectIndex AddObject(CObject *pObj)
		{
			ObjectIndex Index = NewObject();
			m_lObjects[Index] = pObj;
			m_NumObjects++;
			return Index;
		}

		//# Function: RemoveObject (1)
		//# Description: Removes an object into the core.
		//# Declaration:
		void RemoveObject(ObjectIndex iObject)
		{
			Assert(iObject < m_lObjects.Len());
			m_lObjects[iObject] = NULL;
			m_NumObjects--;
		}

		//# Function: RemoveObject (2)
		//# Description: Removes an object into the core.
		//# Declaration:
		void RemoveObject(CObject *pObject)
		{
			Assert(pObject);
			RemoveObject(pObject->Index());
		}
	};
	MACRO_REGISTER_CLASS(CObjects, "EngineModule")
