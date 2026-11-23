#include <foo/func/math.h>

fp32 NFoo::Random()
{
	static fp64 SEED = 91648253;
	static fp64 a = 1389796, m = 2147483647;
	int32 q;

	SEED *= a;
	q = (int32)(SEED / m);
	SEED -= q*m;
	return (fp32)((SEED)/m);
} 
