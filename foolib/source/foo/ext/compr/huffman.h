#ifndef __FOO_HUFFMAN_H__
#define __FOO_HUFFMAN_H__

#include <foo/func/dynamicarray.h>

//#define HUFFMAN_DEBUG
#ifdef HUFFMAN_DEBUG
	#define HUFFMAN_DEBUG_F(x) x
#else
	#define HUFFMAN_DEBUG_F(x)
#endif

namespace NFoo
{
	//
	// Need of serious speed improvements
	//
	class CHuffmanAlphabet
	{
	public:
		class CEntry
		{
		public:
			CEntry();
			CEntry(CEntry &rEntry);
			CEntry(int32 Alpha, int32 Code, int32 CodeSize);

			int32 m_Alpha;
			int32 m_Code;
			int32 m_CodeSize;

			bool operator <(const CEntry &rEntry);
		};

		TDynamicArray<CEntry> m_lEntries;
/*
		class CNode
		{
		public:
			CNode()
			{
				m_End = false;
				m_pZero = NULL;
				m_pOne = NULL;
			}

			~CNode()
			{
				if(!m_End)
				{
					SafeDelete(m_pZero);
					SafeDelete(m_pOne);
				}
			}

			bool m_End;
			union
			{
				struct
				{
					CNode *m_pZero;
					CNode *m_pOne;
				};
				struct
				{
					int32 m_Alpha;
				};
			};
		};

		CNode *m_pFirstNode;*/

		// { End, Alpha/Zero, XXX/One }
		int32 m_aTree[1024*4];
		int32 m_TreeEnd;
		int32 *m_pTreeEnd;

		CHuffmanAlphabet();
		~CHuffmanAlphabet();

		int32 m_LargestCodeSize;

		void Clear();
		void Add(int32 Alpha, int32 CodeSize, int32 Code=-1);
		void CalculateCodes();
		void ReverseCodes();
		
		void ConstructTree();
//		void AddCode(int32 Alpha, int32 CodeSize, int32 Code);
		void AddCodeO(int32 Alpha, int32 CodeSize, int32 Code);

		//
		#ifdef HUFFMAN_DEBUG
			void LogTimes();
			int64 m_Time_CalculateCodes;
			int64 m_Time_Decode;
		#endif

		//
		// This needs to be templated because we can have diffrent bitstreams
		//
		template<class T>
		int32 Decode(T &rBitStream) { return DecodeO(rBitStream); }

		template<class T>
		int32 DecodeO(T &rBitStream)
		{
			int32 Pos = 0;
			HUFFMAN_DEBUG_F(int64 Start = GetTickCount();)
			while(1)
			{
				if(m_aTree[Pos])
				{
					HUFFMAN_DEBUG_F(m_Time_Decode += GetTickCount()-Start;)
					return m_aTree[Pos+1];
				}

				int32 bit = rBitStream.Read(1);
				if(bit)
					Pos = m_aTree[Pos+2];
				else
					Pos = m_aTree[Pos+1];
			}
			DebugBreak();
			return -1;
			
		}

		template<class T>
		int32 DecodeT(T &rBitStream)
		{
			rBitStream.Peek(16);
			CNode *pNode = m_pFirstNode;
			HUFFMAN_DEBUG_F(int64 Start = GetTickCount();)
			while(pNode)
			{
				if(pNode->m_End)
				{
					HUFFMAN_DEBUG_F(m_Time_Decode += GetTickCount()-Start;)
					return pNode->m_Alpha;
				}

				int32 bit = rBitStream.Read(1);
				if(bit)
					pNode = pNode->m_pOne;
				else
					pNode = pNode->m_pZero;
			}
			DebugBreak();
			return -1;
		}

		template<class T>
		int32 DecodeL(T &rBitStream)
		{
			HUFFMAN_DEBUG_F(int64 Start = GetTickCount();)
			for(int32 i = 0; i < m_lEntries.Len(); i++)
			{
				CEntry &rEntry = m_lEntries[i];
				if(rBitStream.Peek(rEntry.m_CodeSize) == rEntry.m_Code)
				{
					rBitStream.Discard(rEntry.m_CodeSize);
					HUFFMAN_DEBUG_F(m_Time_Decode += GetTickCount()-Start;)
					return rEntry.m_Alpha;
				}
			}
			throw "Coudn't find huffman entry";
		}
	};
}

#endif // __FOO_HUFFMAN_H__
