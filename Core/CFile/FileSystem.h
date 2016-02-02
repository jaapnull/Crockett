#pragma once

#include "streams.h"

enum MFileOpenMode	// mask
{
	fomRead = 1,			// open file for reading
	fomWrite = 2,			// open file for writing
	fomDiscard = 4			// discard contents if file exists already
};


class FileSystem
{
	// empty for now
};

// thin wrapper around standard wrapper
class File : public Stream
{
	public:
						File() : mFileHandle(0)											{}
						~File()															{ if (mFileHandle) Close(); }

	void				Open(const Path& inPath, EnumMask<MFileOpenMode> inMode);
	void				Close();
	virtual size64		GetLength() const;
	virtual const Path&	GetPath() const;

	// read
	virtual size64		GetBytes(void* outBytes, size64 inLength);
	// write
	virtual size64		PutBytes(const void* inBytes, size64 inLength);

	virtual size64		GetPointer();
	virtual size64		SetPointer(size64 inReadPointer);

	private:
	FILE*				mFileHandle;
	Path				mPath;
};





