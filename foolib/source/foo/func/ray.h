#ifndef __FOO_RAY_H__
#define __FOO_RAY_H__

#include <foo/func/vector2.h>

//using namespace NI;
namespace NFoo {
	
	template<class T>
	class TRay
	{
	public:
		TVector2<T> m_Org;
		TVector2<T> m_Direction;
	
		TRay()
		{}
	
		TRay(TVector2<T> _Org, TVector2<T> _Direction)
		{
			m_Org = _Org;
			m_Direction = _Direction;
		}
	
		TVector2<T> GetPoint(T _Time)
		{
			return m_Org+m_Direction*_Time;
		}
	
		template<class NEWTYPE>
		TRay<NEWTYPE> Convert()
		{
			return TRay<NEWTYPE>(m_Org.Convert<NEWTYPE>(), m_Direction.Convert<NEWTYPE>());
		}
	};

	template<class T>
	TVector2<T> ClosestPointOnLine(TVector2<T> _LinePoint1, TVector2<T> _LinePoint2, TVector2<T> _Point)
	{
	   // Determine t (the length of the vector from ¾a® to ¾p®)
	   TVector2<T> c = _Point - _LinePoint1;
	   TVector2<T> V = (_LinePoint2 - _LinePoint1);
	   V.Normalize();
	   T d = (_LinePoint1-_LinePoint2).Length();
	   T t = V.DotProduct(c);
	   
	   // Check to see if ¾t® is beyond the extents of the line segment
	   if (t < 0) return _LinePoint1;
	   if (t > d) return _LinePoint2;
	 
	   // Return the point between ¾a® and ¾b®
	   V = V * t;
	   return _LinePoint1 + V;
	}

}

#endif // __NI_RAY__

