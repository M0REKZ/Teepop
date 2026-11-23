
#ifndef __FOO_VECTOR4_H__
#define __FOO_VECTOR4_H__

#include <foo/base/base.h>
#include <foo/base/math.h>

namespace NFoo
{
	template<class T>
	class TVector4
	{
	public:
		T x, y, z, w;
		
		TVector4()
		{
		}
	
		TVector4(T _x, T _y, T _z, T _w)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
	
		TVector4(const TVector4 &t)
		{
			x = t.x;
			y = t.y;
			z = t.z;
			w = t.w;
		}
		
		void Set(T _x, T _y, T _z, T _w)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
	
		void Set(const TVector4 &t)
		{
			x = t.x;
			y = t.y;
			z = t.z;
			w = t.w;
		}
	
		//
		// Swap
		//
		void Swap(TVector4 &_Vec)
		{
			TVector4 Tmp = _Vec;
			_Vec = *this;
			*this = Tmp;
		}
	
		T Length() const
		{
			return (T)sqrt((fp32)(x*x + y*y + z*z + w*w));
		}

		T LengthSqr() const
		{
			return x*x + y*y + z*z + w*w;
		}

		//
		// Unit
		//
		TVector4 Unit() const
		{
			return TVector4(*this).Normalize();
		}

		//
		// Normalize
		//
		const TVector4& Normalize()
		{
			T Len = Length();
	
			if (Len >= 0.000001f)
			{
				x /= Len;
				y /= Len;
				z /= Len;
				w /= Len;
			}
			else	
				x = y = z = w = 0;

			return *this;
		}
	
		//
		// Normalize
		//
		const TVector4& Normalize(T _Len)
		{
			if (_Len >= 0.000001f)
			{
				x /= _Len;
				y /= _Len;
				z /= _Len;
				w /= _Len;
			}
			else	
				x = y = z = w = 0;

			return *this;
		}

		//
		//
		//
		T DotProduct(const TVector4 &t) const
		{
			return x*t.x + y*t.y + z*t.z;
		}

		//
		//
		bool AlmostEqual(const TVector4 &t, T _Epsilon) const
		{
			if( x < t.x+_Epsilon && x > t.x-_Epsilon && y < t.y+_Epsilon && y > t.y-_Epsilon &&
				y < t.z+_Epsilon && y > t.z-_Epsilon && y < t.w+_Epsilon && y > t.w-_Epsilon)
				return true;

			return false;
		}

		//
		// Converters
		//
		template<class TNEWTYPE>
		TVector4<TNEWTYPE> Convert()				const	{ return TVector4<TNEWTYPE>((TNEWTYPE)x, (TNEWTYPE)y, (TNEWTYPE)z, (TNEWTYPE)w); }

		//
		TVector4 Round()							const	{ return Convert<int32>().template Convert<T>(); }
		
		//
		// Operators
		//
		T& operator [] (int32 _Index)						{ return *(&x + _Index); }
		const T& operator [] (int32 _Index)	const			{ return *(&x + _Index); }

		int32 operator == (const TVector4 &t)		const	{ return ((x == t.x) && (y == t.y) && (z == t.z) && (w == t.w)); }
		int32 operator != (const TVector4 &t)		const	{ return ((x != t.x) || (y != t.y) || (z != t.z) && (w != t.w)); }

		void operator = (const TVector4 &t)					{ Set(t); }
	
		void operator += (const TVector4 &t) 				{ x += t.x; y += t.y; z += t.z; w += t.w; }
		void operator -= (const TVector4 &t) 				{ x -= t.x; y -= t.y; z -= t.z; w -= t.w; }
		void operator *= (const TVector4 &t) 				{ x *= t.x; y *= t.y; z *= t.z; w *= t.w; }
		void operator /= (const TVector4 &t) 				{ x /= t.x; y /= t.y; z /= t.z; w /= t.w; }
		
		void operator += (const T t) 						{ x += t; y += t; z += t; w += t; }
		void operator -= (const T t) 						{ x -= t; y -= t; z -= t; w += t; }
		void operator *= (const T t) 						{ x *= t; y *= t; z *= t; w += t; }
		void operator /= (const T t) 						{ x /= t; y /= t; z /= t; w += t; }
	
		TVector4 operator - ()						const	{ return TVector4(-x, -y, -z, -w); }
		TVector4 operator + (const TVector4 &t)		const 	{ return TVector4(x + t.x, y + t.y, z + t.z, w + t.w); }
		TVector4 operator - (const TVector4 &t)		const 	{ return TVector4(x - t.x, y - t.y, z - t.z, w - t.w); }
		TVector4 operator * (const TVector4 &t)		const 	{ return TVector4(x * t.x, y * t.y, z * t.z, w * t.w); }
		TVector4 operator / (const TVector4 &t)		const 	{ return TVector4(x / t.x, y / t.y, z / t.z, w / t.w); }
	
		TVector4 operator + (const T t) 			const	{ return TVector4(x + t, y + t, z + t, w + t); }
		TVector4 operator - (const T t) 			const	{ return TVector4(x - t, y - t, z - t, w - t); }
		TVector4 operator * (const T t) 			const	{ return TVector4(x * t, y * t, z * t, w * t); }
		TVector4 operator / (const T t) 			const	{ return TVector4(x / t, y / t, z / t, w / t); }
	};
} // namespaces

#endif // __FOO_VECTOR3_H__

