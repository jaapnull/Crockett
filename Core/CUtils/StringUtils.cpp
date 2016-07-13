#include "CorePCH.h"
#include <CUtils/StringUtils.h>

bool		gIsNumeric(const char& inChar)			{ return inChar >= '0' && inChar <= '9'; }
bool		gIsWhiteSpace(const char& inChar)		{ return inChar == '\t' || inChar == ' ' || inChar == '\r' || inChar == '\n'; }
char		gToUpper(const char& inChar)			{ return inChar >= 'a' && inChar <= 'z' ? inChar - 'a' + 'A' : inChar; }
void		gToUpper(String& ioString)				{ for (char& c : ioString) c = gToUpper(c); }
char		gToLower(const char& inChar)			{ return inChar >= 'A' && inChar <= 'Z' ? inChar - 'A' + 'a' : inChar; }
void		gToLower(String& ioString)				{ for (char& c : ioString) c = gToLower(c); }

float gStringToFloat(const String& inString)
{
	return float(atof(inString.GetData()));
}

int gStringToInt(const String& inString)
{
	return atoi(inString.GetData());
}

uint gStringToUInt(const String& inString)
{
	return atoi(inString.GetData());
}

bool gStringToBool(const String& inString)
{
	return inString.Equals("true");
}


String gToString(short inShort)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%d", inShort);
	return String(buffer);
}

String gToString(ushort inShort)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%d", inShort);
	return String(buffer);
}

String gToString(int inUint)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%d", inUint);
	return String(buffer);
}

String gToString(uint inUint)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%u", inUint);
	return String(buffer);
}

String gToString(double inDouble)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%f", inDouble);
	return String(buffer);
}

String gToString(float inFloat)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%f", inFloat);
	return String(buffer);
}

offset64 gFindInString(const String& inCorpus, const String& inSearchTerm, offset64 inStartOffset)
{
	if (inCorpus.IsEmpty() || inSearchTerm.IsEmpty() || inCorpus.GetLength() < inSearchTerm.GetLength())
		return cMaxSize64;
	offset64 start_pos_max = inCorpus.GetLength() - inSearchTerm.GetLength();
	offset64 matched_length = 0;
	for (offset64 c = inStartOffset; c <= start_pos_max; c++)
	{
		char current_char = inCorpus[c];
		matched_length =	(inSearchTerm[matched_length] == current_char) ? matched_length + 1 :
							(inSearchTerm[0] == current_char) ? 1 : 0;

		if (matched_length == inSearchTerm.GetLength())
			return c-matched_length+1;
	}
	return cMaxSize64;
}
