
#include <foo/func/bitstream.h>
#include <foo/base/log.h>
#include "huffman.h"

using namespace NFoo;

//
//
//
int32 ReverseBits(int32 Bits, int32 nBits)
{
	int32 NewBits = 0;
	for(int32 b = 0; b < nBits; b++)
		NewBits |= ((Bits>>b)&1)<<(nBits-b-1);
	return NewBits;
}

//
// Bubblesort for eXtresme sp33ds!!!?!!
//
template<class T>
void SortArray_Bubble(TDynamicArray<T> &lArray)
{
	bool Changing = true;
	while(Changing)
	{
		Changing = false;
		for(int32 i = 1; i < lArray.Len(); i++)
		{
			if(lArray[i] < lArray[i-1])
			{
				T temp = lArray[i];
				lArray[i] = lArray[i-1];
				lArray[i-1] = temp;
				Changing = true;
			}
		}
	}
}


//
//
//
CHuffmanAlphabet::CEntry::CEntry()
{
}

//
//
//
CHuffmanAlphabet::CEntry::CEntry(CEntry &rEntry)
{
	*this = rEntry;
}

//
//
//
CHuffmanAlphabet::CEntry::CEntry(int32 Alpha, int32 Code, int32 CodeSize)
{
	m_Alpha = Alpha;
	m_Code = Code;
	m_CodeSize = CodeSize;
}

//
//
//
bool CHuffmanAlphabet::CEntry::operator <(const CEntry &rEntry)
{
	if(m_CodeSize < rEntry.m_CodeSize)
		return true;
	else if(m_CodeSize == rEntry.m_CodeSize)
	{
		if(m_Alpha < rEntry.m_Alpha)
			return true;
	}

	return false;
}

//
//
//
CHuffmanAlphabet::CHuffmanAlphabet()
{
//	m_pFirstNode = 0;
	Clear();

#ifdef HUFFMAN_DEBUG
	m_Time_CalculateCodes = 0;
	m_Time_Decode = 0;
#endif
}

//
//
//
CHuffmanAlphabet::~CHuffmanAlphabet()
{
	//SafeDelete(m_pFirstNode);
}

//
//
//
#ifdef HUFFMAN_DEBUG
void CHuffmanAlphabet::LogTimes()
{
	NLog::WriteF("calculatecodes=%.3f decode=%.3f\n",
					m_Time_CalculateCodes/(fp32)GetTickFrequency(), m_Time_Decode/(fp32)GetTickFrequency());
}
#endif

//
//
//
void CHuffmanAlphabet::Clear()
{
	m_lEntries.Clear();
	m_LargestCodeSize = 0;
}

//
//
//
void CHuffmanAlphabet::Add(int32 Alpha, int32 CodeSize, int32 Code)
{
	CEntry Entry(Alpha, Code, CodeSize);
	m_lEntries.Add(Entry);
}

//
//
//
void CHuffmanAlphabet::CalculateCodes()
{
#ifdef HUFFMAN_DEBUG
	int64 Start = GetTickCount();
#endif
	SortArray_Bubble(m_lEntries);

	int32 CodeSize = 1;
	int32 Code = 0;
	for(int32 i = 0; i < m_lEntries.Len(); i++)
	{
		CEntry &rEntry = m_lEntries[i];
		Code <<= rEntry.m_CodeSize-CodeSize;
		CodeSize = rEntry.m_CodeSize;
		rEntry.m_Code = Code;
		Code++;
	}

	ReverseCodes();
	CHuffmanAlphabet::ConstructTree();

#ifdef HUFFMAN_DEBUG
	m_Time_CalculateCodes += GetTickCount()-Start;
#endif
}


//
//
//
void CHuffmanAlphabet::ConstructTree()
{
	//SafeDelete(m_pFirstNode);
	m_TreeEnd = 3;
	m_pTreeEnd = m_aTree+3;
	m_aTree[0] = 0;
	m_aTree[1] = 0;
	m_aTree[2] = 0;

	for(int32 i = m_lEntries.Len()-1; i >= 0 ; i--)
	{
		AddCodeO(m_lEntries[i].m_Alpha, m_lEntries[i].m_CodeSize, m_lEntries[i].m_Code);
	}

	/*
	int32 aStackPos[32];
	int32 aStackCode[32];
	int32 Depth = 0;
	int32 Alpha, CodeSize, Code;
	aStackPos[0] = 0;
	aStackCode[0] = 0;
	for(int32 i = 0; i < m_lEntries.Len(); i++)
	{
		Alpha = m_lEntries[i].m_Alpha;
		CodeSize = m_lEntries[i].m_CodeSize;
		Code = m_lEntries[i].m_Code;

		// backup
		if(Depth > CodeSize-1)
			Depth = CodeSize-1;

		while(Depth && aStackCode[Depth] != (Code&((1<<Depth)-1)))
			Depth--;


		// traverse
		Code >>= Depth;
		int32 Pos = aStackPos[Depth];
		int32 CurrentCode = aStackCode[Depth];
		while(1)
		{
			if(Depth == CodeSize)
			{
				//if(pNode->m_End || pNode->m_pZero || pNode->m_pOne)
				//	DebugBreak();
				m_aTree[Pos] = 1;
				m_aTree[Pos+1] = Alpha;
				m_aTree[Pos+2] = 0;
				break;
			}
			else if(m_aTree[Pos])
				DebugBreak();

			int32 *NextPos = &m_aTree[Pos+1];
			//CNode **pAppendNode = &pNode->m_pZero;
			if(Code&1)
			{
				CurrentCode |= (1<<(Depth));
				NextPos = &m_aTree[Pos+2];
			}
				//pAppendNode = &pNode->m_pOne;
			if(!(*NextPos))
			{
				(*NextPos) = m_TreeEnd;
				m_aTree[m_TreeEnd] = 0;
				m_aTree[m_TreeEnd+1] = 0;
				m_aTree[m_TreeEnd+2] = 0;
				m_TreeEnd+=3;
			}
			Pos = *NextPos;
			Depth++;
			aStackPos[Depth] = Pos;
			aStackCode[Depth] = CurrentCode;
			//	*pAppendNode = new CNode;
			//pNode = *pAppendNode;
			Code>>=1;
			//CodeSize--;
		}
	}*/

}

void CHuffmanAlphabet::AddCodeO(int32 Alpha, int32 CodeSize, int32 Code)
{
	//int32 *pPos = m_aTree;
	int32 Pos = 0;
	while(1)
	{
		if(CodeSize == 0)
		{
			m_aTree[Pos] = 1;
			m_aTree[Pos+1] = Alpha;
			m_aTree[Pos+2] = 0;
			return;
		}

		int32 *NextPos = &m_aTree[Pos+1];
		if(Code&1)
			NextPos = &m_aTree[Pos+2];
		if(!(*NextPos))
		{
			*NextPos = m_TreeEnd;
			m_aTree[m_TreeEnd++] = 0;
			m_aTree[m_TreeEnd++] = 0;
			m_aTree[m_TreeEnd++] = 0;
		}
		Pos = *NextPos;
		Code>>=1;
		CodeSize--;
	}
}

/*
void CHuffmanAlphabet::AddCode(int32 Alpha, int32 CodeSize, int32 Code)
{
	if(!m_pFirstNode)
		m_pFirstNode = new CNode;
	CNode *pNode = m_pFirstNode;
	while(1)
	{
		if(CodeSize == 0)
		{
			if(pNode->m_End || pNode->m_pZero || pNode->m_pOne)
				DebugBreak();
				
			pNode->m_End = true;
			pNode->m_Alpha = Alpha;
			return;
		}
		else if(pNode->m_End)
			DebugBreak();

		CNode **pAppendNode = &pNode->m_pZero;
		if(Code&1)
			pAppendNode = &pNode->m_pOne;
		if(!*pAppendNode)
			*pAppendNode = new CNode;
		pNode = *pAppendNode;
		Code>>=1;
		CodeSize--;
	}
}*/

//
//
//
void CHuffmanAlphabet::ReverseCodes()
{
	for(int32 i = 0; i < m_lEntries.Len(); i++)
		m_lEntries[i].m_Code = ReverseBits(m_lEntries[i].m_Code, m_lEntries[i].m_CodeSize);
}





















