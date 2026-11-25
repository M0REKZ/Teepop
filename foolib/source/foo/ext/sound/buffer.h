#ifndef FOO_FILE_EXT_SOUND_BUFFER_H
#define FOO_FILE_EXT_SOUND_BUFFER_H

#include "format.h"

namespace NFoo { namespace NSound {

	class CBuffer
	{
	public:
		CBuffer()
		{
			m_pData = NULL;
			m_Samples = 0;
		}

		int32_t m_Samples;
		CFormat m_Format;
		void *m_pData;

		void CreateData(int32_t Samples)
		{
			m_Samples = Samples;
			m_pData = new int8[Samples*4];
		}

		void DestroyData()
		{
			SafeArrayDelete(m_pData);
		}

		void Clear()
		{
			m_Samples = 0;
			m_pData = 0;
		}
	};

} }

#endif // FOO_FILE_EXT_SOUND_BUFFER_H
