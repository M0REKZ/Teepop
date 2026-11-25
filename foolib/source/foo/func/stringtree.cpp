#include <foo/base/string.h>
#include <foo/base/memory.h>
#include <foo/base/log.h>
#include "stringtree.h"

using namespace NFoo;

//
//
//
CStringTreeBase::CStringTreeBase()
{
	m_pStringData = 0;
	NewNode(0); // create root node
}

//
//
//
CStringTreeBase::~CStringTreeBase()
{
	Clear();
}

//
//
//
void CStringTreeBase::NodeSetName(NodeIndex iNode, const char *pName, int32_t Len)
{
	SafeArrayDelete(m_lNodeNames[iNode]);
	if(Len < 0)
		Len = StringLength(pName);
	m_lNodeNames[iNode] = new char [Len+1];
	MemoryCopy(m_lNodeNames[iNode], pName, Len+1);
	m_lNodeNames[iNode][Len] = 0;
}

//
//
//
CStringTreeBase::NodeIndex CStringTreeBase::NewNode(const char *pName)
{
	if(m_pStringData)
	{
		// must convert
		throw "Don't DO that!";
	}

	CNode Node;
	NodeIndex iNode = m_lNodes.Add(Node);
	m_lNodeNames.Add(0);
	if(pName)
		NodeSetName(iNode, pName);
	return iNode; 
}

//
//
//
void CStringTreeBase::NodeAddChild(NodeIndex iParent, NodeIndex iChild)
{
	NodeSibling(iChild) = NodeChild(iParent);
	NodeChild(iParent) = iChild;
}

//
//
//
void CStringTreeBase::NodeDump(NodeIndex iNode, COutputStream &rStream, int32_t Indent)
{
	static char aIndent[] = "  ";
	static char aEnd[] = "\n";
	for(int32_t i = 0; i < Indent; i++)
		rStream.Write(aIndent, StringLength(aIndent));
	if(NodeName(iNode))
		rStream.Write(NodeName(iNode), StringLength(NodeName(iNode)));
	rStream.Write(aEnd, StringLength(aEnd));

	iNode = NodeChild(iNode);
	while(NodeValid(iNode))
	{
		NodeDump(iNode, rStream, Indent+1);
		iNode = NodeSibling(iNode);
	}
}

//
//
//
void CStringTreeBase::Clear()
{
	if(m_pStringData)
		SafeArrayDelete(m_pStringData);
	else
	{
		for(int32_t i = 0; i < m_lNodeNames.Len(); i++)
			delete [] m_lNodeNames[i];
	}
	m_lNodeNames.SetLen(0);
	m_lNodes.SetLen(0);
}

//
//
//
void CStringTreeBase::Dump(COutputStream &rStream)
{
	NodeDump(ERootNode, rStream);
	//NLog::WriteF("%d nodes, %d, %d bytes\n", g_NumNodes, sizeof(CNode), g_NumNodes*sizeof(CNode));
}

//
//
//
bool CStringTreeBase::Save(COutputStream &rStream)
{
	rStream.WriteBE<int32_t>(m_lNodes.Len());
	rStream.Write(&m_lNodes[0], sizeof(CNode)*m_lNodes.Len());
	int32_t StringDataSize = 0;
	for(int32_t i = 0; i < m_lNodes.Len(); i++)
	{
		if(m_lNodeNames[i])
			StringDataSize += StringLength(m_lNodeNames[i])+1;
		else
			StringDataSize++;
	}

	rStream.WriteBE<int32_t>(StringDataSize);
	for(int32_t i = 0; i < m_lNodes.Len(); i++)
	{
		if(m_lNodeNames[i])
			rStream.Write(m_lNodeNames[i], StringLength(m_lNodeNames[i])+1);
		else
			rStream.WriteNE<uint8_t>(0);
	}
	return true;
}

//
//
//
bool CStringTreeBase::Load(CInputStream &rStream)
{
	Clear();
	int32_t Len = rStream.ReadBE<int32_t>();
	m_lNodes.SetLen(Len);
	m_lNodeNames.SetLen(Len);
	rStream.Read(&m_lNodes[0], sizeof(CNode)*m_lNodes.Len());

	Len = rStream.ReadBE<int32_t>();
	m_pStringData = new char [Len];
	rStream.Read(m_pStringData, Len);

	char *pCurrent = m_pStringData;
	for(int32_t i = 0; i < m_lNodeNames.Len(); i++)
	{
		m_lNodeNames[i] = pCurrent;
		pCurrent += StringLength(pCurrent)+1;
	}

	return true;
}

//
//
//
CStringTreeBase::NodeIndex CStringTreeBase::GetNode(const char *pString)
{
	NodeIndex iCurrent = ERootNode;
	const char *pName = pString;
	int32_t NameLen = StringLength(pName);

	NodeIndex iBestMatch = ENullNode;
	int32_t BestMatchLen = 0;

	while(NodeValid(iCurrent))
	{
		CNode &rNode = Node(iCurrent);
		int32_t NodeLen = 0;
		const char *pNodeName = NodeName(iCurrent);
		if(pNodeName)
			NodeLen = StringLength(pNodeName);

		int32_t Len =  NodeLen < NameLen ? NodeLen : NameLen; 

		if(StringCompare(pNodeName, pName, Len) == 0)
		{
			// decend
			if(NodeValid(rNode.m_iChild))
			{
				iCurrent = rNode.m_iChild;
				pName += Len;
				NameLen -= Len;
				continue;
			}

			return iCurrent;
		}
		iCurrent = rNode.m_iSibling;
	}

	return ENullNode;
}

//
//
//
CStringTreeBase::NodeIndex CStringTreeBase::AddNode(const char *pString)
{
	NodeIndex iCurrent = ERootNode;
	const char *pName = pString;
	int32_t NameLen = StringLength(pName);

	NodeIndex iBestMatch = ENullNode;
	int32_t BestMatchLen = 0;

	while(1)
	{
		const char *pNodeName = NodeName(iCurrent);
		int32_t NodeLen = 0;
		if(pNodeName)
			NodeLen = StringLength(pNodeName);

		// find matching length
		int32_t Len =  NodeLen < NameLen ? NodeLen : NameLen; 
		int32_t MatchLength = 0;
		for(; MatchLength < Len; MatchLength++)
			if(pNodeName[MatchLength] != pName[MatchLength])
				break;

		if(MatchLength == NodeLen)
		{
			if(NodeValid(NodeChild(iCurrent)))
			{
				// decend
				iBestMatch = iCurrent;
				iCurrent = NodeChild(iCurrent);
				pName += MatchLength;
				NameLen -= MatchLength;
				BestMatchLen = 0;
				continue;
			}
			else
			{
				// create new node
				iBestMatch = iCurrent;
				iCurrent = ENullNode;
			}
		}
		else if(MatchLength > BestMatchLen)
		{
			// found good match
			iBestMatch = iCurrent;
			BestMatchLen = MatchLength;
		}

		if(NodeValid(iCurrent) && NodeValid(NodeSibling(iCurrent)))
			iCurrent = NodeSibling(iCurrent); // advance to next node
		else
		{
			if(!NodeValid(iBestMatch))
				iBestMatch = ERootNode;

			if(NodeName(iBestMatch) && BestMatchLen != 0)
			{
				// split up best node and create a new for this string
				NodeIndex iOld = NewNode(NodeName(iBestMatch)+BestMatchLen); // an allocation can happen here
				NodeDataTransfer(iOld, iBestMatch);
				NodeChild(iOld) = NodeChild(iBestMatch);
				NodeChild(iBestMatch) = ENullNode;
				
				NodeSetName(iBestMatch, pName, BestMatchLen);
				NodeAddChild(iBestMatch, iOld);
			}

			// create new node and link it in
			NodeIndex iNew = NewNode(pName+BestMatchLen);
			NodeAddChild(iBestMatch, iNew);
			return iNew;
		}
	}
}
