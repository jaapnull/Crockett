#pragma once
#include <stdafx.h>
#include "String.h"

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
	
	return inString.Equals("true") || atoi(inString.GetData()) != 0;
}


