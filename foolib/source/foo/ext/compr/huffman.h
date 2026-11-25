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
			CEntry(int32_t Alpha, int32_t Code, int32_t CodeSize);

			int32_t m_Alpha;
			int32_t m_Code;
			int32_t m_CodeSize;

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
					int32_t m_Alpha;
				};
			};
		};

		CNode *m_pFirstNode;*/

		// { End, Alpha/Zero, XXX/One }
		int32_t m_aTree[1024*4];
		int32_t m_TreeEnd;
		int32_t *m_pTreeEnd;

		CHuffmanAlphabet();
		~CHuffmanAlphabet();

		int32_t m_LargestCodeSize;

		void Clear();
		void Add(int32_t Alpha, int32_t CodeSize, int32_t Code=-1);
		void CalculateCodes();
		void ReverseCodes();
		
		void ConstructTree();
//		void AddCode(int32_t Alpha, int32_t CodeSize, int32_t Code);
		void AddCodeO(int32_t Alpha, int32_t CodeSize, int32_t Code);

		//
		#ifdef HUFFMAN_DEBUG
			void LogTimes();
			int64_t m_Time_CalculateCodes;
			int64_t m_Time_Decode;
		#endif

		//
		// This needs to be templated because we can have diffrent bitstreams
		//
		template<class T>
		int32_t Decode(T &rBitStream) { return DecodeO(rBitStream); }

		template<class T>
		int32_t DecodeO(T &rBitStream)
		{
			int32_t Pos = 0;
			HUFFMAN_DEBUG_F(int64_t Start = GetTickCount();)
			while(1)
			{
				if(m_aTree[Pos])
				{
					HUFFMAN_DEBUG_F(m_Time_Decode += GetTickCount()-Start;)
					return m_aTree[Pos+1];
				}

				int32_t bit = rBitStream.Read(1);
				if(bit)
					Pos = m_aTree[Pos+2];
				else
					Pos = m_aTree[Pos+1];
			}
			DebugBreak();
			return -1;
			
		}

		//+KZ: seems unused and was giving build errors
		/*template<class T>
		int32_t DecodeT(T &rBitStream)
		{
			rBitStream.Peek(16);
			CNode *pNode = m_pFirstNode;
			HUFFMAN_DEBUG_F(int64_t Start = GetTickCount();)
			while(pNode)
			{
				if(pNode->m_End)
				{
					HUFFMAN_DEBUG_F(m_Time_Decode += GetTickCount()-Start;)
					return pNode->m_Alpha;
				}

				int32_t bit = rBitStream.Read(1);
				if(bit)
					pNode = pNode->m_pOne;
				else
					pNode = pNode->m_pZero;
			}
			DebugBreak();
			return -1;
		}*/

		template<class T>
		int32_t DecodeL(T &rBitStream)
		{
			HUFFMAN_DEBUG_F(int64_t Start = GetTickCount();)
			for(int32_t i = 0; i < m_lEntries.Len(); i++)
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
