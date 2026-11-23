#ifndef __FOO_SPHERE_H__
#define __FOO_SPHERE_H__

#include <foo/func/vector2.h>
#include <foo/func/ray.h>

namespace NFoo {
	
	template<class T>
	class TSphere
	{
	public:
		TSphere()
		{}
	
		TSphere(TVector2<T> _Center, T _Radius)
		{
			m_Center = _Center;
			m_Radius = _Radius;
		}
	
		TVector2<T> m_Center;
		T m_Radius;
	
		template<class NEWTYPE>
		TSphere<NEWTYPE> Convert()
		{
			return TSphere<NEWTYPE>(m_Center.Convert<NEWTYPE>(), (NEWTYPE)m_Radius);
		}
	
		T Intersect(TRay<T> _Ray)
		{
			TVector2<T> Q = m_Center - _Ray.m_Org;
			T c = Q.Length();
			T v = Q.DotProduct(_Ray.m_Direction);
			T d = m_Radius*m_Radius - (c*c - v*v);
	
			if(d < 0)
				return -1;
	
			T Result = v - (fp32)sqrt(d);
			//DebugLine(_Ray.m_Org, _Ray.GetPoint(Result), 0xFFFF0080);
	
			return Result;
		}
	};
}


#endif // __FOO_SPHERE_H__

