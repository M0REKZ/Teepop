#ifdef FOO_PLATFORM_WIN32
//#define WIN32_LEAN_AND_MEAN
//#define VC_EXTRA_LEAN
#include <windows.h> // FUCK!!! ASSS!!!

#include <foo/base/log.h>
#include <foo/func/thread.h>
#include <foo/func/map.h>

#include "../device.h"
#include "../sources/swmixer.h"

using namespace NFoo;
using namespace NFoo::NSound;

//
//
class CSoundDevice_WaveOut : public CSoundDevice
{
	HWAVEOUT m_hWaveOut;
	enum { ENumBuffers=10 };
	
	WAVEHDR m_aBufferHeaders[ENumBuffers];
	void *m_aBuffers[ENumBuffers];
	int32_t m_BufferSamples;
	int32_t m_BufferSize;
	int32_t m_iCurrentBuffer;

	CFormat m_Format;

	int32_t m_iCurrentHandle;
	TMap<int32_t, CInstance*> m_lInstances;

	class CWorker : public CThread
	{
	public:
		CSoundDevice_WaveOut *m_pDevice;
		virtual int32_t ThreadMain()
		{
			m_pDevice->m_ThreadLock.Aquire();
			m_pDevice->m_ThreadID = GetCurrentThreadId();
			m_pDevice->m_ThreadCommand = 2;
			m_pDevice->m_ThreadLock.Release();

			while(1)
			{
				m_pDevice->m_ThreadLock.Aquire();
				int32_t cmd = m_pDevice->m_ThreadCommand;
				m_pDevice->m_ThreadLock.Release();

				if(cmd == 1)
					break;
			}

			while(!AwaitingTermination())
			{
				MSG Msg;
				if(GetMessage(&Msg, NULL, NULL, NULL))
					m_pDevice->Refill();
			}

			return 0;
		}
	};

	CSoftwareMixer m_Mixer;
	CSoftwareMixerInstance *m_pMixerInstance;
	virtual bool Refill();

public:
	CLock m_ThreadLock;
	uint32_t m_ThreadID;
	int32_t m_ThreadCommand;
	CWorker m_Worker;

	CSoundDevice_WaveOut();
	~CSoundDevice_WaveOut();

	virtual bool Init(CFormat Format, int32_t BufferSize);
	virtual bool Shutdown();
	virtual bool Update();
	
	virtual int32_t Play(CSource *pSource, int32_t Flags);
	virtual void Destroy(int32_t iSound);
	virtual bool Set(int32_t iSound, int32_t What, int32_t Value);


	CPuller m_Puller;
};

//
//
//
CSoundDevice_WaveOut::CSoundDevice_WaveOut()
{
	m_hWaveOut = NULL;
	m_iCurrentBuffer = 0;
	m_ThreadCommand = 0;

	for(int32_t i = 0; i < ENumBuffers; i++)
		m_aBuffers[i] = NULL;

	m_iCurrentHandle = 0;
	m_Puller.m_pInstance = NULL;
	m_Worker.m_pDevice = this;
}

CSoundDevice_WaveOut::~CSoundDevice_WaveOut()
{
	if(m_pMixerInstance)
		Destroy(m_pMixerInstance->m_iSound);
	SafeDelete(m_pMixerInstance);
	for(int32_t i = 0; i < ENumBuffers; i++)
		SafeArrayDelete(m_aBuffers[i]);
}

const bool g_MultiThread = false;

//
//
//
bool CSoundDevice_WaveOut::Init(CFormat Format, int32_t BufferSize)
{
	m_Format = Format;

	// open device
	WAVEFORMATEX WaveFormat;
	MemoryZero(&Format);
	WaveFormat.cbSize = sizeof(WAVEFORMATEX);
	WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	WaveFormat.nSamplesPerSec = m_Format.m_SampleRate;
	WaveFormat.wBitsPerSample = m_Format.m_Bits;
	WaveFormat.nChannels = m_Format.m_Channels;
	WaveFormat.nAvgBytesPerSec = 0;
	WaveFormat.nBlockAlign = m_Format.SampleSize();

	if(g_MultiThread)
	{
		m_Worker.StartThread();

		while(1)
		{
			m_ThreadLock.Aquire();
			int32_t cmd = m_ThreadCommand;
			m_ThreadLock.Release();

			if(cmd == 2)
				break;
		}
	}

	MMRESULT res = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &WaveFormat, m_ThreadID, NULL, CALLBACK_THREAD);
	if(res != MMSYSERR_NOERROR)
		return false;

	// create buffers
	int32_t BufferLength = BufferSize;
	m_BufferSamples = m_Format.m_SampleRate/(1000/BufferLength)/ENumBuffers;
	m_BufferSize = m_BufferSamples*m_Format.SampleSize();
	m_iCurrentBuffer = 0;

	for(int32_t i = 0; i < ENumBuffers; i++)
	{
		m_aBuffers[i] = new uint8[m_BufferSize];
		MemoryZero(m_aBuffers[i], m_BufferSize);

		MemoryZero(&m_aBufferHeaders[i]);
		m_aBufferHeaders[i].lpData = (int8*)m_aBuffers[i];
		m_aBufferHeaders[i].dwBufferLength = m_BufferSize;
		m_aBufferHeaders[i].dwBytesRecorded = m_BufferSize;
	}

	NLog::WriteF("Sound Buffer Length = %.2f ms (%d samples)\n", m_BufferSamples/(fp32)m_Format.m_SampleRate*1000*ENumBuffers, m_BufferSamples);

	m_pMixerInstance = (CSoftwareMixerInstance *)m_Mixer.CreateInstance(this);
	m_Puller.m_pInstance = m_pMixerInstance;

	// start thread
	Refill();

	if(g_MultiThread)
	{
		m_ThreadLock.Aquire();
		m_ThreadCommand = 1;
		m_ThreadLock.Release();
	}

	return true;
}

//
//
//
int32_t CSoundDevice_WaveOut::Play(CSource *pSource, int32_t Flags)
{
	m_ThreadLock.Aquire();

	CInstance *pInstance = pSource->CreateInstance(this);
	pInstance->m_iSound = m_iCurrentHandle++;
	pInstance->m_Flags = Flags;
	
	m_lInstances[pInstance->m_iSound] = pInstance;
	CPuller *pPuller = new CPuller(pInstance);
	pInstance->m_pPuller = pPuller;
	NLog::WriteF("WaveOut: created sound=%d\n", pInstance->m_iSound);

	m_pMixerInstance->m_lSources.Add(pPuller);

	m_ThreadLock.Release();
	return 0;
}

void CSoundDevice_WaveOut::Destroy(int32_t iSound)
{
	CInstance *pInstance = m_lInstances.Find(iSound, NULL);
	if(!pInstance)
		return;

	m_ThreadLock.Aquire();
	NLog::WriteF("WaveOut: destroy sound=%d\n", pInstance->m_iSound);
	m_lInstances.Remove(pInstance->m_iSound);
	delete pInstance;
	//m_pMixerInstance->m_lSources.
	m_ThreadLock.Release();
}

bool CSoundDevice_WaveOut::Set(int32_t iSound, int32_t What, int32_t Value)
{
	CInstance *pInstance = m_lInstances.Find(iSound, NULL);
	if(!pInstance)
		return false;

	m_ThreadLock.Aquire();
	bool v = pInstance->Set(What, Value);
	//m_pMixerInstance->m_lSources.
	m_ThreadLock.Release();
	return v;
}


//
//
//
bool CSoundDevice_WaveOut::Update()
{
	if(!g_MultiThread)
		Refill();
	return true;
}

//
//
//
bool CSoundDevice_WaveOut::Refill()
{
	while(1)
	{
		WAVEHDR &rCurrentHeader = m_aBufferHeaders[m_iCurrentBuffer];

		// check to see if we need to pull more data
		if(rCurrentHeader.dwFlags&WHDR_PREPARED && !(rCurrentHeader.dwFlags&WHDR_DONE))
			return true;

		waveOutUnprepareHeader(m_hWaveOut, &rCurrentHeader, sizeof(rCurrentHeader));

		// pull data
		m_ThreadLock.Aquire();
		m_Puller.Fill(rCurrentHeader.lpData, 0, m_BufferSamples);
		m_ThreadLock.Release();
		//m_pSource->PullData(rCurrentHeader.lpData, m_BufferSamples);

		// prepare header and write data
		waveOutPrepareHeader(m_hWaveOut, &rCurrentHeader, sizeof(rCurrentHeader));
		waveOutWrite(m_hWaveOut, &rCurrentHeader, sizeof(rCurrentHeader));

		// switch buffer
		m_iCurrentBuffer++;
		m_iCurrentBuffer %= ENumBuffers; 
	}

	return true;
}


//
//
//
bool CSoundDevice_WaveOut::Shutdown()
{
	// terminate thread
	m_Worker.SendTermination();
	m_Worker.WaitForTermination();

	if(!m_hWaveOut)
		return false;
	if(waveOutClose(m_hWaveOut) != MMSYSERR_NOERROR)
		return false;
	SafeArrayDelete(m_aBuffers[0]);
	SafeArrayDelete(m_aBuffers[1]);
	return true;
}


//MACRO_REGISTER_CLASS(CSoundDevice_WaveOut, NSound::aDeviceClassTag)

//#pragma comment(lib, "winmm.lib")

#endif
