#ifndef __FOO_BYTETOOLS_H__
#define __FOO_BYTETOOLS_H__

//# Context: ByteTools
//# Req_Namespace: NFoo
//# Req_Header: base/base.h
//# Req_Library: foobase.lib

#include "base.h"

#ifdef FOO_CPU_BYTEORDER_BIGENDIAN
	inline bool IsCPUBigEndian() { return true; }
	inline bool IsCPUSmallEndian() { return false; }
#elif defined(FOO_CPU_BYTEORDER_LITTLEENDIAN)
	inline bool IsCPUBigEndian() { return false; }
	inline bool IsCPUSmallEndian() { return true; }
#else
	#error "No CPU definition, can't generate swap functions"
#endif


template<class T> inline void ByteSwap16ref(T &x)
{
	x = (x>>8)|((x&0xFF)<<8);
}

template<class T> inline T ByteSwap16(const T y)
{
	uint16 x = *((uint16*)&y);
	ByteSwap16ref(x);
	return *((T*)&x);
}

template<class T> inline void ByteSwap32ref(T &x)
{
	x = ((x>>24)|((x&0xFF0000)>>8)|((x&0xFF00)<<8)|((x&0xFF)<<24));
}

template<class T> inline T ByteSwap32(const T y)
{
	uint32_t x = *((uint32_t*)&y);
	ByteSwap32ref(x);
	return *((T*)&x);
}

 
// size sensitive swapper
template<class T, int S>
class TByteSwap
{
public:
	static T Swap(T _Value) { return _Value; }
};
 
template<class T>
class TByteSwap<T,2>
{ public:
	static T Swap(T _Value) { return ByteSwap16(_Value); }
};
 
template<class T>
class TByteSwap<T,4>
{ public:
	static T Swap(T _Value) { return ByteSwap32(_Value); }
};
 
// swapper function
template<class T>
T ByteSwap(T _Value)
{
	return TByteSwap<T,sizeof(T)>::Swap(_Value);
}

// swapper function
template<class T>
void ByteSwapref(T &Value)
{
	Value = TByteSwap<T,sizeof(T)>::Swap(Value);
}

/*
inline fp4 Swapfp4(const fp4 y)
{
	uint32_t x = *((uint32_t*)&y);
	x = ((x>>24)|((x&0xFF0000)>>8)|((x&0xFF00)<<8)|((x&0xFF)<<24));
	return *((fp4*)&x);
}
*/

#ifdef FOO_CPU_BYTEORDER_BIGENDIAN
	template<class T> T ByteSwapLE(T _Value) { return ByteSwap(_Value); }
	template<class T> T ByteSwapBE(T _Value) { return _Value; }
#elif defined(FOO_CPU_BYTEORDER_LITTLEENDIAN)
	template<class T> T ByteSwapLE(T _Value) { return _Value; }
	template<class T> T ByteSwapBE(T _Value) { return ByteSwap(_Value); }
#else
	#error "No CPU definition, can't generate swap functions"
#endif
/*
#ifdef FOO_CPU_BYTEORDERFLOAT_BIGENDIAN
	inline fp32 Swapfp32LE(const fp32 x) { return Swap32(x); }
	inline fp32 Swapfp32BE(const fp32 x) { return x; }
	inline fp64 Swapfp64LE(const fp64 x) { return x; }
	inline fp64 Swapfp64BE(const fp64 x) { return x; }
#elif defined(FOO_CPU_BYTEORDERFLOAT_LITTLEENDIAN)
	inline fp32 Swapfp32LE(const fp32 x) { return x; }
	inline fp32 Swapfp32BE(const fp32 x) { return Swap32(x); }
	inline fp64 Swapfp64LE(const fp64 x) { return x; }
	inline fp64 Swapfp64BE(const fp64 x) { return x; }
#else
	#error "No CPU definition, can't generate swap functions"
#endif*/

//# Function: ByteSwap
//# Description: Performs a byteswap operations.
//# Parameters:
//#		x, Value to swap.
//# Returns: Swapped balue
//# Declaration:
template<class T> inline const T ByteSwapLEtoCPU(const T x) { return ByteSwapLE(x); }
//# Declaration:
template<class T> inline const T ByteSwapBEtoCPU(const T x) { return ByteSwapBE(x); }
//# Declaration:
template<class T> inline const T ByteSwapCPUtoLE(const T x) { return ByteSwapLE(x); }
//# Declaration:
template<class T> inline const T ByteSwapCPUtoBE(const T x) { return ByteSwapBE(x); }



#endif
