#pragma once

#include <stdafx.h>
#include <stdio.h>

#include <CCore/types.h>
#include <CCore/array.h>
#include <CCore/string.h>
#include <CUtils/enummask.h>

typedef String Path;

class Stream
{
public:
	static const size64		cStreamError = size64(-1);
	static const size64		cStreamEndOfStream = size64(-2);

	virtual const Path&		GetPath() const = 0;
	virtual size64			GetBytes(void* outBytes, size64 inLength) = 0;			// returns amount of bytes read
	virtual size64			PutBytes(const void* inBytes, size64 inLength) = 0;		// returns amount of bytes written
};

inline Stream& operator<<(Stream& ioStream, const String& inString)
{
	ioStream.PutBytes(inString.GetData(), inString.GetLength());
	return ioStream;
}




