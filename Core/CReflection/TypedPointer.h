#pragma once
#include <CCore/Assert.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CReflection/TypeDecl.h>

class TypedCompoundPointer;
class TypedArrayPointer;
class TypedPointerPointer;


struct ReflectPathPart
{
	ReflectPathPart() {}
	ReflectPathPart(uint32 inOffset) : mOffset(inOffset) {}
	uint32	mOffset = 0;			///< Offset of the next part; either an element index for arrays of member index for compounds
};


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

	void							Clear()											{ mPointer = nullptr; mType.Clear(); }
	bool							IsValid() const									{ return mType.IsValid() && mPointer != nullptr; }
	TypedPointer					GetObjectAtStringPath(const String& inPath);	// Evaluate a path and return object that is pointed to
	TypedPointer					GetObjectAtPath(const Array<ReflectPathPart>& inPath);
	String							ResolvePathToString(const Array<ReflectPathPart>& inPath) const;

	TypeDecl						mType;
	void*							mPointer;
};

class TypedCompoundPointer : public TypedPointer
{
public:

									TypedCompoundPointer() {}
									TypedCompoundPointer(const TypeDecl& inType, void* inPointer) : TypedPointer(inType, inPointer) { }
									TypedCompoundPointer(const TypedPointer& tp) : TypedPointer(tp)									{ gAssert(mType.IsNakedCompound()); }


	TypedPointer					DynamicCast() const;
	TypedPointer					GetCompoundMemberByIndex(uint32 inIndex) const;
	uint32							GetCompoundMemberIndex(const String& inMemberName) const;
	TypedPointer					GetCompoundMember(const String& inMemberName) const;

	template<class T>
	T* GetCompoundMember(const String& inMemberName)
	{
		for (const ClassMember& m : mType.mCompoundInfo->mMembers)
		{
			if (m.mName == inMemberName)
			{
				TypedPointer tp(m.mType, gOffsetPointer<void>(mPointer, m.mOffset));
				TypeDecl dcl = gInspectDeclaration<T>();
				gAssert(dcl == tp.mType);
				return (T*)tp.mPointer;
			}
		}
		return nullptr;
	}

	template<class T>
	const T* GetCompoundMember(const String& inMemberName) const
	{
		for (const ClassMember& m : mType.mCompoundInfo->mMembers)
		{
			if (m.mName == inMemberName)
			{
				TypedPointer tp(m.mType, gOffsetPointer<void>(mPointer, m.mOffset));
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

	TypedArrayPointer() {}
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
	TypedPointerPointer() {}
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
	void* ptr = (void*)(&inObject);
	return TypedPointer(td, ptr);
}

template <typename T>
TypedPointer gInspectObject(const T& inObject)
{
	TypeDecl td = gLookupTypeForObject<const T>(inObject);
	void* ptr = (void*) const_cast<T*>(&inObject);
	return TypedPointer(td, ptr);
}


