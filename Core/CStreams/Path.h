#pragma once
#include <CUtils/StringUtils.h>
#include <String.h>

/// Path has form
/// DEVICENAME@[DIR\]* [FILENAME.]+  EXTENSION:OBJNAME

class Path : public String
{
public:
	Path() {}
	Path(const char* inPath) : String(inPath) {}
	Path(const String& inPath) : String(inPath) {}

	String GetDir() const		//		Dir = [DIR\]*
	{
		String s = GetLocation();
		Array<String> l;
		gExplodeString(l, s, '@');
		gAssert(l.GetLength() <= 2);
		return l.Back();
	}

	String GetFile() const		//		File = [FILENAME.]+  EXTENSION
	{
		size64 begin = Find('@');
		if (begin == cMaxSize64) begin = 0; else begin++;
		size64 end = FindR(':');
		if (end == cMaxSize64) end = GetLength();
		return Substring(begin, end - begin);
	}


	String GetDeviceName() const
	{
		Array<String> device_split;
		gExplodeString<char>(device_split, *this, '@');
		return (device_split.GetLength() == 2) ? device_split[0] : String();
	}
	
	String GetLocation() const
	{
		Array<String> location_split;
		gExplodeString<char>(location_split, *this, ':');
		gAssert(location_split.GetLength() <= 2);
		return location_split[0];
	}
	
	bool IsFileLocation() const
	{
		return Find('.') != cMaxSize64;
	}

	String GetObjectName() const
	{
		Array<String> device_split;
		gExplodeString<char>(device_split, *this, '@');
		Array<String> location_split;
		gExplodeString<char>(location_split, device_split.Back(), ':');
		if (location_split.GetLength() == 1) return String();
		gAssert(location_split.GetLength() <= 2);
		return location_split.GetLength() == 1 ? "" : location_split[1];
	}

	void SetDeviceName(const String& inDeviceName)
	{
		Array<String> device_split;
		gExplodeString<char>(device_split, *this, '@');
		gAssert(device_split.GetLength() <= 2);
		Set(inDeviceName);
		if (!inDeviceName.IsEmpty()) Append("@");
		Append(device_split.Back());
	}
};
