#pragma once
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CCore/Streams.h>

class Resource
{
public:
	String mName;
	String mLocation;
	virtual void Boo() {}

	void Inspect(ObjectInspector& inInspector)
	{
	}

};

typedef Array<Resource> ResourceSet;

// Dependency between serialized files or between files in the same source file
struct Dependency
{
	TypedPointer			mObject;						// Pointer of the object that has the dependency
	String					mTargetPath;					// Path of the target
	String					mReflectionPath;				// Path in the reflection tree of mObject
};

