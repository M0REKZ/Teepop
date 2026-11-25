#include <foo/base/memory.h>
#include "puller.h"

using namespace NFoo;
using namespace NFoo::NSound;

//
//
//
CPuller::CPuller(CInstance *pInstance)
{
	m_pInstance = pInstance;
	m_SamplesLeft = 0;
}

CPuller::~CPuller()
{
	for(int32_t i = 0; i < m_lBuffers.Len(); i++)
		m_lBuffers[i].DestroyData();
	m_lStartPositions.Clear();
	m_lBuffers.Clear();
}

//
//
//
int32_t CPuller::Fill(void *pData, int64 Position, int32_t Samples) // should do convertion
{
	if(!m_pInstance)
		return 0;

	int32_t StartSamples = Samples;
	uint8 *pDst = (uint8 *)pData;

	// remove old buffers
	for(int32_t i = 0; i < m_lStartPositions.Len(); i++)
	{
		if(m_lStartPositions[i]+m_lBuffers[i].m_Samples+40000 < Position)
		{
			m_lStartPositions.RemoveByIndex(i);
			m_lBuffers[i].DestroyData();
			m_lBuffers.RemoveByIndex(i);
			i--;
		}
		else
			break;
	}

	// fill with existing buffers
	for(int32_t i = 0; i < m_lStartPositions.Len(); i++)
	{
		if(m_lStartPositions[i]+m_lBuffers[i].m_Samples < Position)
			continue;
		int32_t bufpos = Position-m_lStartPositions[i];
		if(bufpos < 0)
			bufpos = 0;
		int32_t CopySize = Min(Samples, m_lBuffers[i].m_Samples-bufpos);
		MemoryCopy(pDst, (uint8*)m_lBuffers[i].m_pData+(bufpos)*4, CopySize*4);
		pDst += CopySize*4;
		Samples -= CopySize;
		Position += CopySize;

		if(Samples == 0) // done filling
			return StartSamples;
	};
	
	// ok, we need to pull more data
	while(1)
	{
		CBuffer PullBuffer;
		m_pInstance->PullData(PullBuffer, Position);

		if(!PullBuffer.m_Samples)
			return StartSamples-Samples;

		CBuffer Buffer;
		Buffer.CreateData(PullBuffer.m_Samples);
		MemoryCopy(Buffer.m_pData, PullBuffer.m_pData, PullBuffer.m_Samples*4);
		m_lStartPositions.Add(Position);
		m_lBuffers.Add(Buffer);

		int32_t CopySize = Min(Samples, Buffer.m_Samples);
		MemoryCopy(pDst, Buffer.m_pData, CopySize*4);
		pDst += CopySize*4;
		Samples -= CopySize;
		Position += CopySize;

		if(Samples == 0)
			return StartSamples;
	}
}

/*
int32_t CPuller::Fill(void *pData, int32_t Position, int32_t Samples) // should do convertion
{
	if(!m_pInstance)
		return 0;

	int32_t StartSamples = Samples;
	int8 *pDst = (int8 *)pData;
	int32_t ss = m_Buffer.m_Format.SampleSize();

	if(m_SamplesLeft)
	{
		int32_t ToCopy = m_SamplesLeft < Samples ? m_SamplesLeft : Samples;
		MemoryCopy(pDst, (int8*)(m_Buffer.m_pData)+(m_Buffer.m_Samples-m_SamplesLeft)*ss, ToCopy*ss);
		pDst+=ToCopy*ss;
		m_SamplesLeft -= ToCopy;
		Samples -= ToCopy;
	}

	while(Samples)
	{
		m_Buffer.Clear();
		m_pInstance->PullData(m_Buffer, Position);
		m_SamplesLeft = m_Buffer.m_Samples;

		if(!m_SamplesLeft) // empty
			break;

		int32_t ToCopy = m_SamplesLeft < Samples ? m_SamplesLeft : Samples;
		MemoryCopy(pDst, m_Buffer.m_pData, ToCopy*ss);
		pDst+=ToCopy*ss;

		Position += m_Buffer.m_Samples;
		m_SamplesLeft -= ToCopy;
		Samples -= ToCopy;
	}

	return StartSamples-Samples;
}*/
