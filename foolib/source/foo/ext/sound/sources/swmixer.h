
#include <foo/base/memory.h>

#include "../instance.h"
#include "../source.h"
#include "../buffer.h"
#include "../puller.h"

namespace NFoo { namespace NSound {

	//
	//
	//
	class CSoftwareMixerInstance : public CInstance
	{
	public:
		CFormat m_Format;
		CBuffer m_MixBuffer;
		CBuffer m_SrcBuffer;
		int32 m_Samples;

		CSoftwareMixerInstance(CSource *pSource, CSoundDevice *pDevice)
		: CInstance(pSource, pDevice)
		{
			m_pPuller = NULL;
			m_Format = m_pSource->m_Format;
			m_Samples = m_Format.m_SampleRate/10;
			m_MixBuffer.CreateData(m_Samples);
			m_SrcBuffer.CreateData(m_Samples);
		}

		~CSoftwareMixerInstance()
		{
			m_lSources.DeleteAll();
			m_MixBuffer.DestroyData();
			m_SrcBuffer.DestroyData();
		}

		TDynamicArray<CPuller*> m_lSources;

		virtual void PullData(CBuffer &rBuffer, int64 Position)
		{
			MemoryZero(m_MixBuffer.m_pData, m_Format.SampleSize()*m_Samples); // seams wrong

			for(int32 s = 0; s < m_lSources.Len(); s++)
			{
				CInstance *pInstance = m_lSources[s]->m_pInstance;

				if(!pInstance) // no instance = puller done
				{
					delete m_lSources[s];
					m_lSources.RemoveByIndexFast(s);
					s--;
					continue;
				}

				int16 *pDst16 = (int16 *)m_MixBuffer.m_pData;
				int16 *pSrc16 = (int16 *)m_SrcBuffer.m_pData;

				if(pInstance->m_Start < 0) // set start position if we don't have one
					pInstance->m_Start = Position;

				int32 Samples = m_lSources[s]->Fill(m_SrcBuffer.m_pData, Position, m_Samples);

				if(Samples == 0)
				{
					pInstance->OnEnd();
					continue;
				}

				int32 Volume = (int32)(pInstance->m_Volume*(1<<12));

				if(m_Format.m_Bits == 16)
				{
					for(int32 i = 0; i < Samples*m_Format.m_Channels; i++) // can anyone say MMX?
					{
						int32 Value = *pDst16+((*pSrc16*Volume)>>12);
						if(Value > (int16)0x7FFF)
							*pDst16++ = (int16)0x7FFF;
						else if(Value < (int16)0x8000)
							*pDst16++ = (int16)0x8000;
						else
							*pDst16++ = Value;
						pSrc16++;
					}
				}
			}

			rBuffer = m_MixBuffer;
		}
	};

	//
	//
	class CSoftwareMixer : public CSource
	{
	public:
		virtual CInstance *CreateInstance(CSoundDevice *pDevice) { return new CSoftwareMixerInstance(this, pDevice); }
	};

} }
