#pragma once
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CStreams/Streams.h>


struct UnresolvedLink
{
	TypedPointer				mLinkParentObject;		///< Object that contains the pointer
	Array<ReflectPathPart>		mReflectionPath;		///< Reflection path to the link (pointer) within the parent
	Path						mTargetLocation;		///< Location of the target object
	String						mTargetName;			///< Target Name
};

// Dependency between serialized files or between files in the same source file
struct Dependency
{
	TypedPointer			mObject;						// Pointer of the object that has the dependency
	String					mResourcePath;					// Path of the target in resource
	String					mReflectionPath;				// Path in the reflection tree of mObject
};

template <class T, class S> T* gGetDebugField(S& inObject, const String& inMemberName)
{
	gAssert(!inMemberName.IsEmpty() && inMemberName[0] == '!');
	TypeDecl debug_type = gInspectDeclaration<T>();
	TypedPointer debug_object = TypedCompoundPointer(gInspectObject(inObject)).GetCompoundMember(inMemberName);
	gAssert(debug_object.mPointer == nullptr ||  debug_type == debug_object.mType);
	return (T*)debug_object.mPointer;
}

