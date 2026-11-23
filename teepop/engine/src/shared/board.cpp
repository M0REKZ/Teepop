#include <foo/func/math.h>

#include "board.h"

using namespace NFoo;


CBoard::CBoard()
{
	m_SetNewBlocks = true;
	Reset();
}

//
//
//
void CBoard::ClearAll()
{
	ClearField();
	ClearExtra();
	NewBlock();
}

//
//
//
void CBoard::ClearField()
{
	MemorySet(m_aField, 0, ESize);
}

//
//
//
void CBoard::ClearExtra()
{
	MemorySet(m_aExtra, 0, ESize);

	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x++)
			m_aExtra[x][y] = m_aField[x][y]<<EExtra_LastShift;
			
}

//
//
//
void CBoard::ClearMarks()
{
	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x++)
			m_aExtra[x][y] &= ~EExtra_Marked;
}

//
//
//
void CBoard::Reset()
{
	m_Lost = false;
	m_Multiplyer = 0;
	m_IttrState = EIttrState1;

	m_Scored = 0;
	m_NumZaps = 0;
	m_SendStones = 0;
	m_QueuedStones = 0;
	m_NumStones = 0;
	m_StoneIndex = 0;
	m_EvadedStones = 0;
	m_BlobCount = 0;

	ClearAll();

	// generate new blocks
	m_aCurrentBlocks[0] = (int32)(Random()*3.99999999f)+1;
	m_aCurrentBlocks[1] = (int32)(Random()*3.99999999f)+1;
	m_aNextBlocks[0] = (int32)(Random()*3.99999999f)+1;
	m_aNextBlocks[1] = (int32)(Random()*3.99999999f)+1;
}


//
//
//
void CBoard::NewBlock()
{
	//m_XPos = 2;
	//m_YPos = 0;
	//m_Rotation = 1;
	if(m_SetNewBlocks)
	{
		m_aCurrentBlocks[0] = m_aNextBlocks[0];
		m_aCurrentBlocks[1] = m_aNextBlocks[1];
		m_aNextBlocks[0] = (int32)(Random()*3.99999999f)+1;
		m_aNextBlocks[1] = (int32)(Random()*3.99999999f)+1;
	}
}


//
//
//
void CBoard::RandomField()
{
	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x++)
			m_aField[x][y] = (int32)(Random()*3.9999999f)+1;
}

//
//
//
bool CBoard::DoGravity()
{
	bool Stable = true;
	for(int32 y = EHeight-2; y >= 0; y--)
		for(int32 x = 0; x < EWidth; x++)
		{
			if(!m_aField[x][y])
				continue;
			if(m_aField[x][y+1] == 0)
			{
				m_aField[x][y+1] = m_aField[x][y];
				m_aField[x][y] = 0;
				m_aExtra[x][y+1] |= EExtra_Falled;
				Stable = false;
			}
		}
	return Stable;
}

//
//
//
bool CBoard::ZapChains()
{
	ClearMarks();
	bool Stable = true;
	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x++)
		{
			int32 Count = FindChain(x, y);
			if(Count >= 4)
			{
				ClearMarks();
				m_aZaps[m_NumZaps].m_x = 0;
				m_aZaps[m_NumZaps].m_y = 0;
				m_aZaps[m_NumZaps].m_Count = ZapChain(x, y);
				m_aZaps[m_NumZaps].m_x /= m_aZaps[m_NumZaps].m_Count;
				m_aZaps[m_NumZaps].m_y /= m_aZaps[m_NumZaps].m_Count;
				m_NumZaps++;
				Stable = false;
			}
		}
	return Stable;
}




int32 CBoard::GetColumnDepth(int32 Col)
{
	int32 aColumnBoost[EWidth] = {0,1,2,2,1,0};
	for(int32 i = 0; i < EHeight; i++)
	{
		if(m_aField[Col][i] != 0)
		{
			if(i == 0)
				return -100;
			return i-aColumnBoost[Col];
			
		}
	}
	return EHeight-1;
}

fp32 CBoard::Coverage()
{
	int32 Blocks = 0;
	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x++)
			if(m_aField[x][y] != 0)
				Blocks++;

	return Blocks/(fp32)(EWidth*EHeight);
}

//
//
int32 CBoard::DeepestColumn()
{
	int32 BestCount = GetColumnDepth(0);
	int32 Best = 0;
	for(int32 i = 1; i < EWidth; i++)
	{
		int32 Count = GetColumnDepth(i);
		if(Count > BestCount)
		{
			Best = i;
			BestCount = Count;
		}
	}

	return Best;
}

//
//
//
bool CBoard::PlaceStones()
{
	if(!m_NumStones)
		return true;

	bool PlacedStones = false;
	for(int32 i = 0; i < m_NumStones && i < EWidth; i++)
	{
		if(	m_aField[m_StoneIndex][0] == 0 && m_aField[m_StoneIndex][1] == 0 &&
			m_aField[m_StoneIndex][2] == 0 && m_aField[m_StoneIndex][3] == 0)
		{
			PlacedStones = true;
			m_aField[m_StoneIndex][0] = EStone;
			m_NumStones--;
		}

		m_StoneIndex++;
		m_StoneIndex%=EWidth;
	}
	
	if(!PlacedStones)
	{
		int32 Deepest = DeepestColumn();
		if(m_aField[Deepest][0] == 0)
		{
			PlacedStones = true;
			m_aField[Deepest][0] = EStone;
			m_NumStones--;
		}
	}

	if(!PlacedStones)
		m_Lost = true;


	return false;
}

//
//
//
bool CBoard::ValidateDrop(int32 x, int32 r)
{
	if(r == 3)
		return ValidatePos(x, 1, r);
	return ValidatePos(x, 0, r);
}

//
//
//
bool CBoard::ValidatePos(int32 x, int32 y, int32 r)
{
	for(int32 i = 0; i < 2; i++)
	{
		int32 fx = x + m_aDirectionOffsets[i][r][0];
		int32 fy = y + m_aDirectionOffsets[i][r][1];

		if(fx < 0 || fx >= EWidth)
			return false;
		if(fy < 0 || fy >= EHeight)
			return false;
		if(m_aField[fx][fy] != 0)
			return false;
	}
	return true;
}

//
//
//
bool CBoard::Drop(int32 x, int32 y, int32 r)
{
	if(ValidatePos(x,y,r))
	{
		for(int32 i = 0; i < 2; i++)
			m_aField[x+m_aDirectionOffsets[i][r][0]][y+m_aDirectionOffsets[i][r][1]] = m_aCurrentBlocks[i];

		m_SendStones = 0;
		m_Multiplyer = 0;
		m_ZapRound = 0;
		m_Scored = 0;
		m_BlobCount = 0;
		m_EvadedStones = 0;
		
		NewBlock();
		return true;
	}
	else
	{
		m_Lost = true;
		return false;
	}
}

//
//
//
bool CBoard::Drop(int32 x, int32 r)
{
	if(r == 3)
		return Drop(x, 1, r);
	return Drop(x, 0, r);
}

//
//
//
bool CBoard::Update()
{
	ClearExtra();
	m_NumZaps = 0;
	
	if(m_IttrState == EIttrState2)
	{
		// stone placing
		if(!DoGravity())
		{
			PlaceStones();
			return false;
		}

		if(!PlaceStones())
			return false;

		m_IttrState = EIttrState1;
		return true;
	}
	else
	{
		if(DoGravity())
		{
			if(ZapChains())
			{
				if(m_SendStones > 0 || m_NumStones) // evade stones
				{
					if(m_NumStones > m_SendStones)
					{
						m_EvadedStones = m_SendStones;
						m_NumStones -= m_SendStones;
						m_SendStones = 0;
					}
					else
					{
						m_EvadedStones = m_NumStones;
						m_SendStones -= m_NumStones;
						m_NumStones = 0;
					}
				}
				else
				{
					m_Scored = 0;
					m_SendStones = 0;
				}

				//m_Scored = 5;
				//m_SendStones = 5;

				//
				m_IttrState = EIttrState2;
			}
			else if(m_NumZaps)
			{
				m_Multiplyer += m_NumZaps;
				//m_Scored += m_ZapRound*m_NumZaps;
				//m_SendStones += m_ZapRound*m_NumZaps;

				m_ZapRound++;
				

				// count stones
				for(int32 z = 0; z < m_NumZaps; z++)
				{
					int32 s = (m_aZaps[z].m_Count-2)/2;
					if(m_aZaps[z].m_Count > 8)
						s += (m_aZaps[z].m_Count-8)*2;
					m_BlobCount += s;
				}

				m_Scored = m_BlobCount*m_Multiplyer;
				m_SendStones = m_BlobCount*m_Multiplyer;

				if(m_Scored <= 1)
				{
					m_Scored = 0;
					m_SendStones = 0;
				}
			}
		}
		return false;
	}
}

//
//
//
int32 CBoard::ZapChain(int32 x, int32 y)
{
	return FindChain(x,y,true);
}

//# Function: FindChain
//# Description: Looks for a chain
int32 CBoard::FindChain(int32 x, int32 y, bool clear, int32 color)
{
	if(m_aExtra[x][y]&EExtra_Marked)
		return 0;
	else if(color == -1)
		color = m_aField[x][y];
	else if(clear && m_aField[x][y] == EStone)
	{
		m_aField[x][y] = 0;
		m_aExtra[x][y] |= EExtra_Zapped;
		return 0; // stones doesn't count
	}
	else if(m_aField[x][y] != color)
		return 0;

	m_aExtra[x][y] |= EExtra_Marked;
	if(color == 0 || color == EStone)
		return 0;

	if(clear)
	{
		m_aExtra[x][y] |= EExtra_Zapped;
		m_aField[x][y] = 0;
		m_aZaps[m_NumZaps].m_x += x;
		m_aZaps[m_NumZaps].m_y += y;
	}

	int32 n = 1;
	if(y > 0			) n += FindChain(x,y-1,clear,color);
	if(y < EHeight-1	) n += FindChain(x,y+1,clear,color);
	if(x > 0			) n += FindChain(x-1,y,clear,color);
	if(x < EWidth-1		) n += FindChain(x+1,y,clear,color);
	return n;
}

//
//
//
int32 CBoard::GetConnectionIndex(int32 x, int32 y)
{
	int32 i = 0;
	int32 c = m_aField[x][y];
	int32 e = m_aExtra[x][y];
	if(c == EStone)
		return 16;
	if(e&EExtra_Falled)
		return 0;
	if(c == 0)
		return 0;

	if(x > 0		&& m_aField[x-1][y] == c && !(m_aExtra[x-1][y]&EExtra_Falled)) i |= 1;
	if(x < EWidth-1 && m_aField[x+1][y] == c && !(m_aExtra[x+1][y]&EExtra_Falled)) i |= 2;
	if(y > 0		&& m_aField[x][y-1] == c && !(m_aExtra[x][y-1]&EExtra_Falled)) i |= 4;
	if(y < EHeight-1&& m_aField[x][y+1] == c && !(m_aExtra[x][y+1]&EExtra_Falled)) i |= 8;
	return i;
}

//
//
//
void CBoard::Pack(COutputStream &rStream) // 6*12/2=36 byte
{
	rStream.WriteBE<uint8>(m_StoneIndex); // should be packed into 1 byte?
	rStream.WriteBE<uint8>(m_NumStones);
	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x+=2)
			rStream.WriteNE<uint8>((m_aField[x][y]<<4)|m_aField[x+1][y]);
}

//
//
//
void CBoard::Unpack(CInputStream &rStream)
{
	m_StoneIndex = rStream.ReadBE<uint8>();
	m_NumStones = rStream.ReadBE<uint8>();
	for(int32 y = 0; y < EHeight; y++)
		for(int32 x = 0; x < EWidth; x+=2)
		{
			uint8 b = rStream.ReadNE<uint8>();
			m_aField[x][y] = b>>4;
			m_aField[x+1][y] = b&0xF;
		}
}