#ifndef FOO_FILE_EXT_SOUND_FORMAT_H
#define FOO_FILE_EXT_SOUND_FORMAT_H

#include <foo/base/base.h>

namespace NFoo { namespace NSound {


	class CFormat
	{
	public:
		CFormat(int32 SampleRate=44100, int32 Channels=2, int32 Bits=16)
		{
			m_SampleRate = SampleRate;
			m_Channels = Channels;
			m_Bits = Bits;
		}

		int32 m_SampleRate;
		int32 m_Channels;
		int32 m_Bits;

		int32 SampleSize() { return m_Bits/8*m_Channels; }
		int32 BitRate() { return m_Bits*m_Channels*m_SampleRate; }
	};

} }

#endif // FOO_FILE_EXT_SOUND_FORMAT_H