#include <CCore/Assert.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CReflection/TypeDecl.h>



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

	bool							IsValid() const									{ return mType.IsValid() && mPointer != nullptr; }
	const TypedPointer				DerefPointer() const;									// Deref pointer
	const size64					GetContainerElementCount() const;						// Get element count in case of container type
	TypedPointer					GetContainerElement(size64 inIndex) const;				// Get element from container type
	TypedPointer					GetCompoundMember(const String& inMemberName) const;	// Get element from container type
	TypedPointer					GetObjectAtPath(const String& inPath);					// Evaluate a path and return object that is pointed to

	TypeDecl						mType;
	void*							mPointer;
};

template <typename T>
TypedPointer gInspectObject(T& inObject)
{
	TypeDecl td = gLookupTypeForObject<T>(inObject);
	void* ptr = &inObject;
	return TypedPointer(td, ptr);
}



