
#include "sample.h"

using namespace NFoo;
using namespace NFoo::NSound;

CInstance *CSample::CreateInstance(CSoundDevice *pDevice)
{
	return new CSampleInstance(this, pDevice);
}

