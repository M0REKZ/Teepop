
#ifndef __FOO_VECTOR2_H__
#define __FOO_VECTOR2_H__

//#include <foo/sys/posix.h>
#include <foo/base/math.h>
#include <foo/base/base.h>


//extern "C" fp8 atan2(fp8, fp8);

namespace NFoo
{
	template<class T>
	class TVector2
	{
	public:
		T x, y;
		
		TVector2()
		{
			x = y = 0;
		}

		TVector2(T _Val)
		{
			x = y = _Val;
		}
	
		TVector2(T _x, T _y)
		{
			x = _x;
			y = _y;
		}
	
		TVector2(const TVector2 &t)
		{
			x = t.x;
			y = t.y;
		}
		
		void Set(T _x, T _y)
		{
			x = _x;
			y = _y;
		}
	
		void Set(const TVector2 &t)
		{
			x = t.x;
			y = t.y;
		}
	
		T Length() const
		{
			return (T)sqrt((fp32)(x*x + y*y));
		}
	
		T LengthSqr() const
		{
			return x*x + y*y;
		}
	
		void ClampLength(T _Length)
		{
			T Len = Length();
	
			if(Len > _Length)
			{
				Normalize();
				x *= _Length;
				y *= _Length;
			}
		}
	
		//
		// Angles
		//
		fp32 GetAngle() const
		{
			return (fp32)atan2((fp32)y, (fp32)x);
		}
	
		void SetAngle(T a)
		{
			x = (T)cos((fp32)a);
			y = (T)sin((fp32)a);
		}
	
		//
		// Rotation
		//
		void Rotate(T _Angle)
		{
			T Temp = x;
			x = (T)cos((fp32)_Angle)*x - (T)sin((fp32)_Angle)*y;
			y = (T)sin((fp32)_Angle)*Temp + (T)cos((fp32)_Angle)*y;
		}
	
		//
		// Swap
		//
		void Swap(TVector2 &_Vec)
		{
			TVector2 Tmp = _Vec;
			_Vec = *this;
			*this = Tmp;
		}
	
		//
		// Perpendicular
		//
		TVector2 Perp() const
		{
			return TVector2(y,-x);
		}
	
		//
		// Unit
		//
		TVector2 Unit() const
		{
			return TVector2(*this).Normalize();
		}

		//
		// Normalize
		//
		const TVector2& Normalize()
		{
			T Len = Length();
	
			if (Len >= 0.000001f)
			{
				x /= Len;
				y /= Len;
			}
			else	
				x = y = 0;

			return *this;
		}
	
		//
		// Normalize
		//
		const TVector2&  Normalize(T _Len)
		{
			if (_Len >= 0.000001f)
			{
				x /= _Len;
				y /= _Len;
			}
			else	
				x = y = 0;

			return *this;
		}
	
		//
		//
		//
		T DotProduct(const TVector2 &t) const
		{
			return x*t.x + y*t.y;
		}

		//
		//
		//
		bool AlmostEqual(const TVector2 &t, T _Epsilon) const
		{
			if( x < t.x+_Epsilon && x > t.x-_Epsilon && y < t.y+_Epsilon && y > t.y-_Epsilon)
				return true;

			return false;
		}

		//
		// Converters
		//
		template<class TNEWTYPE>
		TVector2<TNEWTYPE> Convert()				const	{ return TVector2<TNEWTYPE>((TNEWTYPE)x, (TNEWTYPE)y); }

		//
		//
		//
		TVector2 Round()							const	{ return Convert<int32_t>().template Convert<T>(); }
		
		//
		// Operators
		//
		T& operator [] (int32_t _Index)						{ return *(&x + _Index); }

		int32_t operator == (const TVector2 &t)		const	{ return ((x == t.x) && (y == t.y)); }
		int32_t operator != (const TVector2 &t)		const	{ return ((x != t.x) || (y != t.y)); }
	
		void operator = (const TVector2 &t)					{ Set(t); }

		void operator += (const TVector2 &t) 				{ x += t.x, y += t.y; }
		void operator -= (const TVector2 &t) 				{ x -= t.x, y -= t.y; }
		void operator *= (const TVector2 &t) 				{ x *= t.x, y *= t.y; }
		void operator /= (const TVector2 &t) 				{ x /= t.x, y /= t.y; }
		
		void operator += (const T t) 						{ x += t; y += t; }
		void operator -= (const T t) 						{ x -= t; y -= t; }
		void operator *= (const T t) 						{ x *= t; y *= t; }
		void operator /= (const T t) 						{ x /= t; y /= t; }
	
		TVector2 operator - ()						const	{ return TVector2(-x, -y); }
		TVector2 operator + (const TVector2 &t)		const	{ return TVector2(x + t.x, y + t.y); }
		TVector2 operator - (const TVector2 &t)		const	{ return TVector2(x - t.x, y - t.y); }
		TVector2 operator * (const TVector2 &t)		const	{ return TVector2(x * t.x, y * t.y); }
		TVector2 operator / (const TVector2 &t)		const	{ return TVector2(x / t.x, y / t.y); }
	
		TVector2 operator + (const T t) 			const	{ return TVector2(x + t, y + t); }
		TVector2 operator - (const T t) 			const	{ return TVector2(x - t, y - t); }
		TVector2 operator * (const T t) 			const	{ return TVector2(x * t, y * t); }
		TVector2 operator / (const T t) 			const	{ return TVector2(x / t, y / t); }
	};
} // namespace

#endif // __FOO_VECTOR2_H__

