
#include "bitstream.h"

using namespace NFoo;

CBitInputStream::CBitInputStream(CInputStream &rInput)
: m_rInput(rInput)
{
	m_Bits = 0;
	m_NumBits = 0;
}

CBitOutputStream::CBitOutputStream(COutputStream &rOutput)
: m_rOutput(rOutput)
{
	written = 0;
	m_Bits = 0;
	m_NumBits = 0;
}


CBitInputStreamHigh::CBitInputStreamHigh(CInputStream &rInput)
: m_rInput(rInput)
{
	m_Bits = 0;
	m_NumBits = 0;
}
