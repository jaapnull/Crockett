#pragma once
#include "../CCore/String.h"
#include "../CCore/Array.h"

//utility functions (char-string only)

bool		gIsNumeric(const char& inChar);
bool		gIsWhiteSpace(const char& inChar);
char		gToUpper(const char& inChar);
void		gToUpper(String& inString);
char		gToLower(const char& inChar);
void		gToLower(String& inString);

float		gStringToFloat(const String& inString);
int			gStringToInt(const String& inString);
uint		gStringToUInt(const String& inString);
String		gToString(uint inUint);
String		gToString(double inDouble);
String		gToString(float inFloat);

template<class T>
void gExplodeString(Array<BaseString<T>>& outParts, const BaseString<T>& inString, const T& inLimiter)
{
	uint begin = 0;
	for (uint end = 0; end <= inString.GetLength(); end++)
	{
		if (end == inString.GetLength() || inString[end] == inLimiter)
		{
			if (begin < end)
			{
				outParts.Append(inString.Substring(begin, end - begin));
			}
			begin = end + 1;
		}
	}
}

