#include <stdafx.h>
#include <CFile/File.h>

void File::Open(const Path& inPath, EnumMask<EFileOpenMode> inMode)
{
	assert(inMode.Contains(fomRead) || inMode.Contains(fomWrite));		// read or write at least
	assert(!inMode.Contains(fomDiscard) || inMode.Contains(fomWrite));	// discard only makes sense if we have write access		
	// its easier to list all valid modes than to actually setup crazy logic for it:
	char* open_mode = (inMode == fomRead) ? "rb" :
		(inMode == fomWrite) ? "ab" :
		(inMode == (fomRead | fomWrite)) ? "ab+" :
		(inMode == (fomWrite | fomDiscard)) ? "wb" :
		(inMode == (fomRead | fomWrite | fomDiscard)) ? "wb+" : 0;
	assert(open_mode);
	int err = fopen_s(&mFileHandle, inPath.GetCString(), open_mode);
	assert(mFileHandle);
	mPath = inPath; // this is now officially a stream with a valid path (to the file opened)
}

void File::Close()
{
	assert(mFileHandle != 0);
	fclose(mFileHandle);
	mFileHandle = 0;
	mPath.Clear();
}

size64 File::GetLength() const
{
	fseek(mFileHandle, 0, SEEK_END);
	size64 file_size = ftell(mFileHandle);
	fseek(mFileHandle, 0, SEEK_SET);
	return file_size;
}

const Path& File::GetPath() const
{
	return mPath;
}

size64 File::GetPointer()
{
	return ftell(mFileHandle);
}
size64 File::SetPointer(size64 inReadPointer)
{
	return fseek(mFileHandle, (long)inReadPointer, SEEK_SET);
}


// read 
size64 File::GetBytes(void* outBytes, size64 inLength)
{
	size64 read_bytes = size64(fread(outBytes, 1, inLength, mFileHandle));
	if (read_bytes != inLength)
	{
		if (ferror(mFileHandle)) return Stream::cStreamError;
		assert(feof(mFileHandle)); // can't find good reason why the read bytes is not inLength
	}
	return read_bytes;
}


size64 File::PutBytes(const void* inBytes, size64 inLength)
{
	return size64(fwrite(inBytes, 1, inLength, mFileHandle));
}


