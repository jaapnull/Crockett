#pragma once
#include <CFile/File.h>

class LineReader
{
public:
	LineReader(Stream& inStream) : mInStream(inStream) { }
	bool ReadLine(String& outString)
	{
		if (!mInStream.IsValid())
			return false;

		outString.Clear();
		char c;
		bool read_something = false;
		while (mInStream.GetBytes(&c, 1) != 0)
		{
			read_something = true;
			if (c == '\r') continue;
			if (c == '\n')
			{
				return true;
			}
			outString.Append(c);
		}
		return read_something;
	}
private:
	String mBuffer;
	Stream& mInStream;
};

