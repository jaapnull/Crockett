#include <CorePCH.h>
#include <CFile/File.h>


String gPathToSystem(const String& inLocationNoDevice)
{
	String s = inLocationNoDevice;
	gStripWhitespace(s);
	for (char& c : s)	if (c == '\\') c = '/';
	return s;
}

void File::Open(const String& inFileName, EnumMask<EFileOpenMode> inMode)
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
	int err = fopen_s(&mFileHandle, inFileName.GetCString(), open_mode);
	assert(mFileHandle);
	mPath = String("system:") + inFileName; // this is now officially a stream with a valid path (to the file opened)
}


Stream* FileDevice::CreateStream(const Path& inPath, StreamMode inMode)
{
	// stream should not have a device name (yet)
	gAssert(inPath.GetDeviceName().IsEmpty() || inPath.GetDeviceName() == mDeviceName);

	// stream should be a file location
	gAssert(inPath.IsFileLocation());

	// stream should not have an object name
	gAssert(inPath.GetObjectName().IsEmpty());

	// stream should have a location
	gAssert(!inPath.GetLocation().IsEmpty());

	// generate full stream path
	Path stream_path = inPath;
	stream_path.SetDeviceName(mDeviceName);

	String system_path = mSystemRoot + String("\\") + gPathToSystem(inPath.GetFile());

	FileDeviceStream* f = new FileDeviceStream(stream_path);
	f->Open(system_path, inMode == smRead ? fomRead : fomWriteDiscard);
	return f;
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





