
#ifndef __FOO_VECTOR3_H__
#define __FOO_VECTOR3_H__

#include <foo/base/base.h>
#include <foo/base/math.h>

namespace NFoo
{
	template<class T>
	class TVector3
	{
	public:
		T x, y, z;
		
		TVector3()
		{
			x = y = z = 0;
		}
	
		TVector3(T _x, T _y, T _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
	
		TVector3(const TVector3 &t)
		{
			x = t.x;
			y = t.y;
			z = t.z;
		}
		
		void Set(T _x, T _y, T _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
	
		void Set(const TVector3 &t)
		{
			x = t.x;
			y = t.y;
			z = t.z;
		}
	
		T Length() const
		{
			return (T)sqrt((fp32)(x*x + y*y + z*z));
		}
	
		T LengthSqr() const
		{
			return x*x + y*y + z*z;
		}

		void ClampLength(T _Length)
		{
			T len = Length();
	
			if(len > _Length)
			{
				Normalize();
				x *= _Length;
				y *= _Length;
				z *= _Length;
			}
		}
	
		//
		// Swap
		//
		void Swap(TVector3 &_Vec)
		{
			TVector3 Tmp = _Vec;
			_Vec = *this;
			*this = Tmp;
		}
	
		//
		// Unit
		//
		TVector3 Unit() const
		{
			return TVector3(*this).Normalize();
		}

		//
		// Normalize
		//
		const TVector3& Normalize()
		{
			T Len = Length();
	
			if (Len >= 0.000001f)
			{
				x /= Len;
				y /= Len;
				z /= Len;
			}
			else	
				x = y = z = 0;

			return *this;
		}
	
		//
		// Normalize
		//
		const TVector3& Normalize(T _Len)
		{
			if (_Len >= 0.000001f)
			{
				x /= _Len;
				y /= _Len;
				z /= _Len;
			}
			else	
				x = y = z = 0;

			return *this;
		}
	
		//
		//
		//
		T DotProduct(const TVector3 &t) const
		{
			return x*t.x + y*t.y + z*t.z;
		}

		//
		//
		//
		TVector3 CrossProduct(const TVector3 &t) const
		{
			return TVector3(y * t.z - z * t.y, 
							z * t.x - x * t.z,
							x * t.y - y * t.x);
		}

		//
		//
		//
		bool AlmostEqual(const TVector3 &t, T _Epsilon) const
		{
			if( x < t.x+_Epsilon && x > t.x-_Epsilon && y < t.y+_Epsilon && y > t.y-_Epsilon &&
				z < t.z+_Epsilon && z > t.z-_Epsilon)
				return true;

			return false;
		}

		//
		// Converters
		//
		template<class TNEWTYPE>
		TVector3<TNEWTYPE> Convert()				const	{ return TVector3<TNEWTYPE>((TNEWTYPE)x, (TNEWTYPE)y, (TNEWTYPE)z); }

		//
		//
		//
		TVector3 Round()							const	{ return Convert<int32>().Convert<T>(); }
	
		//
		// Operators
		//
		T& operator [] (int32 _Index)						{ return *(&x + _Index); }

		int32 operator == (const TVector3 &t)		const	{ return ((x == t.x) && (y == t.y) && (z == t.z)); }
		int32 operator != (const TVector3 &t)		const	{ return ((x != t.x) || (y != t.y) || (z != t.z)); }
	
		void operator = (const TVector3 &t)					{ Set(t); }
	
		void operator += (const TVector3 &t) 				{ x += t.x; y += t.y; z += t.z; }
		void operator -= (const TVector3 &t) 				{ x -= t.x; y -= t.y; z -= t.z; }
		void operator *= (const TVector3 &t) 				{ x *= t.x; y *= t.y; z *= t.z; }
		void operator /= (const TVector3 &t) 				{ x /= t.x; y /= t.y; z /= t.z; }
		
		void operator += (const T t) 						{ x += t; y += t; z += t; }
		void operator -= (const T t) 						{ x -= t; y -= t; z -= t; }
		void operator *= (const T t) 						{ x *= t; y *= t; z *= t; }
		void operator /= (const T t) 						{ x /= t; y /= t; z /= t; }
	
		TVector3 operator - ()						const	{ return TVector3(-x, -y, -z); }
		TVector3 operator + (const TVector3 &t)		const 	{ return TVector3(x + t.x, y + t.y, z + t.z); }
		TVector3 operator - (const TVector3 &t)		const 	{ return TVector3(x - t.x, y - t.y, z - t.z); }
		TVector3 operator * (const TVector3 &t)		const 	{ return TVector3(x * t.x, y * t.y, z * t.z); }
		TVector3 operator / (const TVector3 &t)		const 	{ return TVector3(x / t.x, y / t.y, z / t.z); }
			
		TVector3 operator + (const T t) 			const 	{ return TVector3(x + t, y + t, z + t); }
		TVector3 operator - (const T t) 			const 	{ return TVector3(x - t, y - t, z - t); }
		TVector3 operator * (const T t) 			const 	{ return TVector3(x * t, y * t, z * t); }
		TVector3 operator / (const T t) 			const 	{ return TVector3(x / t, y / t, z / t); }
	};

	//
	// Default Vector Type
	//
	//#define CVector2D TVector2<fp4>
	//#define CVector2Di TVector2<int32>

} // namespaces

#endif // __FOO_VECTOR3_H__

