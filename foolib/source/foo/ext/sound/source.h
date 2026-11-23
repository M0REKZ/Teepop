#ifndef FOO_FILE_EXT_SOUND_SOURCE_H
#define FOO_FILE_EXT_SOUND_SOURCE_H

#include <foo/func/dynamicarray.h>
#include "format.h"

namespace NFoo { namespace NSound {

	class CSoundDevice;
	class CInstance;

	//
	//
	class CSource
	{
	public:
		CSource()
		{}

		enum
		{
			EAction_Loop,
			EAction_Destroy
		};

		enum
		{
			ESet_Volume=0,
			ESet_End
		};


		CFormat m_Format;
		//TDynamicArray<class CInstance *> m_lInstances;

		virtual CInstance *CreateInstance(CSoundDevice *pDevice) pure;
	};

} }

#endif // FOO_FILE_EXT_SOUND_SOURCE_H