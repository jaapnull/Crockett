#pragma once
#include "../CCore/String.h"
#include "../CCore/Array.h"

//utility functions (char-string only)

extern bool		gIsNumeric(const char& inChar);
extern bool		gIsWhiteSpace(const char& inChar);
extern char		gToUpper(const char& inChar);
extern void		gToUpper(String& inString);
extern char		gToLower(const char& inChar);
extern void		gToLower(String& inString);
extern float	gStringToFloat(const String& inString);
extern int		gStringToInt(const String& inString);
extern uint		gStringToUInt(const String& inString);
extern String	gToString(uint inUint);
extern String	gToString(double inDouble);
extern String	gToString(float inFloat);
extern String	gToString(int inFloat);
extern String	gToString(short inFloat);
extern String	gToString(ushort inFloat);
extern offset64 gFindInString(const String& inCorpus, const String& inSearchTerm, offset64 inStartOffset = 0);


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

