#pragma once

#include <stdio.h>
#include <unordered_map>

#include <CorePCH.h>
#include <CCore/types.h>
#include <CCore/array.h>
#include <CCore/string.h>
#include <CUtils/stringutils.h>
#include <CUtils/enummask.h>
#include <CStreams/Path.h>

class Stream;
class StreamDevice;

enum StreamMode
{
	smRead,
	smWrite
};


class DeviceRegistery
{
public:
	void			RegisterDevice(StreamDevice* inDevice);
	StreamDevice*	FindDevice(const String& inName);

private:
	typedef std::pair<String, StreamDevice*>						DeviceEntry;
	typedef std::unordered_map<String, StreamDevice*>				DeviceMap;
	DeviceMap														mDeviceMap;
};

extern DeviceRegistery gDevices;

class StreamDevice
{
public:
	virtual const String& GetName() const = 0;
	virtual Stream* CreateStream(const Path& inPath, StreamMode inMode) = 0;
	virtual void CloseStream(Stream* inStream) = 0; 
};


class Stream
{
public:
	static const size64		cStreamError = size64(-1);
	static const size64		cStreamEndOfStream = size64(-2);

	virtual bool			IsValid() const = 0;
	virtual const Path&		GetPath() const = 0;
	virtual size64			GetBytes(void* outBytes, size64 inLength) = 0;			// returns amount of bytes read
	virtual size64			PutBytes(const void* inBytes, size64 inLength) = 0;		// returns amount of bytes written
};

inline Stream& operator<<(Stream& ioStream, const String& inString)
{
	ioStream.PutBytes(inString.GetData(), inString.GetLength());
	return ioStream;
}

inline Stream& operator<<(Stream& ioStream, const char& inChar)
{
	ioStream.PutBytes(&inChar, 1);
	return ioStream;
}


