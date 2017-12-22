#pragma once
#include <CFile/File.h>

class LineReader
{
public:
	LineReader(Stream& inStream) : mInStream(inStream) { }


	void ReadAllLines(Array<String>& outLines)
	{
		String line;
		while (ReadLine(line))
		{
			outLines.Append(line);
		}
	}

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
				mLinesRead++;
				return true;
			}
			outString.Append(c);
		}
		if (read_something) mLinesRead++;
		return read_something;
	}

	uint GetLinesRead() const { return mLinesRead; }

private:
	uint32		mLinesRead = 0;
	String		mBuffer;
	Stream&		mInStream;
};

