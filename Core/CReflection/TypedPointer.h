#pragma once
#include <CCore/Assert.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CReflection/TypeDecl.h>

class TypedCompoundPointer;
class TypedArrayPointer;
class TypedPointerPointer;

class TypedPointer
{
public:
									TypedPointer() : 
										mPointer(0)									{}
									TypedPointer(const TypedPointer& inOther) :
										mType(inOther.mType),
										mPointer(inOther.mPointer)					{}
									TypedPointer(const TypeDecl& inType, void* inPointer) :
										mType(inType),
										mPointer(inPointer)							{}

	bool							IsValid() const											{ return mType.IsValid() && mPointer != nullptr; }
	TypedPointer					GetObjectAtPath(const String& inPath);					// Evaluate a path and return object that is pointed to

	TypeDecl						mType;
	void*							mPointer;
};

class TypedCompoundPointer : public TypedPointer
{
public:

	TypedCompoundPointer(const TypeDecl& inType, void* inPointer) : TypedPointer(inType, inPointer) {}

	TypedCompoundPointer(const TypedPointer& tp) : TypedPointer(tp)
	{
		gAssert(mType.IsNakedCompound());
	}
	TypedPointer					GetCompoundMember(const String& inMemberName) const;	// Get element from container type


	template<class T>
	T* GetCompoundMember(const String& inMemberName)
	{
		for (const ClassMember& m : mType.mCompoundInfo->mMembers)
		{
			if (m.mName == inMemberName)
			{
				TypedPointer tp(m.mType, gOffsetPointer<void>(mPointer, m.mOffset));
				//const CompoundReflectionInfo* expected_type = ReflectionHost::sGetReflectionHost().FindCompoundInfoStatic<T>();
				TypeDecl dcl = gInspectDeclaration<T>();
				gAssert(dcl == tp.mType);
				return (T*)tp.mPointer;
			}
		}
		return nullptr;
	}



};

class TypedArrayPointer : public TypedPointer
{
public:
	TypedArrayPointer(const TypedPointer& tp) : TypedPointer(tp)
	{
		gAssert(mType.GetOuterDecoration() == ctArrayOf);
	}
	const size64					GetContainerElementCount() const;						// Get element count in case of container type
	TypedPointer					GetContainerElement(size64 inIndex) const;				// Get element from container type
	TypedPointer					CreateNewArrayItem();
};

class TypedPointerPointer : public TypedPointer
{
public:
	TypedPointerPointer(const TypedPointer& tp) : TypedPointer(tp)
	{
		gAssert(mType.GetOuterDecoration() == ctPointerTo);
	}
	const TypedPointer				DerefPointer() const;									// Deref pointer
};

template <typename T>
TypedPointer gInspectObject(T& inObject)
{
	TypeDecl td = gLookupTypeForObject<T>(inObject);
	void* ptr = &inObject;
	return TypedPointer(td, ptr);
}



