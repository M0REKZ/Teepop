#ifndef __FOO_MATRIX4_H__
#define __FOO_MATRIX4_H__

#include <foo/base/base.h>
#include <foo/base/math.h>
#include <foo/func/vector3.h>


namespace NFoo
{

	//
	//
	//
	template<class T>
	class TMatrix4
	{
	public:
		T k[4][4];

		//
		TMatrix4()
		{}

		TVector3<T> GetRow(const int _Index) const
		{
			return TVector3<T>(k[_Index][0], k[_Index][1], k[_Index][2]);
		}

		TVector3<T> GetDir() const { return GetRow(0); }
		TVector3<T> GetPos() const { return GetRow(3); }

		//
		void Unit()
		{
			k[0][0] = k[0][1] = k[0][2] = k[0][3] = 0;
			k[1][0] = k[1][1] = k[1][2] = k[1][3] = 0;
			k[2][0] = k[2][1] = k[2][2] = k[2][3] = 0;
			k[3][0] = k[3][1] = k[3][2] = k[3][3] = 0;

			k[0][0] = k[1][1] = k[2][2] = k[3][3] = 1;
		}

		//
		TVector3<T> operator *(const TVector3<T> &_Vector) const
		{
			TVector3<T> Result(0,0,0);

			Result.x += _Vector.x*k[0][0] + _Vector.y*k[1][0] + _Vector.z*k[2][0] + k[3][0];
			Result.y += _Vector.x*k[0][1] + _Vector.y*k[1][1] + _Vector.z*k[2][1] + k[3][1];
			Result.z += _Vector.x*k[0][2] + _Vector.y*k[1][2] + _Vector.z*k[2][2] + k[3][2];
			return Result;
		}

		//
		TMatrix4 operator *(const TMatrix4 &_Matrix) const
		{
			TMatrix4 Result;

			for(int32 i = 0; i < 4; i++)
				for(int32 j = 0; j < 4; j++)
				{
					Result.k[i][j] = 0;
					for(int32 a = 0; a < 4; a++)
						Result.k[i][j] += k[a][j] * _Matrix.k[i][a];
				}
			return Result;
		}

		//
		void RotX(T _Angle)
		{
			T sina = (T)sin(_Angle);
			T cosa = (T)cos(_Angle);

			Unit();
			k[1][1] = cosa; k[2][1] =-sina;
			k[1][2] = sina; k[2][2] = cosa;
		}

		//
		void RotY(T _Angle)
		{
			T sina = (T)sin(_Angle);
			T cosa = (T)cos(_Angle);

			Unit();
			k[0][0] = cosa; k[2][0] =-sina;
			k[0][2] = sina; k[2][2] = cosa;
		}

		//
		void RotZ(T _Angle)
		{
			T sina = (T)sin(_Angle);
			T cosa = (T)cos(_Angle);

			Unit();
			k[0][0] = cosa; k[1][0] =-sina;
			k[0][1] = sina; k[1][1] = cosa;
		}

		//
		TMatrix4 Transposed()
		{
			TMatrix4 Result;

			for(int32 i = 0; i < 4; i++)
				for(int32 j = 0; j < 4; j++)
					Result.k[i][j] = k[j][i];
			return Result;
		}
	};
}

#endif // __FOO_MATRIX4_H__

