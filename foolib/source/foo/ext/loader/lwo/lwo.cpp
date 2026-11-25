
#include <foo/base/string.h>
#include "lwo.h"

using namespace NFoo;
using namespace NFoo::NLWO;

//
// TEMPORARY FUNCTIONS
//
uint16_t _GET_SWAP_UINT16(uint8_t* _pBuf) 
{ 
	return ByteSwap(*((uint16_t *)_pBuf));
}

uint16_t _GET_SWAP_UINT16_MOVE(uint8_t** _pBuf) 
{ 
	uint16_t r = ByteSwap(*((uint16_t *)_pBuf));
	*_pBuf += 2;
	return r;
}

uint32_t _GET_SWAP_UINT32(uint8_t* _pBuf) 
{ 
	return ByteSwap(*((uint32_t *)_pBuf));
}

fp32 _GET_FLOAT(uint8_t* _pBuf) 
{ 
	uint32_t i = ByteSwap(*((uint32_t *)_pBuf));
	return *((fp32*)&i);
}


uint32_t _GET_VX(uint8_t* _pBuf) 
{ 
	if (*_pBuf == 0xFF)
		return ByteSwap((*((uint32_t *)_pBuf)) & 0xFFFFFF);
	else
		return _GET_SWAP_UINT16(_pBuf);
}

uint32_t _GET_VX_MOVE(uint8_t** _pBuf) 
{ 
	uint32_t r;
	if (**_pBuf == 0xFF)
	{
		r = ByteSwap((*((uint32_t *)*_pBuf)) & 0xFFFFFF);
		*_pBuf += 4;
		return r;
	}
	else
	{
		r = _GET_SWAP_UINT16(*_pBuf);
		*_pBuf += 2;
		return r;
	}
}

uint8_t *_GET_S0_MOVE(uint8_t** _pBuf) 
{ 
	uint8_t *t = *_pBuf;
	int32_t i = StringLength((char *)*_pBuf) + 1;

	if (i%2)
		i++;

	*_pBuf += i;
	return t;
}

/////////////////////////////////////////////////////////////////////////////
//
// Contructors and Destructors
//
//
/////////////////////////////////////////////////////////////////////////////
CLWO_FaceBuildData::~CLWO_FaceBuildData()
{
	m_lVertices.Clear();
	m_lTVertices.Clear();
	m_lNVertices.Clear();
}

CLWO_Block::CLWO_Block(CLWO_File* _File)
{
	m_pFile = _File;
	m_pPointInfo = 0;
}

CLWO_Block::~CLWO_Block()
{
	int32_t i;

	for(i = 0; i < m_lChunks.Len(); i++)
		SafeDelete(m_lChunks[i]);

	for(i = 0; i < m_lFaceBuildData.Len(); i++)
		SafeDelete(m_lFaceBuildData[i]);

	SafeArrayDelete(m_pPointInfo);
}

CLWO_File::~CLWO_File()
{
	int32_t i;

	for(i = 0; i < m_lBlocks.Len(); i++)
		SafeDelete(m_lBlocks[i]);

	for(i = 0; i < m_lSurfaces.Len(); i++)
		SafeDelete(m_lSurfaces[i]);

	m_lTags.Clear();
	//m_MorphTargets = 1;
}

/////////////////////////////////////////////////////////////////////////////
//
// Main Function.
//
/////////////////////////////////////////////////////////////////////////////

//
// void CLWO_Chunk::Process(int _Phase, CLWO_Block* _pBlock)
//
// Processes the chunk
//
void CLWO_Chunk::Process(int32_t _Phase, CLWO_Block* _pBlock)
{
	//
	// Check so we got the right chunk
	//
	if (m_Type != _Phase)
		return;

	//
	// switch to correct chunk handler
	//
	switch(m_Type)
	{
		//
		// PNTS (Point List)
		// -----------------
		//
		// PNTS { point-location[VEC12] * }
		//
		case 'PNTS':
			{
				//
				// Point Chunk - Calculates num points and assigns the pointbuffer
				// float = 4 byte
				// 3 choors per vertex
				// 4 * 3 = 12
				//
				_pBlock->m_NumPoints = m_Size / 12;
				uint8_t* pTempPtr = m_pBuffer;
				fp32 x, y, z;

				//
				// The Pointinfo list is for getting what polygons that have the specifik point
				//
				_pBlock->m_pPointInfo = new TDynamicArray<SLWO_PointInfo>[_pBlock->m_NumPoints];

				for(int32_t f = 0; f < _pBlock->m_NumPolygons; f++)
					for(int32_t i = 0; i < 3; i++)
					{
						SLWO_PointInfo Info;
						Info.m_FaceIndex = f;
						Info.m_VertexIndex = i;

						_pBlock->m_pPointInfo[_pBlock->m_lFaceBuildData[f]->m_VertexIndex[i]].Add(Info);
					}
				//
				// Load Verts
				//
				for(int32_t i = 0; i < _pBlock->m_NumPoints; i++, pTempPtr+=4)
				{	
					//
					// Get Choords
					//
					x = _GET_FLOAT(pTempPtr);
					y = _GET_FLOAT(pTempPtr+=4);
					z = -_GET_FLOAT(pTempPtr+=4);

					for(int32_t v = 0; v < _pBlock->m_pPointInfo[i].Len(); v++)
						_pBlock->m_lFaceBuildData[_pBlock->m_pPointInfo[i][v].m_FaceIndex]->m_lVertices[_pBlock->m_pPointInfo[i][v].m_VertexIndex].Set(x, y, z);
				}
			} break;

		//
		// POLS (Polygon List)
		// -------------------
		//
		// POLS { type[ID4], ( numvert+flags[U2], vert[VX] # numvert )* }
		//
		case 'POLS':
			{
				if(m_SubType == 'FACE')
				{
					//
					// Face Subtype - Contains regular faces
					//
					// Declarations and assignment
					//
					uint16_t NumVertsAndFlags;
					uint16_t Flags;
					uint16_t NumVerts;
					uint8_t* pTempPtr = m_pBuffer + 4;

					// Move thru all the faces
					_pBlock->m_NumPolygons = 0;

					while (pTempPtr < m_pBuffer + m_Size)
					{
						// Read Flags and Num Verts for Polygon
						NumVertsAndFlags = ByteSwap(*((uint16_t *)pTempPtr));
						Flags = NumVertsAndFlags & 0xFC00;
						NumVerts = NumVertsAndFlags - Flags;
						
						pTempPtr += 2;

						CLWO_FaceBuildData* pFaceData = new CLWO_FaceBuildData;
						_pBlock->m_lFaceBuildData.Add(pFaceData);

						// Vertices
						pFaceData->m_lVertices.SetLen(3);
						pFaceData->m_lTVertices.SetLen(3);
						pFaceData->m_lNVertices.SetLen(3);

						//
						// Read Vertecies - They are written with "Variable-length Index" format
						//
						// If the first byte is a 0xFF then 4 byte are used for index
						// else the two byte format should be used. When the 4 byte
						//
						// This rutine can only hande polygons with 3 points
						//
						int32_t i = 0;
						if(NumVerts > 2)
						{
							for (; i < 3; i++)
							{
								pFaceData->m_VertexIndex[i] = _GET_VX(pTempPtr);
								
								pTempPtr += 2;
								if(*pTempPtr == 0xFF)
									pTempPtr = pTempPtr+2;
								//pTempPtr = (uint8_t *)CMovE(*pTempPtr, 0xFF, (long)pTempPtr, (long)(pTempPtr+2));
							}

						}

						for (; i < NumVerts; i++)
						{
							pTempPtr += 2;
							if(*pTempPtr == 0xFF)
								pTempPtr = pTempPtr+2;
							//pTempPtr = (uint8_t *)CMovE(*pTempPtr, 0xFF, (long)pTempPtr, (long)(pTempPtr+2));
						}

						// Only Polygons with 3 points are valid
							// pFaceData->m_Valid = true;
						pFaceData->m_Valid = NumVerts == 3 ? true : false;

						_pBlock->m_NumPolygons++;
					}

				}
			} break;

		//
		// VMAP (Vertex Mapping)
		// ---------------------
		//
		// VMAP { type[ID4], dimension[U2], name[S0], ( vert[VX], value[F4] # dimension )* }
		//
		case 'VMAP':
			{
				uint16_t Dim;
				uint8_t *Name;
				uint8_t *pEndPos = m_pBuffer + m_Size;
				uint8_t *pTempPtr = m_pBuffer + 4;
				
				Dim = _GET_SWAP_UINT16(pTempPtr);
				pTempPtr += 2;
				
				Name = _GET_S0_MOVE(&pTempPtr);
					
				// Check what subchunk it is
				if(m_SubType == 'TXUV')
				{
					//
					// UV Texture map
					//
					// Start to read the data
					//
					while (pTempPtr < pEndPos - 1)
					{
						int32_t id = _GET_VX_MOVE(&pTempPtr);
						fp32 u = _GET_FLOAT(pTempPtr);
						fp32 v = _GET_FLOAT(pTempPtr + 4);
						pTempPtr += 8;
						
						for(int32_t k = 0; k < _pBlock->m_lFaceBuildData.Len(); k++)
						{
							for(int32_t l = 0; l < 3; l++)
							{
								if (_pBlock->m_lFaceBuildData[k]->m_VertexIndex[l] != id)
									continue;

								_pBlock->m_lFaceBuildData[k]->m_lTVertices[l].Set(u, 1 - v);
							}
						}
					}
				}
				/*
				else if(m_SubType == 'MORF')
				{
					//
					// Morph Target
					//
					// Start to read the data
					//
					while (pTempPtr < pEndPos - 1)
					{
						int32_t id = _GET_VX_MOVE(&pTempPtr);
						fp4 x = _GET_FLOAT(pTempPtr);
						fp4 y = _GET_FLOAT(pTempPtr + 4);
						fp4 z = -_GET_FLOAT(pTempPtr + 8);
						pTempPtr += 12;
						
						for(int32_t k = 0; k < _pBlock->m_lFaceBuildData.Len(); k++)
						{
							for(int32_t l = 0; l < 3; l++)
							{
								if (_pBlock->m_lFaceBuildData[k]->m_VertexIndex[l] != id)
									continue;

								_pBlock->m_lFaceBuildData[k]->m_lVertices[l].Set(	_pBlock->m_lFaceBuildData[k]->m_lVertices[l].x + x,
																					_pBlock->m_lFaceBuildData[k]->m_lVertices[l].y + y,
																					_pBlock->m_lFaceBuildData[k]->m_lVertices[l].z + z);
							}
						}
					}
				}
				*/
			} break;

		//
		// VMAD (Discontinuous Vertex Mapping)
		// -----------------------------------
		//
		// VMAD { type[ID4], dimension[U2], name[S0], ( vert[VX], poly[VX], value[F4] # dimension )* } 
		//
		case 'VMAD':
			{
				//
				// Check that we have the correct Subchunk
				//
				if (m_SubType != 'TXUV')
					break;

				uint16_t Dim;
				uint8_t *Name;
				uint8_t *pEndPos = m_pBuffer + m_Size;
				uint8_t *pTempPtr = m_pBuffer + 4;

				Dim = _GET_SWAP_UINT16(pTempPtr);
				pTempPtr += 2;

				//Dim = _GET_SWAP_UINT16(pTempPtr);
				//pTempPtr += 2;

				Name = _GET_S0_MOVE(&pTempPtr);

				while (pTempPtr < pEndPos)
				{
					int32_t vid = _GET_VX_MOVE(&pTempPtr);
					int32_t pid = _GET_VX_MOVE(&pTempPtr);
					fp32 u = _GET_FLOAT(pTempPtr);
					fp32 v = _GET_FLOAT(pTempPtr + 4);
					pTempPtr += 8;

					for(int l = 0; l < 3; l++)
					{
						if (_pBlock->m_lFaceBuildData[pid]->m_VertexIndex[l] != vid)
							continue;

						_pBlock->m_lFaceBuildData[pid]->m_lTVertices[l].Set(u, 1 - v);
					}
				}
			} break;

		//
		// PTAG (Polygon Tag Mapping)
		// --------------------------
		//
		// PTAG { type[ID4], ( poly[VX], tag[U2] )* }
		//
		case 'PTAG':
			{
				//
				// Check that we have the correct Subchunk
				//
				if (m_SubType != 'SURF')
					break;

				uint8_t *pEndPos = m_pBuffer + m_Size;
				uint8_t *pTempPtr = m_pBuffer + 4;

				while(pTempPtr < pEndPos - 1)
				{
					int32_t pid = _GET_VX_MOVE(&pTempPtr);
					int32_t surf = _GET_SWAP_UINT16_MOVE(&pTempPtr);

					for(int32_t j = 0; j < _pBlock->m_pFile->m_lSurfaces.Len(); j++)
					{
						if (_pBlock->m_pFile->m_lSurfaces[j]->m_LWOIDNumber == surf)
							_pBlock->m_lFaceBuildData[pid]->m_Surf = surf;
					}
				}
			} break;
	}
}

//
// Calculate Normals
//
void CLWO_Block::CalculateNormals()
{
	int32_t f, v;

	fp32 relx1, rely1, relz1, relx2, rely2, relz2;
	fp32 len;
	fp32 nx, ny, nz;
	int32_t cn;

	//
	// Build Point to PlyList
	//
	//TFeDynamicList<CVector3D *> *_pPolygons = new TFeDynamicList<CVector3D *>[m_NumPoints];

	//for(f = 0; f < m_NumPolygons; f++)
	//	for(int32_t i = 0; i < 3; i++)
	//		_pPolygons[m_lFaceBuildData[f]->m_VertexIndex[i]].Add(&m_lFaceBuildData[f]->m_lNVertices[i]);

	for (v = 0; v < m_NumPoints; v++)
	{
		nx = ny = nz = 0;
		cn = 0;

		for(f = 0; f < m_pPointInfo[v].Len(); f++)
		{
			CLWO_FaceBuildData *pFaceData = m_lFaceBuildData[m_pPointInfo[v][f].m_FaceIndex];
			relx1 = pFaceData->m_lVertices[1].x - pFaceData->m_lVertices[0].x;
			rely1 = pFaceData->m_lVertices[1].y - pFaceData->m_lVertices[0].y;
			relz1 = pFaceData->m_lVertices[1].z - pFaceData->m_lVertices[0].z;
			relx2 = pFaceData->m_lVertices[2].x - pFaceData->m_lVertices[0].x;
			rely2 = pFaceData->m_lVertices[2].y - pFaceData->m_lVertices[0].y;
			relz2 = pFaceData->m_lVertices[2].z - pFaceData->m_lVertices[0].z;

			nx -= (rely1 * relz2 - relz1 * rely2);
			ny -= (relz1 * relx2 - relx1 * relz2);
			nz -= (relx1 * rely2 - rely1 * relx2);

			cn++;
		}
		/*
		for (f = 0; f < m_NumPolygons; f++)
		{
			if (m_lFaceBuildData[f]->m_VertexIndex[0] == v ||
				m_lFaceBuildData[f]->m_VertexIndex[1] == v ||
				m_lFaceBuildData[f]->m_VertexIndex[2] == v)
			{
				relx1 = m_lFaceBuildData[f]->m_lVertices[1].x - m_lFaceBuildData[f]->m_lVertices[0].x;
				rely1 = m_lFaceBuildData[f]->m_lVertices[1].y - m_lFaceBuildData[f]->m_lVertices[0].y;
				relz1 = m_lFaceBuildData[f]->m_lVertices[1].z - m_lFaceBuildData[f]->m_lVertices[0].z;
				relx2 = m_lFaceBuildData[f]->m_lVertices[2].x - m_lFaceBuildData[f]->m_lVertices[0].x;
				rely2 = m_lFaceBuildData[f]->m_lVertices[2].y - m_lFaceBuildData[f]->m_lVertices[0].y;
				relz2 = m_lFaceBuildData[f]->m_lVertices[2].z - m_lFaceBuildData[f]->m_lVertices[0].z;
				
				nx -= (rely1 * relz2 - relz1 * rely2);
				ny -= (relz1 * relx2 - relx1 * relz2);
				nz -= (relx1 * rely2 - rely1 * relx2);
				
				cn++;
			}
		}
		*/
		
		if (cn != 0)
		{
			nx /= cn;
			ny /= cn;
			nz /= cn;
			
			len = sqrt(nx*nx + ny*ny + nz*nz);

			if (len == 0)
				len = 1;

			nx /= len;
			ny /= len;
			nz /= len;

			for(f = 0; f < m_pPointInfo[v].Len(); f++)
				m_lFaceBuildData[m_pPointInfo[v][f].m_FaceIndex]->m_lNVertices[m_pPointInfo[v][f].m_VertexIndex].Set(nx, ny, nz);

			/*
			for(f = 0; f < m_NumPolygons; f++)
				for(int32_t i = 0; i < 3; i++)
				{
					if (m_lFaceBuildData[f]->m_VertexIndex[i] != v)
						continue;

					m_lFaceBuildData[f]->m_lNVertices[i].Set(nx, ny, nz);
				}
				*/
		}
	}

}

//
// void CLWO_Block::Process(int _Phase)
//
// Just sends the command over to every chunk in the block
//
void CLWO_Block::Process(int32_t _Phase)
{
	for(int32_t i = 0; i < m_lChunks.Len(); i++)
		m_lChunks[i]->Process(_Phase, this);
}

//
//
//
void CLWO_File::Load(uint8_t* _pBuffer, int32_t _Size)
{
	// Local Variabels
	uint32_t*	pDWORDBuffer = (uint32_t*)_pBuffer;
	uint8_t*	pBuffer = (uint8_t*)_pBuffer;

	// Inital format check
	{
		if (_Size < 12)
			return;
		
		if (*pDWORDBuffer != (uint32_t)ByteSwap('FORM'))
			return;
		
		if (pDWORDBuffer[1] != (uint32_t)ByteSwap(_Size-8))
			return;
		
		if (pDWORDBuffer[2] != (uint32_t)ByteSwap('LWO2'))
			return;
	}

	m_lBlocks.Clear();

	//
	// Chunk Reading
	// -----------------------------------------------------------
	// Ok, Let's start reading some chunks of data 
	// and remember that the chunks is 2 byte aligned.
	//
	{
		// Sub-chunk general attributes and chunk start positon holder
		CLWO_Block* pCurrentBlock = NULL;

		// Where begins the SubChunks
		int32_t CurrentPosition = 12;

		// Main Loop
		while(CurrentPosition < _Size)
		{
			int32_t SubChunkID = ByteSwap(*((uint32_t *)&_pBuffer[CurrentPosition]));
			int32_t SubChunkSize = ByteSwap(*((uint32_t *)&_pBuffer[CurrentPosition+4]));
			int32_t SubChunkSubType = ByteSwap(*((uint32_t *)&_pBuffer[CurrentPosition+8]));

			// The PNTS subchunk is the begining of a new block
			if (SubChunkID == 'PNTS')
			{
				pCurrentBlock = new CLWO_Block(this);
				m_lBlocks.Add(pCurrentBlock);
			}
			
			// Check for valid blocks
			if (pCurrentBlock && (
				SubChunkID == 'PNTS' ||
				SubChunkID == 'POLS' ||
				(SubChunkID == 'VMAP' && SubChunkSubType == 'TXUV') ||
				(SubChunkID == 'VMAP' && SubChunkSubType == 'MORF') ||
				(SubChunkID == 'VMAD' && SubChunkSubType == 'TXUV') ||
				(SubChunkID == 'PTAG' && SubChunkSubType == 'SURF')))
			{
				// Create the chunk
				CLWO_Chunk *Chunk = new CLWO_Chunk(pCurrentBlock);
				pCurrentBlock->m_lChunks.Add(Chunk);

				// Fill in the information
				Chunk->m_Size = SubChunkSize;
				Chunk->m_Type = SubChunkID;
				Chunk->m_SubType = SubChunkSubType;
				Chunk->m_pBuffer = _pBuffer + CurrentPosition + 8;
			}
			
			//
			// The TAGS block is a global block for the file.
			//
			// TAGS { tag-string[S0] * }
			//
			if (SubChunkID == 'TAGS')
			{
				m_lTags.Clear();
				uint8_t *pTemp = _pBuffer + CurrentPosition + 8;
				uint8_t *pEnd = _pBuffer + CurrentPosition + SubChunkSize + 8;

				// Process the chunk and add the strings
				while(pTemp < pEnd)
					m_lTags.Add((char *)_GET_S0_MOVE(&pTemp));
			}

			//
			// The SURF block is a global block for the file.
			//
			// SURF { name[S0], source[S0], attributes[SUB-CHUNK] * }
			//
			if (SubChunkID == 'SURF')
			{
				/*
				uint8_t *pTemp = _pBuffer + CurrentPosition + 8;
				int8_t Name[512];
				strcpy(Name, (char *)_GET_S0_MOVE(&pTemp));

				if (strlen(Name) > 0)
				{
					CLWO_SurfaceData *Surf = new CLWO_SurfaceData;
					m_lSurfaces.Add(Surf);
					strcpy(Surf->FileName, (int8_t *)_GET_S0_MOVE(&pTemp));
					strcpy(Surf->Name, Name);
					strcpy(Surf->UVMapname, "");
					
					//
					// Start Digout the VMAP name
					//
					int32_t CurPosition = 0;
					int32_t Size = SubChunkSize-1;

					//
					// Main Loop
					//
					while(CurPosition < Size-4)
					{
						int32_t SubSubChunkID = ByteSwap32(*((uint32_t *)&pTemp[CurPosition]));
						int32_t SubSubChunkSize = ByteSwap16(*((uint16_t *)&pTemp[CurPosition+4]));

						if(SubSubChunkID == 'BLOK')
						{
							pTemp = pTemp+CurPosition+6;
							CurPosition = 0;
							Size = SubSubChunkSize;
						}
						else if(SubSubChunkID == 'VMAP')
						{
							pTemp = pTemp+CurPosition+6;
							strcpy(Surf->UVMapname, (int8_t *)_GET_S0_MOVE(&pTemp));
							Size = 0;
						}
						else
						{
							CurPosition += 6 + SubSubChunkSize;
							if(CurPosition & 1)
								CurPosition++;

						}
					}

					for (int32_t j = 0; j < m_lTags.Len(); j++)
					{
						Surf->m_LWOIDNumber = CMovE(stricmp(m_lTags[j], Name), 0, Surf->m_LWOIDNumber, j);
						//if ( == 0)
						//	Surf->m_LWOIDNumber = j;
					}
				}
				*/
			}

			//
			// Position fix and
			// 2 byte alignment fix
			//
			CurrentPosition += SubChunkSize + 8;
			if(CurrentPosition & 1)
				CurrentPosition++;
		}
	}
}

//
//
void CLWO_File::Process()
{
	//
	// Process
	// -------
	//

	// Count Morphtargets
	{
		int32_t b, c;
			
		// All blocks
		for(b = 0; b < m_lBlocks.Len(); b++)
		{
			int32_t Count = 1;

			// All chunks
			for(c = 0; c < m_lBlocks[b]->m_lChunks.Len(); c++)
			{
				if(m_lBlocks[b]->m_lChunks[c]->m_Type == 'VMAP' && m_lBlocks[b]->m_lChunks[c]->m_SubType == 'MORF')
					Count++;
			}

			//
			// Update
			//
			//if(Count > m_MorphTargets)
			//	m_MorphTargets = Count;
			
		}
	}

	// Do the processing
	int32_t i, n;
	n = m_lBlocks.Len();									//
	for(i = 0; i < n; i++) m_lBlocks[i]->Process('POLS');	// Polygon List 
	for(i = 0; i < n; i++) m_lBlocks[i]->Process('PNTS');	// Point List
	for(i = 0; i < n; i++) m_lBlocks[i]->Process('PTAG');	// Polygon Tag Mapping
	for(i = 0; i < n; i++) m_lBlocks[i]->Process('VMAP');	// Vertex Mapping
	for(i = 0; i < n; i++) m_lBlocks[i]->Process('VMAD');	// Discontinuous Vertex Mapping (Overrides VMAP)
	for(i = 0; i < n; i++) m_lBlocks[i]->CalculateNormals();//
}


//
// LWO2 Converter Code End
//
bool NLWO::Load(CStorage &_Storage, CLWO_File &_rFile)
{
	if(!_Storage.IsReady())
		return false;

	// Get Size
	int32_t Size = _Storage.GetSize();

	// Load File and close
	int8_t *pBuffer = new int8_t[Size];
	_Storage.Read(pBuffer, Size);

	// Process the file
	_rFile.Load((uint8_t *)pBuffer, Size);
	_rFile.Process();

	return true;
}

