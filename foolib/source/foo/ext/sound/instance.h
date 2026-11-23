#ifndef FOO_FILE_EXT_SOUND_INSTANCE_H
#define FOO_FILE_EXT_SOUND_INSTANCE_H

#include "source.h"
#include "puller.h"

namespace NFoo { namespace NSound {

	class CPuller;

	class CInstance
	{
		friend class CPuller;
		friend class CInstance;
		

		virtual void PullData(class CBuffer &rBuffer, int64 Position) pure;
	public:
		CInstance(CSource *pSource, CSoundDevice *pDevice);
		virtual ~CInstance();

		void OnEnd();

		virtual bool Set(int32 What, int32 Value)
		{
			if(What == CSource::ESet_Volume)
				m_Volume = Value/1000.0f;
			else
				return false;
			return true;
		}

		CSoundDevice *m_pDevice;
		CSource *m_pSource;

		CPuller *m_pPuller;

		int32 m_iSound;

		int64 m_Start;
		int32 m_Flags;
		fp32 m_Volume;
		fp32 m_Rate;
	};

} }

#endif // FOO_FILE_EXT_SOUND_INSTANCE_H