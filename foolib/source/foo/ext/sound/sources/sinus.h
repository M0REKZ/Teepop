#include "../instance.h"
#include "../source.h"
#include "../buffer.h"

namespace NFoo { namespace NSound {

	//
	//
	class CSinusInstance : public CInstance
	{
	public:
		int32_t m_Counter;

		CSinusInstance(CSource *pSource, CSoundDevice *pDevice)
		: CInstance(pSource, pDevice)
		{
			m_Counter = 0;
		}

		virtual void PullData(CBuffer &rBuffer, int64 Position);
	};

	class CSinusSource : public CSource
	{
	public:
		CSinusSource(int32_t Hertz=440)
		{
			m_Hertz = Hertz;
			m_Buffer.CreateData(m_Format.m_SampleRate/m_Hertz);

			int32_t Samples = m_Buffer.m_Samples;

			if(m_Format.m_Bits == 8)
			{
				uint8 *pData = (uint8 *)m_Buffer.m_pData;
				if(m_Format.m_Channels == 1)
				{
					for(int32_t s = 0; s < Samples; s++)
						*pData++ = (uint8)((sin(s/(fp32)m_Format.m_SampleRate*m_Hertz*PI*2)+1)*127);
				}
				else if(m_Format.m_Channels == 2)
				{
					for(int32_t s = 0; s < Samples; s++)
					{
						uint8 c = (uint8)((sin(s/(fp32)m_Format.m_SampleRate*m_Hertz*PI*2)+1)*127);
						*pData++ = c;
						*pData++ = c;
					}
				}
			}
			else if(m_Format.m_Bits == 16)
			{
				int16 *pData = (int16 *)m_Buffer.m_pData;
				if(m_Format.m_Channels == 1)
				{
					for(int32_t s = 0; s < Samples; s++)
						*pData++ = (int16)((sin(s/(fp32)m_Format.m_SampleRate*m_Hertz*PI*2))*(0xffff/4));
				}
				else if(m_Format.m_Channels == 2)
				{
					for(int32_t s = 0; s < Samples; s++)
					{
						int16 c = (int16)((sin(s/(fp32)m_Format.m_SampleRate*m_Hertz*PI*2))*(0xffff/4));
						*pData++ = c;
						*pData++ = c;
					}
				}
			}
		}

		~CSinusSource()
		{
			m_Buffer.DestroyData();
		}

		virtual CInstance *CreateInstance(CSoundDevice *pDevice) { return new CSinusInstance(this, pDevice); }
		
		int32_t m_Hertz;
		CFormat m_Format;
		CBuffer m_Buffer;
	};

	void CSinusInstance::PullData(CBuffer &rBuffer, int64 Position)
	{
		rBuffer = ((CSinusSource *)m_pSource)->m_Buffer;
		//return rBuffer.m_Samples;
	}
} }