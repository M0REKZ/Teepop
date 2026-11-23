#ifndef __FOO_LWO_H__
#define __FOO_LWO_H__

#include <foo/base/base.h>
#include <foo/func/dynamicarray.h>
#include <foo/func/vector2.h>
#include <foo/func/vector3.h>
#include <foo/func/vector4.h>
#include <foo/func/storage.h>

namespace NFoo
{
	namespace NLWO
	{
		//
		//
		class CLWO_Chunk;
		class CLWO_Block;
		class CLWO_File;
		class CLWO_FaceBuildData;

		struct SLWO_PointInfo
		{
			int32 m_FaceIndex;
			int32 m_VertexIndex;
		};

		//
		//
		class CLWO_FaceBuildData
		{
		public:
			~CLWO_FaceBuildData();
			int32 m_VertexIndex[3];
			TDynamicArray<TVector3<fp32> > m_lVertices;
			TDynamicArray<TVector2<fp32> > m_lTVertices;
			TDynamicArray<TVector3<fp32> > m_lNVertices;

			bool m_Valid;
			int32 m_Color;
			int32 m_Surf;
		};

		//
		//
		class CLWO_SurfaceData
		{
		public:
			int8 Name[512];
			int8 FileName[512];
			int8 UVMapname[512];
			int32 m_SurfaceIDNumber;
			int32 m_LWOIDNumber;
		};


		//
		//
		class CLWO_Chunk
		{
		public:
			CLWO_Chunk(CLWO_Block *_pBlock)
			{
				m_pBlock = _pBlock;
			}

			int m_Size;
			int m_Type;
			int m_SubType;
			uint8* m_pBuffer;

			CLWO_Block *m_pBlock;
			void Process(int32 _Phase, CLWO_Block* _pBlock);
		};

		//
		//
		class CLWO_Block
		{
		public:
			CLWO_Block(CLWO_File* _File);
			~CLWO_Block();
			TDynamicArray<CLWO_FaceBuildData*> m_lFaceBuildData;
			TDynamicArray<CLWO_Chunk*> m_lChunks;
			
			CLWO_File *m_pFile;

			int32 m_NumPolygons;
			int32 m_NumPoints;
			
			TDynamicArray<SLWO_PointInfo> *m_pPointInfo;

			int32 Load(uint8* _pBuffer, int32 _Size);
			void Process(int32 _Phase);
			void CalculateNormals();
			void Build();
		};

		//
		//
		class CLWO_File
		{
		public:
			~CLWO_File();
			TDynamicArray<CLWO_SurfaceData*> m_lSurfaces;
			TDynamicArray<CLWO_Block*> m_lBlocks;
			TDynamicArray<int8 *> m_lTags;

			void Load(uint8* _pBuffer, int32 _Size);
			void Process();
			void Build();

			//int32 m_MorphTargets;
		};

		bool Load(CStorage &_Storage, CLWO_File &_rFile);
	}
} // namespace end

#endif // __FOO_LWO_H__

