#pragma once
#include <CorePCH.h>
#include <CCore/types.h>
#include <CCore/string.h>
#include <CReflection/TypedPointer.h>
#include <unordered_map>


template <typename T>
struct ReflectHelper
{
	static void*					sConstructInstance(void* inPlacement)					{ void* new_object = (inPlacement == 0) ? new T() : new (inPlacement)T(); return new_object; }
	static void						sAssignInstance(void* inDest, const void* inSource)		{ *((T*)inDest) = *((T*)inSource); }
	static void						sCopyInstance(void* inDest, const void* inSource)		{ *((T*)inDest) = *((T*)inSource); }
	static void						sDestructInstance(void* inPlacement, bool inKeepMemory)	{ if (inKeepMemory) ((T*)inPlacement)->~T(); else delete inPlacement; }
};



// Inspect code to generate a TypeInfo from a class <T>
template <typename T> const TypedPointer gInspectObject(T& inObject)	{ TypedPointer tp; gInspectDeclaration<T>(tp.mType); tp.mPointer = (void*)&inObject; return tp; }

// Inspect code to generate a TypeInfo from a class <T>
template <typename T> void gInspectDeclaration(TypeDecl& ioInfo)		{ InspectFunc<T> p; p.Fill(ioInfo); }
// Various templates to generate various types of type-specific stuff
template <typename T> struct InspectFunc								{ void Fill(TypeDecl& ioInfo) const { ioInfo.mCompoundInfo = ReflectionHost::sGetReflectionHost().FindCompoundInfo<T>(); ioInfo.mNakedType = etCompound; } };
template <> struct InspectFunc<int>										{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etInt;		} };
template <> struct InspectFunc<float>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etFloat;	} };
template <> struct InspectFunc<bool>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etBool;	} };
template <> struct InspectFunc<wchar_t>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etWChar;	} };
template <> struct InspectFunc<char>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etChar;	} };
template <> struct InspectFunc<unsigned char>							{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etUChar;	} };
template <typename T> struct InspectFunc<Array<T>>						{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctArrayOf);	} };
template <typename T> struct InspectFunc<BaseString<T>>					{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf);	} };

// specialized container overloads; kinda sucks but couldn't get to pick up overloaded vers.
template <> struct InspectFunc<String>									{ void Fill(TypeDecl& ioInfo) const { InspectFunc<char> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf); } };
template <> struct InspectFunc<WString>									{ void Fill(TypeDecl& ioInfo) const { InspectFunc<wchar_t> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf); } };

template <typename T> struct InspectFunc <T*>							{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctPointerTo);	} };




class ObjectInspector
{
public:

	ObjectInspector(CompoundReflectionInfo& inDescription) : mTargetDescription(inDescription) {}
	
	// static analysis entry point
	template <typename T> void StaticAnalysis(void)
	{
		mTargetDescription.mName = sGetTypeName<T>();
		mTargetDescription.mSize = sizeof(T);
		mTargetDescription.mAlign= __alignof(T);
		mTargetDescription.mInstanceFunction = &(ReflectHelper<T>::sConstructInstance);
		mTargetDescription.mCopyFunction = &(ReflectHelper<T>::sCopyInstance);
		mTargetDescription.mDestructorFunction = &(ReflectHelper<T>::sDestructInstance);
		mTargetDescription.mAssignFunction= &(ReflectHelper<T>::sAssignInstance);
		((T*)0)->T::Inspect(*this);
	}

	// Visit callback during an inspection of an object
	template <typename T>
	void Inspect(T& inMemberObject, const String& inIdentifier)
	{
		ClassMember member_info;
		gInspectDeclaration<T>(member_info.mType);
		member_info.mOffset = (offset64)&inMemberObject;	// since inMemberObject is a member of a null-object, each offset is relative to zero
		member_info.mName = inIdentifier;
		mTargetDescription.mMembers.Append(member_info);
	}
private:
	template <typename T>
	static ClassName			sGetTypeName()		{ return ClassName(typeid(T)); }
	CompoundReflectionInfo&		mTargetDescription;
};


// The register that keeps track of all Serializable classes and their inspected bits and pieces. Basically a reflection symbol table
class ReflectionHost
{
public:

	static ReflectionHost& sGetReflectionHost()
	{
		if (sReflectionHost == nullptr)
		{
			sReflectionHost = new ReflectionHost();
		}
		return *sReflectionHost;
	}
	
	ReflectionHost() {}
	template<typename T>
	void RegisterClassType()						// Registers a class, giving instance func and id
	{

		CompoundReflectionInfo description;

		// add to hash-map, hashed on typename
		ClassInfoEntry entry;
		entry.first = sGetTypeName<T>();
		mItems.insert(entry);

		// fill description through static analysis
		ClassInfoMap::iterator it = mItems.find(entry.first);
		ObjectInspector reflect_inspector(it->second);
		reflect_inspector.StaticAnalysis<T>();
	}

	// find by name
	const CompoundReflectionInfo*			FindClassInfo(const ClassName& inClassName) const;

	// find by static type
	template <typename T>
	const CompoundReflectionInfo*			FindCompoundInfo() const
	{
		ClassName type_name = sGetTypeName<T>();
		return FindClassInfo(type_name);
	}

private:
	typedef std::pair<ClassName, CompoundReflectionInfo>			ClassInfoEntry;
	typedef std::unordered_map<ClassName, CompoundReflectionInfo>	ClassInfoMap;			// 
	ClassInfoMap													mItems;					// a map from id to instance function		
	static ReflectionHost*											sReflectionHost;

	template <typename T>
	static ClassName												sGetTypeName() { return ClassName(typeid(T)); }
};



