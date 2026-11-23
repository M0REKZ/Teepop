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
		TDynamicArray<int32> m_lStartPositions;
		//CBuffer m_Buffer;
		int32 m_SamplesLeft;

		int32 Fill(void *pData, int64 Position, int32 Samples); // should do convertion
	};

} }

#endif // FOO_FILE_EXT_SOUND_PULLER_H