#ifdef FOO_LOWLEVEL_MATH_LIBC

#include "math.h"
#include <math.h>

namespace std {}
using namespace std;

fp32 NFoo::sin(const fp32 _a) { return ::sin(_a); }
fp32 NFoo::cos(const fp32 _a) { return ::cos(_a); }
fp32 NFoo::tan(const fp32 _a) { return ::tan(_a); }

fp32 NFoo::asin(const fp32 _a) { return ::asin(_a); }
fp32 NFoo::acos(const fp32 _a) { return ::acos(_a); }
fp32 NFoo::atan(const fp32 _a) { return ::atan(_a); }
fp32 NFoo::atan2(const fp32 _a, const fp32 _b){ return ::atan2(_a,_b); }

fp32 NFoo::sqrt(const fp32 _a){ return ::sqrt(_a); }
fp32 NFoo::abs(const fp32 _a){ return ::fabs(_a); }
fp32 NFoo::floor(const fp32 _a){ return ::floor(_a); }
fp32 NFoo::pow(const fp32 _a, const fp32 _b){ return ::pow(_a, _b); }

#endif
