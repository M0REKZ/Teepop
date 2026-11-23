#ifndef __FOO_PLANE2_H__
#define __FOO_PLANE2_H__

#include <foo/func/vector2.h>
#include <foo/func/ray.h>
#include <foo/func/sphere.h>
#include <foo/func/math.h>

namespace NFoo {
	
	template<class T>
	class TPlane2
	{
	public:
		typedef TVector2<T> CVector;
	
		CVector m_Normal;
		CVector m_Center;
	
		TPlane2()
		{}
	
		TPlane2(CVector _Point1, CVector _Point2)
		{
			m_Center = Lerp(_Point1, _Point2, 0.5f);
			m_Normal = (_Point1-_Point2).Perp();
			m_Normal.Normalize();
		}
	
		TVector2<T> ClosestPoint(TVector2<T> _Point)
		{
			// Determine t (the length of the vector from ¾a® to ¾p®)
			TVector2<T> c = _Point - m_Center;
			TVector2<T> V = m_Normal.Perp();
			T t = V.DotProduct(c);
			V = V * t;
			return m_Center + V;
		}

		bool PointInFront(TVector2<T> _Point)
		{
			if(m_Normal.DotProduct(_Point - m_Center))
				return true;
			return false;
		}
	
		T Intersect(TRay<T> _Ray)
		{
			T d = -(m_Normal.DotProduct(m_Center));
			T numer = m_Normal.DotProduct(_Ray.m_Org)+d;
			T denom = m_Normal.DotProduct(_Ray.m_Direction);
			T result = -(numer / denom);
			return result;
		}
	
		TRay<T> GetSphereIntersectRay(TSphere<T> _Sphere, TVector2<T> _Direction)
		{
			return TRay<T>(_Sphere.m_Center+(m_Normal*-1)*_Sphere.m_Radius, _Direction);
		}
	};
}

#endif // __FOO_PLANE2_H__