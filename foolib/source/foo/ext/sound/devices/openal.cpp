#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <foo/base/log.h>
#include <foo/func/map.h>

#include "../device.h"
#include "../puller.h"
#include "../sources/swmixer.h"

using namespace NFoo;
using namespace NFoo::NSound;

class CSoundDevice_OpenAL : public CSoundDevice
{
	enum
	{
		EBufferCount = 3
	};

	ALCdevice *m_pALDevice;
	ALCcontext *m_pContext;
	ALuint m_Source;
	ALuint m_aBuffers[EBufferCount];
	CFormat m_Format;
	int32_t m_BufferSamples;
	int32_t m_iCurrentHandle;
	int64 m_SampleClock;
	TMap<int32_t, CInstance*> m_lInstances;
	CSoftwareMixer m_Mixer;
	CSoftwareMixerInstance *m_pMixerInstance;
	CPuller m_Puller;

	bool CheckALError(const int8 *_pOperation)
	{
		ALenum Error = alGetError();
		if(Error == AL_NO_ERROR)
			return true;

		NLog::WriteF("OpenAL: %s failed with error 0x%x\n", _pOperation, Error);
		return false;
	}

	int32_t QueueBuffer(ALuint _Buffer)
	{
		int32_t BufferSize = m_BufferSamples * m_Format.SampleSize();
		int8 *pData = new int8[BufferSize];
		int32_t Samples = m_Puller.Fill(pData, m_SampleClock, m_BufferSamples);
		if(Samples <= 0)
		{
			delete [] pData;
			return Samples;
		}

		alBufferData(_Buffer, AL_FORMAT_STEREO16, pData,
			Samples * m_Format.SampleSize(), m_Format.m_SampleRate);
		delete [] pData;
		if(!CheckALError("alBufferData"))
			return -1;

		alSourceQueueBuffers(m_Source, 1, &_Buffer);
		if(!CheckALError("alSourceQueueBuffers"))
			return -1;

		m_SampleClock += Samples;
		return Samples;
	}

public:
	CSoundDevice_OpenAL()
	: m_pALDevice(NULL)
	, m_pContext(NULL)
	, m_Source(0)
	, m_BufferSamples(0)
	, m_iCurrentHandle(0)
	, m_SampleClock(0)
	, m_pMixerInstance(NULL)
	{
		for(int32_t i = 0; i < EBufferCount; i++)
			m_aBuffers[i] = 0;
		m_Puller.m_pInstance = NULL;
	}

	virtual ~CSoundDevice_OpenAL()
	{
		Shutdown();
	}

	virtual bool Init(CFormat Format, int32_t BufferSize)
	{
		(void)BufferSize;
		Shutdown();

		if(Format.m_Channels != 2 || Format.m_Bits != 16 || Format.m_SampleRate <= 0)
		{
			NLog::Write("OpenAL: only positive-rate, 16-bit stereo audio is supported\n");
			return false;
		}

		m_pALDevice = alcOpenDevice(NULL);
		if(!m_pALDevice)
		{
			NLog::Write("OpenAL: failed to open the default audio device\n");
			return false;
		}

		m_pContext = alcCreateContext(m_pALDevice, NULL);
		if(!m_pContext || !alcMakeContextCurrent(m_pContext))
		{
			NLog::Write("OpenAL: failed to create the audio context\n");
			Shutdown();
			return false;
		}

		m_Format = Format;
		m_BufferSamples = Format.m_SampleRate / 10;
		m_Mixer.m_Format = Format;
		m_pMixerInstance = (CSoftwareMixerInstance *)m_Mixer.CreateInstance(this);
		m_Puller.m_pInstance = m_pMixerInstance;

		alGenSources(1, &m_Source);
		if(!CheckALError("alGenSources"))
		{
			Shutdown();
			return false;
		}

		alGenBuffers(EBufferCount, m_aBuffers);
		if(!CheckALError("alGenBuffers"))
		{
			Shutdown();
			return false;
		}

		int32_t Queued = 0;
		for(int32_t i = 0; i < EBufferCount; i++)
		{
			int32_t Samples = QueueBuffer(m_aBuffers[i]);
			if(Samples < 0)
			{
				Shutdown();
				return false;
			}
			if(Samples > 0)
				Queued++;
		}

		if(Queued == 0)
		{
			NLog::Write("OpenAL: no audio data was available to start playback\n");
			Shutdown();
			return false;
		}

		alSourcePlay(m_Source);
		if(!CheckALError("alSourcePlay"))
		{
			Shutdown();
			return false;
		}

		return true;
	}

	virtual bool Shutdown()
	{
		while(m_lInstances.Len())
			Destroy(m_lInstances.GetKey(0));

		if(m_pContext)
			alcMakeContextCurrent(m_pContext);

		if(m_Source)
		{
			alSourceStop(m_Source);
			ALint Queued = 0;
			alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &Queued);
			while(Queued-- > 0)
			{
				ALuint Buffer;
				alSourceUnqueueBuffers(m_Source, 1, &Buffer);
			}
			alDeleteSources(1, &m_Source);
			m_Source = 0;
		}

		for(int32_t i = 0; i < EBufferCount; i++)
		{
			if(m_aBuffers[i])
				alDeleteBuffers(1, &m_aBuffers[i]);
			m_aBuffers[i] = 0;
		}

		m_Puller.m_pInstance = NULL;
		SafeDelete(m_pMixerInstance);

		if(m_pContext)
		{
			alcMakeContextCurrent(NULL);
			alcDestroyContext(m_pContext);
			m_pContext = NULL;
		}
		if(m_pALDevice)
		{
			alcCloseDevice(m_pALDevice);
			m_pALDevice = NULL;
		}

		m_BufferSamples = 0;
		m_SampleClock = 0;
		return true;
	}

	virtual bool Update()
	{
		if(!m_pContext || !alcMakeContextCurrent(m_pContext))
			return false;

		ALint Processed = 0;
		alGetSourcei(m_Source, AL_BUFFERS_PROCESSED, &Processed);
		if(!CheckALError("alGetSourcei"))
			return false;

		while(Processed-- > 0)
		{
			ALuint Buffer;
			alSourceUnqueueBuffers(m_Source, 1, &Buffer);
			if(!CheckALError("alSourceUnqueueBuffers"))
				return false;

			if(QueueBuffer(Buffer) < 0)
				return false;
		}

		ALint Queued = 0;
		ALint State = AL_STOPPED;
		alGetSourcei(m_Source, AL_BUFFERS_QUEUED, &Queued);
		alGetSourcei(m_Source, AL_SOURCE_STATE, &State);
		if(!CheckALError("alGetSourcei"))
			return false;

		if(Queued > 0 && State != AL_PLAYING)
		{
			alSourcePlay(m_Source);
			if(!CheckALError("alSourcePlay"))
				return false;
		}

		return true;
	}

	virtual int32_t Play(CSource *_pSource, int32_t Flags)
	{
		if(!m_pMixerInstance || !_pSource)
			return -1;

		CInstance *pInstance = _pSource->CreateInstance(this);
		if(!pInstance)
			return -1;

		pInstance->m_iSound = m_iCurrentHandle++;
		pInstance->m_Flags = Flags;
		CPuller *pPuller = new CPuller(pInstance);
		pInstance->m_pPuller = pPuller;
		m_lInstances[pInstance->m_iSound] = pInstance;
		m_pMixerInstance->m_lSources.Add(pPuller);
		return pInstance->m_iSound;
	}

	virtual void Destroy(int32_t _iSound)
	{
		CInstance *pInstance = m_lInstances.Find(_iSound, NULL);
		if(!pInstance)
			return;

		m_lInstances.Remove(_iSound);
		delete pInstance;
	}

	virtual bool Set(int32_t _iSound, int32_t What, int32_t Value)
	{
		CInstance *pInstance = m_lInstances.Find(_iSound, NULL);
		if(!pInstance)
			return false;

		return pInstance->Set(What, Value);
	}
};

MACRO_REGISTER_CLASS(CSoundDevice_OpenAL, NSound::aDeviceClassTag)
