#ifndef FOO_FILE_EXT_SOUND_FORMAT_H
#define FOO_FILE_EXT_SOUND_FORMAT_H

#include <foo/base/base.h>

namespace NFoo { namespace NSound {


	class CFormat
	{
	public:
		CFormat(int32_t SampleRate=44100, int32_t Channels=2, int32_t Bits=16)
		{
			m_SampleRate = SampleRate;
			m_Channels = Channels;
			m_Bits = Bits;
		}

		int32_t m_SampleRate;
		int32_t m_Channels;
		int32_t m_Bits;

		int32_t SampleSize() { return m_Bits/8*m_Channels; }
		int32_t BitRate() { return m_Bits*m_Channels*m_SampleRate; }
	};

} }

#endif // FOO_FILE_EXT_SOUND_FORMAT_H