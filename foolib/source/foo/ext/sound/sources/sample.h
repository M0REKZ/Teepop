
//#include <foo/base/memory.h>
#include <foo/func/stream.h>

#include "../instance.h"
#include "../source.h"
#include "../buffer.h"
//#include "../puller.h"

namespace NFoo { namespace NSound {

	class CSample : public CSource
	{
	public:
		void *m_pData;
		int32 m_DataSize;

		CSample()
		{
			m_DataSize = 0;
			m_pData = 0;
		}

		void Load(CInputStream &rStream)
		{
			m_DataSize = rStream.GetSize();
			m_pData = new int8[m_DataSize];
			rStream.Read(m_pData, m_DataSize);
		}

		~CSample()
		{
			delete [] (int8 *)m_pData;
			m_pData = NULL;
		}

		virtual CInstance *CreateInstance(CSoundDevice *pDevice);
	};

	class CSampleInstance : public CInstance
	{
	public:
		CSample *m_pSample;
		int32 m_Position;

		CSampleInstance(CSource *pSource, CSoundDevice *pDevice)
		: CInstance(pSource, pDevice)
		{
			m_pSample = (CSample *)pSource;
			m_Position = 0;
		}

		virtual void PullData(CBuffer &rBuffer, int64 Position)
		{
			if(m_Position == -1)
				return; 

			m_Position = -1;
			rBuffer.m_pData = m_pSample->m_pData;
			rBuffer.m_Samples = m_pSample->m_DataSize/4;
		}
	};

} }