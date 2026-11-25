#ifndef __FOO_LOWLEVEL_STRING_H__
#define __FOO_LOWLEVEL_STRING_H__

#include "base.h"
#include <cstddef>
#include <stdarg.h>

//# Context: Base/Strings
//# Req_Namespace: NFoo
//# Req_Header: base/string.h
//# Req_Library: foobase.lib

namespace NFoo
{
	inline bool IsAlpha(const char c)
	{
		if(c >= 'a' && c <= 'z')
			return true;
		if(c >= 'A' && c <= 'Z')
			return true;
		return false;
	}

	inline bool IsNumeric(const char c)
	{
		if(c >= '0' && c <= '9')
			return true;
		return false;
	}

	inline bool IsAlphaNumeric(const char c)
	{	
		if(IsAlpha(c) || IsNumeric(c) || c == '_')
			return true;
		return false;
	}

	inline bool IsSpace(const char c)
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
	void StringCopy(char *_pDest, const char *_pSrc);
	void StringCopy(char *_pDest, const char *_pSrc, intr _Max);
	
	//# Function: StringLength
	//# Description: Returns the length of a zero-terminated string.
	//# Parameters:
	//#		Src, String to get the length of.
	//# Returns: Returns the length of the string.
	//# Declaration:
	intr StringLength(const char *_pSrc);

	//# Function: StringCFormatArgList
	//# Remarks: MODIFIED
	//# Declaration:
	char *StringCFormatArgList(char *_pDst, size_t _DstSize, const char *_pFormat, va_list _Args);

	//# Function: StringCFormatArg
	//# Description: Performs printf formating to a string.
	//# Parameters:
	//#		Dest, A buffer that will recive the formated string.
	//#		Format, The formating string.
	//#		..., Variables used by the format string.
	//# Returns: Returns the Dest pointer passed to the function.
	//# Remarks: It's recommended that you reed the libc documentation about printf string formating
	//# Declaration:
	char *StringCFormat(char *_pDst, size_t _DstSize, const char *_pFormat, ...);

	//# Function: StringCompare
	//# Description: Compares two zero-terminated strings.
	//# Declaration:
	intr StringCompare(const char *_pDest, const char *_pSrc);
	intr StringCompare(const char *_pDest, const char *_pSrc, intr _Max);

	//# Function: StringGetInt
	//# Description: Tries to convert a zero-terminated string into an integer.
	//# Returns: Returns the integer. Zero is returned if there there was a problem.
	//# Declaration:
	int32_t StringGetInt(const char *_pDest);

	//# Function: StringGetFloat
	//# Description: Tries to convert a zero-terminated string into a float.
	//# Returns: Returns the float. Zero is returned if there there was a problem.
	//# Declaration:
	fp64 StringGetFloat(const char *_pDest);


	//# Function: StringGetHash
	//# Description: Calculates an id-number from the string.
	//# Returns: Returns the id number.
	//# Parameters:
	//#		Name, A pointer to a zero-terminated string to be used for the calculation.
	//# Declaration:
	int32_t StringGetHash(const char *_pName);
}

#endif // __FOO_LOWLEVEL_STRING_H__

