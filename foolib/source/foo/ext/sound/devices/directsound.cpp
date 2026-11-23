//#define WIN32_LEAN_AND_MEAN
//#define VC_EXTRA_LEAN
#include <windows.h> // FUCK!!! ASSS!!!
#include <dsound.h>

#include <foo/base/log.h>
#include <foo/base/math.h>
#include <foo/func/thread.h>
#include <foo/func/map.h>

#include "../device.h"
#include "../sources/swmixer.h"

#pragma comment(lib, "dsound.lib")

using namespace NFoo;
using namespace NFoo::NSound;

//
//
class CSoundDevice_DirectSound : public CSoundDevice
{
	//HDirectSound m_hDirectSound;
	LPDIRECTSOUND8 m_pDSDevice;
	LPDIRECTSOUNDBUFFER m_pBuffer; 
	
	int32 m_BufferSamples;
	int32 m_BufferSize;
	int32 m_MixAhead;

	CFormat m_Format;

	int32 m_iCurrentHandle;
	TMap<int32, CInstance*> m_lInstances;

	CSoftwareMixer m_Mixer;
	CSoftwareMixerInstance *m_pMixerInstance;
	virtual bool Refill();

public:

	CSoundDevice_DirectSound();
	virtual ~CSoundDevice_DirectSound();

	virtual bool Init(CFormat Format, int32 BufferSize);
	virtual bool Shutdown();
	virtual bool Update();
	
	virtual int32 Play(CSource *pSource, int32 Flags);
	virtual void Destroy(int32 iSound);
	virtual bool Set(int32 iSound, int32 What, int32 Value);


	CPuller m_Puller;
};

//
//
//
CSoundDevice_DirectSound::CSoundDevice_DirectSound()
{
	m_iCurrentHandle = 0;
	m_Puller.m_pInstance = NULL;
	m_BufferSize = 44100*4*10;
	m_MixAhead = 44100;
}

CSoundDevice_DirectSound::~CSoundDevice_DirectSound()
{

	while(m_lInstances.Len())
		Destroy(m_lInstances.GetKey(0));
		/*
	for(int32 i = 0; i < m_lInstances.Len(); i++)
		Destroy(m_lInstances[i]->)*/

	//if(m_pMixerInstance)
	//	Destroy(m_pMixerInstance->m_iSound);
	SafeDelete(m_pMixerInstance);
}


extern HWND g_WIN32_MainWindowHandle;

//
//
//
bool CSoundDevice_DirectSound::Init(CFormat Format, int32 BufferSize)
{
	m_Format = Format;

	//
	m_MixAhead = (BufferSize*44100*4)/1000;

	// create format
	WAVEFORMATEX WaveFormat;
	MemoryZero(&Format);
	WaveFormat.cbSize = sizeof(WAVEFORMATEX);
	WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	WaveFormat.nSamplesPerSec = m_Format.m_SampleRate;
	WaveFormat.wBitsPerSample = m_Format.m_Bits;
	WaveFormat.nChannels = m_Format.m_Channels;
	WaveFormat.nAvgBytesPerSec = m_Format.m_Channels*(m_Format.m_Bits/8)*m_Format.m_SampleRate;
	WaveFormat.nBlockAlign = m_Format.SampleSize();

	// open device
	HRESULT hr = DirectSoundCreate8(NULL, &m_pDSDevice, NULL);
	NLog::WriteF("m_pDSDevice=%x\n", m_pDSDevice);
	HRESULT coopres = m_pDSDevice->SetCooperativeLevel(g_WIN32_MainWindowHandle, DSSCL_PRIORITY);

	if(coopres == DSERR_ALLOCATED) NLog::WriteF("DirectSound: Error setting cooperative level, DSERR_ALLOCATED\n");
	if(coopres == DSERR_INVALIDPARAM) NLog::WriteF("DirectSound: Error setting cooperative level, DSERR_INVALIDPARAM\n");
	if(coopres == DSERR_UNINITIALIZED) NLog::WriteF("DirectSound: Error setting cooperative level, DSERR_UNINITIALIZED\n");
	if(coopres == DSERR_UNSUPPORTED) NLog::WriteF("DirectSound: Error setting cooperative level, DSERR_UNSUPPORTED\n");


	// create buffers
	DSBUFFERDESC desc;
	MemoryZero(&desc);
	desc.dwSize = sizeof(DSBUFFERDESC); 
	desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_LOCSOFTWARE|DSBCAPS_STICKYFOCUS;
	desc.dwBufferBytes = m_BufferSize;
	desc.lpwfxFormat = &WaveFormat; 

	m_pDSDevice->CreateSoundBuffer(&desc, &m_pBuffer, NULL); 
	NLog::WriteF("m_pBuffer=%x\n", m_pBuffer);

	HRESULT playres = m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if(playres == DSERR_BUFFERLOST) NLog::WriteF("DSERR_BUFFERLOST\n");
	if(playres == DSERR_INVALIDCALL) NLog::WriteF("DSERR_INVALIDCALL\n");
	if(playres == DSERR_INVALIDPARAM) NLog::WriteF("DSERR_INVALIDPARAM\n");
	if(playres == DSERR_PRIOLEVELNEEDED) NLog::WriteF("DSERR_PRIOLEVELNEEDED\n");


	m_pMixerInstance = (CSoftwareMixerInstance *)m_Mixer.CreateInstance(this);
	m_Puller.m_pInstance = m_pMixerInstance;

	return true;
}

//
//
//
int32 CSoundDevice_DirectSound::Play(CSource *pSource, int32 Flags)
{
	CInstance *pInstance = pSource->CreateInstance(this);
	pInstance->m_iSound = m_iCurrentHandle++;
	pInstance->m_Flags = Flags;
	//pInstance->m_StartSample = -1;
	
	m_lInstances[pInstance->m_iSound] = pInstance;
	CPuller *pPuller = new CPuller(pInstance);
	pInstance->m_pPuller = pPuller;
	//NLog::WriteF("DirectSound: created sound=%d\n", pInstance->m_iSound);

	m_pMixerInstance->m_lSources.Add(pPuller);
	return 0;
}

void CSoundDevice_DirectSound::Destroy(int32 iSound)
{
	CInstance *pInstance = m_lInstances.Find(iSound, NULL);
	if(!pInstance)
		return;

	//NLog::WriteF("DirectSound: destroy sound=%d\n", pInstance->m_iSound);
	m_lInstances.Remove(pInstance->m_iSound);
	delete pInstance;
}

bool CSoundDevice_DirectSound::Set(int32 iSound, int32 What, int32 Value)
{
	CInstance *pInstance = m_lInstances.Find(iSound, NULL);
	if(!pInstance)
		return false;

	return pInstance->Set(What, Value);
}

bool CSoundDevice_DirectSound::Update()
{
	Refill();
	return true;
}


int32 m_LastWritePos = 0;
int64 m_SampleClock = 0; // in bytes

int64 m_SamleClockStart = -1;
bool g_WritePosWorkAround = false;


//
//
//
bool CSoundDevice_DirectSound::Refill()
{
	static int64 LastRefill = 0;
	
	int8 *apData[2] = {0};
	uint32 aSize[2] = {0};
	uint32 PlayPos;
	uint32 WritePos;

	int32 MixAhead = 44100*4;

	// get positions
	m_pBuffer->GetCurrentPosition(&PlayPos, &WritePos); // TODO: these can't be trusted!

	// update sample clock
	int64 start = m_SampleClock;
	if(WritePos == m_LastWritePos)
		return true;
	else if(WritePos > m_LastWritePos)
		m_SampleClock += WritePos - m_LastWritePos;
	else
		m_SampleClock += (m_BufferSize-m_LastWritePos) + WritePos;


	// adjust mixahead
	int64 TickCount = NFoo::GetTickCount();
	if(LastRefill != 0)
	{
		MixAhead = (int32)((TickCount-LastRefill)/(fp32)(GetTickFrequency())*1.5f*44100);
		MixAhead = Clamp(MixAhead, 1000, 44100*4); // keep sane values
	}
	LastRefill = TickCount;

	//g_SampleClock = m_SampleClock;
	//if(m_SampleClock < start)
	//	start = start;

	m_LastWritePos = WritePos;
	HRESULT lockres;
	//if(WritePos < m_LastOffset || (WritePos < 10000 && WritePos > m_LastOffset))
	//HRESULT lockres = m_pBuffer->Lock(0, g_WriteAheadSize, (void**)&apData[0], &aSize[0], 0, 0, DSBLOCK_FROMWRITECURSOR);
			//, (void**)&apData[1], &aSize[1], DSBLOCK_FROMWRITECURSOR);
	lockres = m_pBuffer->Lock(WritePos, m_MixAhead, (void**)&apData[0], &aSize[0], (void**)&apData[1], &aSize[1], 0);
	//lockres = m_pBuffer->Lock(0, m_MixAhead, (void**)&apData[0], &aSize[0], (void**)&apData[1], &aSize[1], DSBLOCK_FROMWRITECURSOR);
	//HRESULT lockres = m_pBuffer->Lock(m_LastOffset, g_WriteAheadSize, (void**)&apData[0], &aSize[0], (void**)&apData[1], &aSize[1], 0);
	//HRESULT lockres = m_pBuffer->Lock(m_LastOffset, g_WriteAheadSize, (void**)&apData[0], &aSize[0], (void**)&apData[1], &aSize[1], 0);

	if(lockres == DSERR_BUFFERLOST) NLog::WriteF("DSERR_BUFFERLOST\n");
	if(lockres == DSERR_INVALIDCALL) NLog::WriteF("DSERR_INVALIDCALL\n");
	if(lockres == DSERR_INVALIDPARAM) NLog::WriteF("DSERR_INVALIDPARAM\n");
	if(lockres == DSERR_PRIOLEVELNEEDED) NLog::WriteF("DSERR_PRIOLEVELNEEDED\n");
	//NLog::WriteF("SamplePos: %d\n", (int32)(m_SampleClock/44100));

	if(0) // sinus test
	{
		int64 Clock = m_SampleClock/4;
		for(int32 i = 0; i < 2; i++)
		{
			int16 *pDst = (int16 *)apData[i];
			if(!pDst)
				continue;

			for(int32 s = 0; s < aSize[i]/4; s++, Clock++)
			{
				int16 v = (int16)(sin((fp32)(Clock%100)/100.0f*PI*2)*32000);
				*pDst++ = v; 
				*pDst++ = v; 
			}
		}
	}
	else
	{
		if(apData[0])
			m_Puller.Fill(apData[0], m_SampleClock/4, aSize[0]/4);
		if(apData[1])
			m_Puller.Fill(apData[1], m_SampleClock/4+aSize[0]/4, aSize[1]/4);
	}

	NFoo::Sleep(10);

	m_pBuffer->Unlock(apData[0], aSize[0], apData[1], aSize[1]);

	return true;
}


//
//
//
bool CSoundDevice_DirectSound::Shutdown()
{
	// terminate thread

	return true;
}


MACRO_REGISTER_CLASS(CSoundDevice_DirectSound, NSound::aDeviceClassTag)

//#pragma comment(lib, "winmm.lib")


