#pragma once

#include <CCore/Types.h>
#include <CCore/Streams.h>
#include <CUtils/EnumMask.h>

enum EFileOpenMode	// mask
{
	fomRead = 1,			// open file for reading
	fomWrite = 2,			// open file for writing
	fomDiscard = 4			// discard contents if file exists already
};


// thin wrapper around standard wrapper
class File : public Stream
{
	public:
							File() : mFileHandle(0)											{}
							~File()															{ if (mFileHandle) Close(); }

	void					Open(const Path& inPath, EnumMask<EFileOpenMode> inMode);
	void					Close();
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





