#pragma once
#include <CorePCH.h>
#include <CCore/types.h>
#include <CCore/string.h>

#include <CReflection/TypedPointer.h>
#include <CReflection/TypeDecl.h>

#include <unordered_map>


template <typename T>
struct ReflectHelper
{
	static const type_info&			sGetDynamicInfo(void* inObject)								{ return typeid(*((T*) inObject)); }
	static void*					sConstructInstance(void* inPlacement)						{ void* new_object = (inPlacement == 0) ? new T() : new (inPlacement)T(); return new_object; }
	static void						sAssignInstance(void* inDest, const void* inSource)			{ *((T*)inDest) = *((T*)inSource); }
	static void						sCopyInstance(void* inDest, const void* inSource)			{ *((T*)inDest) = *((T*)inSource); }
	static void						sDestructInstance(void* inPlacement, bool inKeepMemory)		{ if (inKeepMemory) ((T*)inPlacement)->~T(); else delete inPlacement; }
};


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
		mTargetDescription.mInfoFunction			= &(ReflectHelper<T>::sGetDynamicInfo);
		mTargetDescription.mInstanceFunction		= &(ReflectHelper<T>::sConstructInstance);
		mTargetDescription.mCopyFunction			= &(ReflectHelper<T>::sCopyInstance);
		mTargetDescription.mDestructorFunction		= &(ReflectHelper<T>::sDestructInstance);
		mTargetDescription.mAssignFunction			= &(ReflectHelper<T>::sAssignInstance);
		((T*)0)->T::Inspect(*this);
	}

	// Visit callback during an inspection of an object
	template <typename T>
	void Inspect(T& inMemberObject, const String& inIdentifier)
	{
		ClassMember member_info;
		member_info.mType = gInspectDeclaration<T>();
		member_info.mOffset = (size64)&inMemberObject;	// since inMemberObject is a member of a null-object, each offset is relative to zero
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

		ClassName class_name = sGetTypeName<T>();
		ClassInfoMap::iterator it = mItems.find(class_name);
		
		if (it != mItems.end())
		{
			gAssert(it->second.IsDeclaredOnly());
		}

		// add to hash-map, hashed on typename
		ClassInfoEntry entry;
		entry.first = class_name;
		mItems.insert(entry);

		// fill description through static analysis
		it = mItems.find(entry.first);
		ObjectInspector reflect_inspector(it->second);
		reflect_inspector.StaticAnalysis<T>();
	}

	// find by name
	const CompoundReflectionInfo*			FindClassInfo(const ClassName& inClassName) const;
	const CompoundReflectionInfo*			FindOrCreateClassInfo(const ClassName& inClassName);
	
	// find by static type
	template <typename T>
	const CompoundReflectionInfo*			FindCompoundInfoStatic() const
	{
		ClassName type_name = sGetTypeName<T>();
		return FindClassInfo(type_name);
	}

	template <typename T>
	const CompoundReflectionInfo*			FindOrCreateCompoundInfoStatic()
	{
		ClassName type_name = sGetTypeName<T>();
		return FindOrCreateClassInfo(type_name);
	}


	// find by static type
	template <typename T>
	const CompoundReflectionInfo*			FindCompoundInfoDynamic(const T& inT) const
	{
		ClassName class_name(typeid(inT));
		return FindClassInfo(class_name);
	}

private:
	typedef std::pair<ClassName, CompoundReflectionInfo>			ClassInfoEntry;
	typedef std::unordered_map<ClassName, CompoundReflectionInfo>	ClassInfoMap;			// 
	ClassInfoMap													mItems;					// a map from id to instance function
	static ReflectionHost*											sReflectionHost;

	template <typename T>
	static ClassName												sGetTypeName() { return ClassName(typeid(T)); }
};




