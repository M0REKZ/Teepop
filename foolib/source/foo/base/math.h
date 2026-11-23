#ifndef __FOO_LOWLEVEL_MATH_H__
#define __FOO_LOWLEVEL_MATH_H__

#include "base.h"

//# Context: Base/Math
//# Req_Namespace: NFoo
//# Req_Header: base/base.h
//# Req_Library: foobase.lib

namespace NFoo
{
	//# Function: sin
	//# Declaration:
	fp32 sin(const fp32 _a);
	//# Function: cos
	//# Declaration:
	fp32 cos(const fp32 _a);
	//# Function: tan
	//# Declaration:
	fp32 tan(const fp32 _a);

	//# Function: asin
	//# Declaration:
	fp32 asin(const fp32 _a);
	//# Function: acos
	//# Declaration:
	fp32 acos(const fp32 _a);
	//# Function: atan
	//# Declaration:
	fp32 atan(const fp32 _a);
	//# Function: atan2
	//# Declaration:
	fp32 atan2(const fp32 _a, const fp32 _b);

	//# Function: sqrt
	//# Declaration:
	fp32 sqrt(const fp32 _a);
	//# Function: abs
	//# Declaration:
	fp32 abs(const fp32 _a);
	//# Function: floor
	//# Declaration:
	fp32 floor(const fp32 _a);
	//# Function: pow
	//# Declaration:
	fp32 pow(const fp32 _a, const fp32 _b);
}

#endif // __FOO_LOWLEVEL_MATH_H__
