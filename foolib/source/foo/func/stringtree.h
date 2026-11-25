#ifndef FILE_FOO_FUNC_STRINGTREE_H
#define FILE_FOO_FUNC_STRINGTREE_H

#include <foo/base/base.h>
#include "dynamicarray.h"
#include "stream.h"

namespace NFoo {

//
// The string tree class is devided into two groups.
// First the base and then the templated wrapper class.
// It's done this way because putting all the CStringTreeBase code into
// a templated class would generate a large amount of data in the code segment.
//
// The current implementation does not allow deletion of nodes.
//
class CStringTreeBase
{
protected:
	typedef uint16 NodeIndex;

	enum // special node indexs
	{
		ERootNode=0,
		ENullNode=(1<<(sizeof(NodeIndex)*8))-1
	};

	bool NodeValid(NodeIndex iNode) { return iNode != ENullNode; }

	// constructor destructor
	CStringTreeBase();
	~CStringTreeBase();

	void Dump(COutputStream &rStream);
	bool Save(COutputStream &rStream);
	bool Load(CInputStream &rStream);

	NodeIndex AddNode(const int8 *pString);
	NodeIndex GetNode(const int8 *pString);
	int32_t NodeCount() { return m_lNodes.Len(); }

	//
	virtual NodeIndex NewNode(const int8 *pString);
	virtual void NodeDataTransfer(NodeIndex iDst, NodeIndex iSrc) {}
private:
	void Clear();

	// Node in the tree
	class CNode
	{
	public:
		CNode()
		{
			m_iChild = ENullNode;
			m_iSibling = ENullNode;
		}

		NodeIndex m_iChild;
		NodeIndex m_iSibling; // 4
	};

	// when loading an string tree from disk, m_lNodeNames
	// points into addresses in the m_pStringData. It's
	// done this way so the memory wont get fragmentated.
	TDynamicArray<CNode> m_lNodes;
	TDynamicArray<int8*> m_lNodeNames;
	int8 *m_pStringData;

	// small helper functions for digging out node info
	CNode &Node(NodeIndex iNode) { return m_lNodes[iNode]; }
	NodeIndex &NodeSibling(NodeIndex iNode) { return m_lNodes[iNode].m_iSibling; }
	NodeIndex &NodeChild(NodeIndex iNode) { return m_lNodes[iNode].m_iChild; }
	int8 *NodeName(NodeIndex iNode) { return m_lNodeNames[iNode]; }

	void NodeSetName(NodeIndex iNode, const int8 *pName, int32_t Len=-1);
	void NodeAddChild(NodeIndex iParent, NodeIndex iChild);
	void NodeDump(NodeIndex iNode, COutputStream &rStream, int32_t Indent=0);
};

//
// this is the templated wrapper discussed above
//
template<typename T>
class TStringTree : private CStringTreeBase
{
	TDynamicArray<T> m_lNodeData;

	virtual NodeIndex NewNode(const int8 *pString)
	{
		m_lNodeData.Add(T());
		return CStringTreeBase::NewNode(pString);
	}

	virtual void NodeDataTransfer(NodeIndex iDst, NodeIndex iSrc)
	{
		m_lNodeData[iDst] = m_lNodeData[iSrc];
	}

public:
	TStringTree()
	{
		m_lNodeData.Add(T()); // add root
	}

	void Dump(COutputStream &rStream)
	{
	}

	//
	// Saves the tree and values to stream
	//
	bool Save(COutputStream &rStream)
	{
		if(!CStringTreeBase::Save(rStream))
			return false;
		rStream.Write(&m_lNodeData[0], sizeof(T)*m_lNodeData.Len());
		return true;
	}

	//
	// Load the tree and values from the stream
	//
	bool Load(CInputStream &rStream)
	{
		if(!CStringTreeBase::Load(rStream))
			return false;
		m_lNodeData.SetLen(NodeCount());
		rStream.Read(&m_lNodeData[0], sizeof(T)*m_lNodeData.Len());
		return true;
	}

	//
	// Get a value from a string
	//
	T Get(const int8 *pString, T Default=0)
	{
		NodeIndex iNode = GetNode(pString);
		if(!NodeValid(iNode))
			return Default;
		return m_lNodeData[iNode];
	}

	//
	// Adds an item to the tree
	//
	bool Add(const int8 *pString, T Item)
	{
		NodeIndex iNode = AddNode(pString);
		m_lNodeData[iNode] = Item;
		return iNode;
	}
};

};

#endif // FILE_FOO_FUNC_STRINGTREE_H

