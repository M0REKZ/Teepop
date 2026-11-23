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
		bool Init(int32 SampleRate, int32 Channels, int32 Bits, int32 BufferSize)
		{
			CFormat Format;
			Format.m_SampleRate = SampleRate;
			Format.m_Channels = Channels;
			Format.m_Bits = Bits;
			return Init(Format, BufferSize);
		}

		virtual bool Init(CFormat Format, int32 BufferSize) pure;
		virtual bool Shutdown() pure;
		virtual bool Update() pure;

		virtual int32 Play(CSource *pSource, int32 Flags=ESoundFlag_AutoDestroy) pure;
		virtual void Destroy(int32 iSound) pure;
		
		virtual bool Set(int32 iSound, int32 What, int32 Value) pure;

		/*
		virtual bool IsInstance(int32 iSound);

		virtual void SetRate(int32 iSound, fp32 Rate);

		virtual void Pause(int32 iSound);
		virtual void Start(int32 iSound);
		*/
	};
} }