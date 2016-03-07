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
		inInspector.Inspect(mName, "!name");
		inInspector.Inspect(mLocation, "!location");
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

template <class T, class S> T* gGetDebugField(S& inObject, const String& inMemberName)
{
	gAssert(!inMemberName.IsEmpty() && inMemberName[0] == '!');
	TypeDecl debug_type = gInspectDeclaration<T>();
	TypedPointer debug_object = gInspectObject(inObject).GetCompoundMember(inMemberName);
	gAssert(debug_object.mPointer == nullptr ||  debug_type == debug_object.mType);
	return (T*)debug_object.mPointer;
}

/*
template <class T, class S> void gSetDebugField(S& inObject, const String& inMemberName, const T& inField)
{
	gAssert(!inMemberName.IsEmpty() && inMemberName[0] == '!');
	TypeDecl debug_type = gInspectDeclaration<T>();
	TypedPointer debug_object = gInspectObject(inObject).GetCompoundMember(inMemberName);
	gAssert(debug_object.mPointer == nullptr || debug_type == debug_object.mType);
	debug_object.mType.mCompoundInfo->mAssignFunction(&debug_object, &inField);
}
*/

