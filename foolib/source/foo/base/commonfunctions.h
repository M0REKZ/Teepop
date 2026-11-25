// Part of foolib. Check docs/license.txt for licensing information.


//# Context: Base/Common Functions
//# Req_Namespace: NFoo
//# Req_Header: base/base.h
//# Req_Library: foobase.lib

#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651f
#define PI64 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651

#define NLOG 2.7182818281828f

namespace NFoo
{
	extern void DebugBreak();
}

//# Function: SafeDelete
//# Description: Performs a regular delete on a pointer.
//# Parameters:
//#		Ptr, Pointer to delete.
//# Remarks: This function first check so that the pointer isn't a NULL pointer before it delete it.
//#			 Also it set the pointer to NULL when done.
//# SeeAlso: SafeArrayDelete
//# Declaration:
template<class T> inline void SafeDelete(T &_Ptr)
{ if(_Ptr) { delete _Ptr; _Ptr = 0; } }

//# Function: SafeArrayDelete
//# Description: Performs a regular delete [] on a pointer.
//# Parameters:
//#		Ptr, Pointer to delete.
//# Remarks: This function first check so that the pointer isn't a NULL pointer before it delete it.
//#			 Also it set the pointer to NULL when done.
//# SeeAlso: SafeDelete
//# Declaration:
template<class T> inline void SafeArrayDelete(T &_Ptr)
{
	if(_Ptr)
	{
		delete [] _Ptr;
		_Ptr = 0;
	}
}

//template<class T> inline void SafeRelease(T& _Ptr) { if(_Ptr) { _Ptr->Release(); _Ptr = NULL; } }

//# Function: Assert
//# Description: Stops execution for debugging of requirements isn't met.
//# Parameters:
//#		Val, Value.
//# Declaration:
void Assert_(const char *pFile, int32_t Line, bool _bVal);
#define Assert(x) Assert_(__FILE__,__LINE__,x)

//# Function: Min
//# Description: Returns the smallest value of two chosen.
//# Parameters:
//#		Value1, First value.
//#		Value2, Second value.
//# Returns: Smallest of the two values.
//# Declaration:
template<class T> inline T Min(const T& _Value1,const T& _Value2)
{ return (_Value1 < _Value2 ? _Value1 : _Value2); }

//# Function: Max
//# Description: Returns the biggest value of two chosen.
//# Parameters:
//#		Value1, First value.
//#		Value2, Second value.
//# Returns: Biggest of the two values.
//# Declaration:
template<class T> inline T Max(const T& _Value1,const T& _Value2)
{ return (_Value1 > _Value2 ? _Value1 : _Value2); }

//# Function: Clamp
//# Description: Clamp a value to a range.
//# Parameters:
//#		Value, Value to clamp.
//#		Min, Lower range limit.
//#		Max, Higher range limit.
//# Returns: Min <= Returned Value <= Max.
template<class T1, class T2, class T3> inline T1 Clamp(T1 Value, T2 Min, T3 Max)
{ return Value < Min ? Min : (Value > Max ? Max : Value); }
