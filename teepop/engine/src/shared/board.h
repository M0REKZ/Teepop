#ifndef FILE_BOARD_H
#define FILE_BOARD_H

#include <foo/base/base.h>
#include <foo/base/memory.h>
#include <foo/func/stream.h>

const int32_t m_aDirectionOffsets[2][4][2] = {{{0,0},{0,0},{0,0},{0,0}},
											{{1,0},{0,1},{-1,0},{0,-1}}};
/*
class CResult
{
public:
	CResult() { MemorySet(this, 0 sizeof(*this)); }
	int32_t m_ChainsRemoved;
	int32_t m_BlobsRemoved;
	int32_t m_Points;
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

	int32_t m_Mode;

	CResult m_Result;
};*/


class CBoard
{
	bool DoGravity(); // returns true when stable
	bool ZapChains(); // returns true when stable

	int32_t FindChain(int32_t x, int32_t y, bool clear=false, int32_t color=-1);
	int32_t ZapChain(int32_t x, int32_t y);
	
	int32_t GetColumnDepth(int32_t Col);
	int32_t DeepestColumn();
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
		ESize = EElements*sizeof(char),

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
		int32_t m_Count;
		int32_t m_x;
		int32_t m_y;
	};

	bool m_SetNewBlocks;

	CZap m_aZaps[EMaxZaps];
	int32_t m_NumZaps;
	int32_t m_ZapRound;

	//
	int32_t m_SendStones;
	int32_t m_QueuedStones;
	int32_t m_NumStones;
	int32_t m_StoneIndex;
	int32_t m_EvadedStones;

	//
	int32_t m_Scored;

	//
	int32_t m_IttrState;
	int32_t m_Multiplyer;
	int32_t m_BlobCount;

	//
	char m_aField[EWidth][EHeight];
	int32_t m_aExtra[EWidth][EHeight];

	// Current Blocks
	uint8_t m_aCurrentBlocks[2];
	uint8_t m_aNextBlocks[2];

	bool m_Lost;

	CBoard();

	void NewBlock();
	void Reset();
	fp32 Coverage();

	bool ValidateDrop(int32_t x, int32_t r);
	bool ValidatePos(int32_t x, int32_t y, int32_t r);
	bool Drop(int32_t x, int32_t y, int32_t r);
	bool Drop(int32_t x, int32_t r);

	bool Update();

	//# Function: FindChain
	//# Description: Looks for a chain
	int32_t GetConnectionIndex(int32_t x, int32_t y);
	int32_t GetColor(int32_t x, int32_t y) { m_aField[x][y]; }

	virtual void Pack(NFoo::COutputStream &rStream); // 6*12/2=36 byte
	virtual void Unpack(NFoo::CInputStream &rStream);
};


#endif // FILE_BOARD_H