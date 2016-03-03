#pragma once
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
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
	ObjectStreamer(Stream& outStream) : mOutStream(outStream), mIndent(0)	{}
	const Path&			GetTargetLocation()									{ return mOutStream.GetPath(); }
	String				IndentStart()										{ mIndent++; return String((mIndent - 1) * 2, ' '); }
	String				IndentStop()										{ mIndent--; return String((mIndent) * 2, ' '); }
	String				Indent()											{ return String((mIndent)* 2, ' '); }

	bool				WriteInstance(const TypedPointer& inTypedPointer);


protected:
	Array<Dependency>	mDependencies;
	Stream&				mOutStream;
	int					mIndent;

};
