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

		int32 m_Samples;
		CFormat m_Format;
		void *m_pData;

		void CreateData(int32 Samples)
		{
			m_Samples = Samples;
			m_pData = new int8[Samples*4];
		}

		void DestroyData()
		{
			delete [] (int8 *)m_pData;
			m_pData = NULL;
		}

		void Clear()
		{
			m_Samples = 0;
			m_pData = 0;
		}
	};

} }

#endif // FOO_FILE_EXT_SOUND_BUFFER_H
