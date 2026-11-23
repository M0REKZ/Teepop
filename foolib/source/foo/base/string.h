#ifndef __FOO_LOWLEVEL_STRING_H__
#define __FOO_LOWLEVEL_STRING_H__

#include "base.h"

//# Context: Base/Strings
//# Req_Namespace: NFoo
//# Req_Header: base/string.h
//# Req_Library: foobase.lib

namespace NFoo
{
	inline bool IsAlpha(const int8 c)
	{
		if(c >= 'a' && c <= 'z')
			return true;
		if(c >= 'A' && c <= 'Z')
			return true;
		return false;
	}

	inline bool IsNumeric(const int8 c)
	{
		if(c >= '0' && c <= '9')
			return true;
		return false;
	}

	inline bool IsAlphaNumeric(const int8 c)
	{	
		if(IsAlpha(c) || IsNumeric(c) || c == '_')
			return true;
		return false;
	}

	inline bool IsSpace(const int8 c)
	{
		if(c == '\t' || c == ' ')
			return true;
		return false;
	}


	//# Function: StringCopy
	//# Description: Copies a zero-terminated string from _pSrc to _pDest.
	//# Parameters:
	//#		Dest, Destination. Must beable to hold the compleate source string.
	//#		Src, Source stringHeight if the window.
	//# 	Max, Maximum length of string.
	//# Declaration:
	void StringCopy(int8 *_pDest, const int8 *_pSrc);
	void StringCopy(int8 *_pDest, const int8 *_pSrc, intr _Max);
	
	//# Function: StringLength
	//# Description: Returns the length of a zero-terminated string.
	//# Parameters:
	//#		Src, String to get the length of.
	//# Returns: Returns the length of the string.
	//# Declaration:
	intr StringLength(const int8 *_pSrc);

	//# Function: StringCFormatArgList
	//# Remarks: the args must be right after the format string
	//# Declaration:
	int8 *StringCFormatArgList(int8 *_pDst, const int8 **_pFormat);

	//# Function: StringCFormatArg
	//# Description: Performs printf formating to a string.
	//# Parameters:
	//#		Dest, A buffer that will recive the formated string.
	//#		Format, The formating string.
	//#		..., Variables used by the format string.
	//# Returns: Returns the Dest pointer passed to the function.
	//# Remarks: It's recommended that you reed the libc documentation about printf string formating
	//# Declaration:
	int8 *StringCFormat(int8 *_pDest, const int8 *_pFormat, ...);

	//# Function: StringCompare
	//# Description: Compares two zero-terminated strings.
	//# Declaration:
	intr StringCompare(const int8 *_pDest, const int8 *_pSrc);
	intr StringCompare(const int8 *_pDest, const int8 *_pSrc, intr _Max);

	//# Function: StringGetInt
	//# Description: Tries to convert a zero-terminated string into an integer.
	//# Returns: Returns the integer. Zero is returned if there there was a problem.
	//# Declaration:
	int32 StringGetInt(const int8 *_pDest);

	//# Function: StringGetFloat
	//# Description: Tries to convert a zero-terminated string into a float.
	//# Returns: Returns the float. Zero is returned if there there was a problem.
	//# Declaration:
	fp64 StringGetFloat(const int8 *_pDest);


	//# Function: StringGetHash
	//# Description: Calculates an id-number from the string.
	//# Returns: Returns the id number.
	//# Parameters:
	//#		Name, A pointer to a zero-terminated string to be used for the calculation.
	//# Declaration:
	int32 StringGetHash(const int8 *_pName);
}

#endif // __FOO_LOWLEVEL_STRING_H__

