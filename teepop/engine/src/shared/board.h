#ifndef FILE_BOARD_H
#define FILE_BOARD_H

#include <foo/base/base.h>
#include <foo/base/memory.h>
#include <foo/func/stream.h>

const int32 m_aDirectionOffsets[2][4][2] = {{{0,0},{0,0},{0,0},{0,0}},
											{{1,0},{0,1},{-1,0},{0,-1}}};
/*
class CResult
{
public:
	CResult() { MemorySet(this, 0 sizeof(*this)); }
	int32 m_ChainsRemoved;
	int32 m_BlobsRemoved;
	int32 m_Points;
};

class CIttrProcess
{
public:
	CIttrProcess()
	{
		m_Mode = EGravity;
	};

	enum
	{
		EGravity,
		EZapping,
		EStable
	};

	int32 m_Mode;

	CResult m_Result;
};*/


class CBoard
{
	bool DoGravity(); // returns true when stable
	bool ZapChains(); // returns true when stable

	int32 FindChain(int32 x, int32 y, bool clear=false, int32 color=-1);
	int32 ZapChain(int32 x, int32 y);
	
	int32 GetColumnDepth(int32 Col);
	int32 DeepestColumn();
	bool PlaceStones(); // returns true when no more blocks to place

	void ClearAll();
	void ClearField();
	void ClearExtra();
	void ClearMarks();
	void RandomField();

public:
	enum
	{
		EStone = 5,
		EWidth = 6,
		EHeight = 12,
		EMaxZaps = (EWidth*EHeight)/4+1,
		EElements = EWidth*EHeight,
		ESize = EElements*sizeof(int8),

		EIttrState1,
		EIttrState2,

		EExtra_Marked=1,
		EExtra_Falled=2,
		EExtra_Zapped=4,
		EExtra_LastShift=8
	};

	class CZap
	{
	public:
		int32 m_Count;
		int32 m_x;
		int32 m_y;
	};

	bool m_SetNewBlocks;

	CZap m_aZaps[EMaxZaps];
	int32 m_NumZaps;
	int32 m_ZapRound;

	//
	int32 m_SendStones;
	int32 m_QueuedStones;
	int32 m_NumStones;
	int32 m_StoneIndex;
	int32 m_EvadedStones;

	//
	int32 m_Scored;

	//
	int32 m_IttrState;
	int32 m_Multiplyer;
	int32 m_BlobCount;

	//
	int8 m_aField[EWidth][EHeight];
	int32 m_aExtra[EWidth][EHeight];

	// Current Blocks
	uint8 m_aCurrentBlocks[2];
	uint8 m_aNextBlocks[2];

	bool m_Lost;

	CBoard();

	void NewBlock();
	void Reset();
	fp32 Coverage();

	bool ValidateDrop(int32 x, int32 r);
	bool ValidatePos(int32 x, int32 y, int32 r);
	bool Drop(int32 x, int32 y, int32 r);
	bool Drop(int32 x, int32 r);

	bool Update();

	//# Function: FindChain
	//# Description: Looks for a chain
	int32 GetConnectionIndex(int32 x, int32 y);
	int32 GetColor(int32 x, int32 y) { m_aField[x][y]; }

	virtual void Pack(NFoo::COutputStream &rStream); // 6*12/2=36 byte
	virtual void Unpack(NFoo::CInputStream &rStream);
};


#endif // FILE_BOARD_H