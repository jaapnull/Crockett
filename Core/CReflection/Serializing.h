#pragma once
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CReflection/Reflection.h>
#include <CCore/Streams.h>



struct StreamSet
{
	Array<TypedPointer> mObjects;
};


// Dependency between serialized files or between files in the same source file
// 
struct Dependency
{
	TypedPointer			mObject;						// Pointer of the object that has the dependency
	String					mTargetPath;					// Path of the target
	String					mReflectionPath;				// Path in the reflection tree of mObject
};


// Helper class that writes out reflected objects to (file) stream
class ObjectStreamer
{
public:
	ObjectStreamer(Stream& outStream) : mOutStream(outStream) {}
	const Path&			GetTargetLocation() { return mOutStream.GetPath(); }
	bool				WriteInstance(const TypedPointer& inTypedPointer);
	bool				WriteNamedInstance(const TypedPointer& inTypedPointer, const String& inIdentifier);

protected:
	Array<Dependency>	mDependencies;
	Stream&				mOutStream;
};
