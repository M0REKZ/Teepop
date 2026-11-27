
#include <SDL3/SDL.h>

#include <foo/base/log.h>
#include <foo/func/map.h>

#include "../device.h"
#include "../puller.h"
#include "../sources/swmixer.h"

using namespace NFoo;
using namespace NFoo::NSound;

class CSoundDevice_SDL3 : public CSoundDevice
{
	bool m_Initialized;

	CFormat m_Format;
	uint8_t * m_pBuffer = nullptr;
	int32_t m_BufferSamples = 0;
	int32_t m_BufferBytes = 0;

	int64_t m_SampleClock;

	SDL_AudioStream * m_pSDL3AudioStream = nullptr;

	int32_t m_iCurrentHandle;
	TMap<int32_t, CInstance*> m_lInstances;

	CSoftwareMixer m_Mixer;
	CSoftwareMixerInstance *m_pMixerInstance;
	CPuller m_Puller;

	bool m_Resumed = false;

public:

	CSoundDevice_SDL3();
	virtual ~CSoundDevice_SDL3();

	virtual bool Init(CFormat Format, int32_t BufferSize);
	virtual bool Shutdown();
	virtual bool Update();
	
	virtual int32_t Play(CSource *pSource, int32_t Flags);
	virtual void Destroy(int32_t iSound);
	virtual bool Set(int32_t iSound, int32_t What, int32_t Value);
};

CSoundDevice_SDL3::CSoundDevice_SDL3() : CSoundDevice()
{
	m_pMixerInstance = nullptr;
	m_Initialized = false;
	m_pSDL3AudioStream = nullptr;
	m_iCurrentHandle = 0;
}

CSoundDevice_SDL3::~CSoundDevice_SDL3()
{
	Shutdown();
	while(m_lInstances.Len())
		Destroy(m_lInstances.GetKey(0));

	SafeDelete(m_pMixerInstance);
}

bool CSoundDevice_SDL3::Init(CFormat Format, int32_t BufferSize)
{
	m_Format = Format;
	m_SampleClock = 0;
	m_Resumed = false;

	SDL_AudioSpec spec = {
		.format = SDL_AUDIO_UNKNOWN,
		.channels = m_Format.m_Channels,
		.freq = m_Format.m_SampleRate,
	};

	switch (Format.m_Bits)
	{
		case 8:
			spec.format = SDL_AUDIO_S8;
			break;

		case 16:
			spec.format = SDL_AUDIO_S16;
			break;

		case 32:
			spec.format = SDL_AUDIO_S32;
			break;
		
		default:
			break;
	}


	m_pSDL3AudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);

	if (!m_pSDL3AudioStream)
	{
		NLog::WriteF("Error while trying to initialize SDL audio: %s\n", SDL_GetError());
		return false;
	}

    m_BufferSamples = Format.m_SampleRate / 50;

    m_BufferBytes = m_BufferSamples * m_Format.SampleSize();

    m_pBuffer = new uint8_t[m_BufferBytes];

	m_pMixerInstance = (CSoftwareMixerInstance *)m_Mixer.CreateInstance(this);
	m_Puller.m_pInstance = m_pMixerInstance;

	m_Initialized = true;
	
    return true;
}

bool CSoundDevice_SDL3::Shutdown()
{
	if(m_Initialized)
	{
		SDL_DestroyAudioStream(m_pSDL3AudioStream);
        m_pSDL3AudioStream = nullptr;

		delete[] m_pBuffer;
		m_pBuffer = nullptr;

		m_Initialized = false;
	}
    return true;
}

bool CSoundDevice_SDL3::Update()
{
	if(!m_Initialized)
	{
		NLog::WriteF("SDL audio not initialized\n");
        return false;
	}

	// Limit queued audio to prevent latency from accumulating.
	const int32_t MaxQueuedBytes = m_BufferBytes * 3;
	const int32_t QueuedBytes =
		SDL_GetAudioStreamQueued(m_pSDL3AudioStream);

	if(QueuedBytes >= MaxQueuedBytes)
		return true;

    // Generate one chunk of mixed PCM.
    int64_t Samples = m_Puller.Fill(m_pBuffer, m_SampleClock, m_BufferSamples);
	if(Samples <= 0)
		return true;
	m_SampleClock += Samples;

	const int32_t Bytes = Samples * m_Format.SampleSize();

    // Queue PCM for SDL playback.
    if(!SDL_PutAudioStreamData(m_pSDL3AudioStream, m_pBuffer, Bytes))
    {
        NLog::WriteF("SDL audio: PutAudioStreamData failed: %s\n",
            SDL_GetError());
        return false;
    }

	if (!m_Resumed && !SDL_ResumeAudioStreamDevice(m_pSDL3AudioStream))
	{
		NLog::WriteF("Error while trying to resume SDL audio: %s\n", SDL_GetError());
		return false;
	}
	m_Resumed = true;

    return true;
}

int32_t CSoundDevice_SDL3::Play(CSource *pSource, int32_t Flags)
{
	if(!pSource)
		return 0;

	CInstance * pInstance = pSource->CreateInstance(this);

	pInstance->m_iSound = m_iCurrentHandle++;
	pInstance->m_Flags = Flags;
	CPuller *pPuller = new CPuller(pInstance);
	pInstance->m_pPuller = pPuller;
	m_lInstances[pInstance->m_iSound] = pInstance;
	m_pMixerInstance->m_lSources.Add(pPuller);
	return pInstance->m_iSound;
}

void CSoundDevice_SDL3::Destroy(int32_t iSound)
{
	CInstance *pInstance = m_lInstances.Find(iSound, NULL);
	if(!pInstance)
		return;

	m_lInstances.Remove(iSound);
	delete pInstance;
}

bool CSoundDevice_SDL3::Set(int32_t iSound, int32_t What, int32_t Value)
{
	CInstance *pInstance = m_lInstances.Find(iSound, NULL);
	if(!pInstance)
		return false;

	return pInstance->Set(What, Value);
}

MACRO_REGISTER_CLASS(CSoundDevice_SDL3, NSound::aDeviceClassTag)
