#ifndef FOO_FILE_EXT_SOUND_PULLER_H
#define FOO_FILE_EXT_SOUND_PULLER_H

#include "buffer.h"
#include "instance.h"

namespace NFoo { namespace NSound {

	//
	//
	//
	class CPuller
	{
	public:
		CPuller(CInstance *pInstance = NULL);
		~CPuller();

		CInstance *m_pInstance;
		TDynamicArray<CBuffer> m_lBuffers;
		TDynamicArray<int32_t> m_lStartPositions;
		//CBuffer m_Buffer;
		int32_t m_SamplesLeft;

		int32_t Fill(void *pData, int64_t Position, int32_t Samples); // should do convertion
	};

} }

#endif // FOO_FILE_EXT_SOUND_PULLER_H