#include <foo/core/core.h>
#include <foo/func/classregistry.h>

#include "format.h"
#include "source.h"

namespace NFoo { namespace NSound {

	const int8 aDeviceClassTag[] = "Sound Device";

	enum
	{
		ESoundFlag_AutoDestroy=1
	};

	//
	//
	//
	class CSoundDevice : public NCore::CSubCore
	{
	public:
		virtual ~CSoundDevice() {}
		bool Init(int32_t SampleRate, int32_t Channels, int32_t Bits, int32_t BufferSize)
		{
			CFormat Format;
			Format.m_SampleRate = SampleRate;
			Format.m_Channels = Channels;
			Format.m_Bits = Bits;
			return Init(Format, BufferSize);
		}

		virtual bool Init(CFormat Format, int32_t BufferSize) pure;
		virtual bool Shutdown() pure;
		virtual bool Update() pure;

		virtual int32_t Play(CSource *pSource, int32_t Flags=ESoundFlag_AutoDestroy) pure;
		virtual void Destroy(int32_t iSound) pure;
		
		virtual bool Set(int32_t iSound, int32_t What, int32_t Value) pure;

		/*
		virtual bool IsInstance(int32_t iSound);

		virtual void SetRate(int32_t iSound, fp32 Rate);

		virtual void Pause(int32_t iSound);
		virtual void Start(int32_t iSound);
		*/
	};
} }