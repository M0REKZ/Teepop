#ifdef FOO_PLATFORM_POSIX

#include <sys/time.h>
#include <unistd.h>
#include "sys.h"
// No need to include carbon.h (slow)
//extern "C" void Microseconds(uint64 *microTickCount);
//#endif

int64 NFoo::GetTickFrequency()
{
	return 1000000;
}

int64 NFoo::GetTickCount()
{
	timeval val;
	gettimeofday(&val, NULL);
	int64 Value = (int64)val.tv_sec*(int64)1000000+(int64)val.tv_usec;
	return Value;
}
/*
int64 NFoo::GetTickFrequency()
{
	return 1000000;
}

int64 NFoo::GetTickCount()
{
	uint64 Value;
	Microseconds(&Value);
	return Value;
}
*/
void NFoo::DebugBreak()
{
}

#include <pthread.h>

// threading
NFoo::CLock::CLock()
{
	m_pPrivateData = new pthread_rwlock_t;
	pthread_rwlock_init((pthread_rwlock_t *)m_pPrivateData, NULL);
}

NFoo::CLock::~CLock()
{
	pthread_rwlock_destroy((pthread_rwlock_t *)m_pPrivateData);
	delete (pthread_rwlock_t *)m_pPrivateData;
}

void NFoo::CLock::Aquire()
{
	pthread_rwlock_wrlock((pthread_rwlock_t *)m_pPrivateData);
}
void NFoo::CLock::Release()
{
	pthread_rwlock_unlock((pthread_rwlock_t *)m_pPrivateData);
}

bool NFoo::CreateThread(void (*pfn)(void*), void *arg)
{
	pthread_t info;
	pthread_create(&info, NULL, (void *(*)(void*))(pfn), arg);
//#ifdef FOO_COMPILER_MSVC7
//	_beginthread(_pfn, 0, _Arg);
//#endif
	return true;
}

void NFoo::Sleep(int32_t _Milliseconds)
{
	usleep(_Milliseconds*1000);
}

#endif

