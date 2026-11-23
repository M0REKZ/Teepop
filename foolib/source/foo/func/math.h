#ifndef __FOO_FUNC_MATH_H__
#define __FOO_FUNC_MATH_H__

#include <foo/base/base.h>

namespace NFoo
{
	template<class T>
	T Lerp(T _Src, T _Dst, fp32 _Amount)
	{
		return (T)(_Src + (_Dst-_Src)*_Amount);
	}

	template<class T>
	T Bezier(T _Value1, T _Value2, T _Value3, fp32 _Amount)
	{
		return (T)(_Value1*(_Amount*_Amount) + _Value2*(2*_Amount*(1-_Amount)) + _Value3*((1-_Amount)*(1-_Amount)));
	}
	
	fp32 Random();
}

#endif // __FOO_FUNC_MATH_H__

