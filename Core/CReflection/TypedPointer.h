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

	const TypedPointer				DerefPointer() const;							// Deref pointer
	const size64					GetContainerElementCount() const;				// Get element count in case of container type
	TypedPointer					GetContainerElement(size64 inIndex) const;		// Get element from container type
	TypedPointer					GetCompoundMember(const String& inMemberName) const;	// Get element from container type
	TypedPointer					GetObjectAtPath(const String& inPath);			// Evaluate a path and return object that is pointed to

	TypeDecl						mType;
	void*							mPointer;
};

// Inspect code to generate a TypeInfo from a class <T>
// template <typename T> const TypedPointer gInspectObject(T& inObject)
// { 
// 	TypedPointer tp; 
// 	gInspectDeclaration<T>(tp.mType); 
// 	tp.mPointer = (void*)&inObject; 
// 	return tp; 
// }


//template <typename T>
//TypedPointer gInspectObject(const T& inObject)
//{
//	const CompoundReflectionInfo* compound_info = ReflectionHost::sGetReflectionHost().FindCompoundInfoDynamic<T>(inObject); 
//	TypeDecl ttype(compound_info);
//	TypedPointer tp = TypedPointer(ttype, (void*)&inObject);
//	return tp;
//}
//
