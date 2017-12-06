#pragma once

#include <CCore/Types.h>
#include <CStreams/Streams.h>
#include <CStreams/Path.h>
#include <CUtils/EnumMask.h>

enum EFileOpenMode			// mask
{
	fomRead = 1,			// open file for reading
	fomWrite = 2,			// open file for writing
	fomDiscard = 4,			// discard contents if file exists already (not valid stand-alone option)
	fomWriteDiscard = 6		// discard contents when writing
};


// thin wrapper around standard wrapper
class File : public Stream
{
	public:
							File() : mFileHandle(0)											{}
							~File()															{ if (mFileHandle) Close(); }

	void					Open(const String& inFileName, EnumMask<EFileOpenMode> inMode);
	void					Close();

	virtual bool			IsValid() const													{ return (mFileHandle != nullptr); }

	virtual size64			GetLength() const;
	virtual const Path&		GetPath() const;

	// read
	virtual size64			GetBytes(void* outBytes, size64 inLength);
	// write
	virtual size64			PutBytes(const void* inBytes, size64 inLength);

	virtual size64			GetPointer();
	virtual size64			SetPointer(size64 inReadPointer);

	private:
	FILE*					mFileHandle;
	Path					mPath;
};


class FileDeviceStream : public File
{
public:
	FileDeviceStream(const Path& inDevicePath) : 
		mDevicePath(inDevicePath) {}
	virtual const Path&		GetPath() const { return mDevicePath; }
	Path					mDevicePath;
};


class FileDevice : public StreamDevice
{

public:
	FileDevice(const String& inName, const String& inSystemRoot) : mDeviceName(inName), mSystemRoot(inSystemRoot) { }

	virtual const String&		GetName() const										{ return mDeviceName; }
	virtual Stream*				CreateStream(const Path& inPath, StreamMode inMode);
	virtual void				CloseStream(Stream* inStream)						{ ((File*)inStream)->Close(); delete inStream; }

private:
	String	mSystemRoot;
	String	mDeviceName;
};

