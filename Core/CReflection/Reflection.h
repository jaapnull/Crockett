#pragma once
#include <Stdafx.h>
#include <CCore/types.h>
#include <CCore/string.h>

const unsigned int MAXCLASSNAMELENGTH = 64;
class ObjectInspector;
class ClassReflectionInfo;

class ClassName : public String
{
public:
	ClassName() {}
	explicit ClassName(const String& inString) : String(inString) {}
	explicit ClassName(const type_info& inTypeInfo)
	{
		String temp(inTypeInfo.name());
		assert(temp.Substring(0, 6) == "class ");
		Set(temp.Substring(6, temp.GetLength() - 6));
	}
};



template <>
struct std::hash<ClassName>
{
	std::size_t operator()(const ClassName& k) const
	{
		return k.GetHash();
	}
};


enum EType				{ etClass, etInt, etFloat, etShort, etUShort, etChar, etUInt, etUChar, etDouble, etBool, etWChar, etCount, etInvalid = etCount };
enum ETypeModifier		{ ctNone = 0, ctPointer = 1, ctArray = 2, ctString = 3 };

template <typename T>
struct ReflectHelper
{
	static void*		sConstructInstance(void* inPlacement)					{ void* new_object = (inPlacement == 0) ? new T() : new (inPlacement)T(); return new_object; }
	static void			sAssignInstance(void* inDest, const void* inSource)		{ *((T*)inDest) = *((T*)inSource); }
	static void			sCopyInstance(void* inDest, const void* inSource)		{ *((T*)inDest) = *((T*)inSource); }
	static void			sDestructInstance(void* inPlacement, bool inKeepMemory)	{ if (inKeepMemory) ((T*)inPlacement)->~T(); else delete inPlacement; }
};




class DecorationStack
{
public:
	DecorationStack() : mData(0) {}
	ETypeModifier		Back()									const { return ETypeModifier(mData & 0x3); }
	ETypeModifier		operator[](uint inIndex)				const { return ETypeModifier((mData >> (inIndex * 2)) & 0x3); }
	bool				IsOnly(ETypeModifier inContainer)		const { return (mData == inContainer); }
	bool				IsEmpty()								const { return mData == 0; }
	void				Pop() { mData >>= 2; }
	void				Push(ETypeModifier inContainer) { gAssert(inContainer != ctNone); mData <<= 2; mData |= uint32(inContainer); }
	bool				IsArray()								const { return (Back() == ctArray); }
	bool				IsString()								const { return (Back() == ctString); }
	bool				IsPointer()								const { return (Back() == ctPointer); }
	bool				IsContainer()							const { return (Back() == ctArray || Back() == ctString); }
	uint				GetLength()								const { for (uint s = 0; s < 16; s++) if ((mData >> (s * 2)) == 0) return s; return 16; }
	void				Clear() { mData = 0; }
private:
	uint32				mData;
};


struct CompoundReflectionInfo;

struct TypeDecl
{
						TypeDecl() : 
							mBaseType(etInvalid), 
							mClassInfo(nullptr) {}
	size64				GetSizeInBytes() const;
	size64				GetAlignment() const;
	bool				IsPrimitiveType() const { return mModifiers.IsEmpty() && mBaseType != etClass; }
	bool				IsClassType() const { return mModifiers.IsEmpty() && mBaseType == etClass; }
	void				GetInternalType(TypeDecl& outType)	const { gAssert(!mModifiers.IsEmpty()); outType = *this; outType.mModifiers.Pop(); }
	String				ToString() const;

	const CompoundReflectionInfo*	mClassInfo;
	EType				mBaseType;
	DecorationStack		mModifiers;
};

struct ClassMember
{
	TypeDecl			mType;
	String				mName;
	offset64			mOffset;
};


struct CompoundReflectionInfo
{
public:
	typedef void*				(ReflectInstanceFunction)(void*);				// InstanceFunction is used to create a "clean" instance for serialization
	typedef void				(ReflectCopyFunction)(void*, const void*);		// CopyFunction is used to copy one object onto the other
	typedef void				(ReflectAssignFunction)(void*, const void*);	// CopyFunction is used to copy one object onto the other
	typedef void				(ReflectDestructFunction)(void*, bool);			// DestructFunction is used to delete a created object, either releasing or keeping memory intact

	ClassName					mName;
	uint						mSize;
	uint						mAlign;
	ReflectInstanceFunction*	mInstanceFunction;
	ReflectAssignFunction*		mAssignFunction;
	ReflectCopyFunction*		mCopyFunction;
	ReflectDestructFunction*	mDestructorFunction;
	Array<ClassMember>			mMembers;
};






struct TypedPointer
{
					TypedPointer() : mPointer(0) {}
					TypedPointer(const TypedPointer& inOther) :
						mType(inOther.mType),
						mPointer(inOther.mPointer)						{}
					TypedPointer(const TypeDecl& inType, void* inPointer) :
						mType(inType),
						mPointer(inPointer)								{}
	bool			IsValidPointer() const								{ return mPointer != nullptr; }

	TypeDecl		mType;
	void*			mPointer;
};


// Inspect code to generate a TypeInfo from a class <T>
template <typename T> void gInspectDeclaration(TypeDecl& ioInfo)		{ InspectFunc<T> p; p.Fill(ioInfo); }
// Various templates to generate various types of type-specific stuff
template <typename T> struct InspectFunc								{ void Fill(TypeDecl& ioInfo) const { ioInfo.mName = ReflectionHost::sGetReflectionHost().sFindClassInfo<T>(); ioInfo.mBaseType = etClass; } };
template <> struct InspectFunc<int>										{ void Fill(TypeDecl& ioInfo) const { ioInfo.mBaseType = etInt;		} };
template <> struct InspectFunc<float>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mBaseType = etFloat;	} };
template <> struct InspectFunc<bool>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mBaseType = etBool;	} };
template <> struct InspectFunc<wchar_t>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mBaseType = etWChar;	} };
template <> struct InspectFunc<char>									{ void Fill(TypeDecl& ioInfo) const { ioInfo.mBaseType = etChar;	} };
template <> struct InspectFunc<unsigned char>							{ void Fill(TypeDecl& ioInfo) const { ioInfo.mBaseType = etUChar;	} };
template <typename T> struct InspectFunc<Array<T>>						{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctArray);	} };
template <typename T> struct InspectFunc<BaseString<T>>					{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctString);	} };

// specialized container overloads; kinda sucks but couldn't get to pick up overloaded vers.
template <> struct InspectFunc<String>									{ void Fill(TypeDecl& ioInfo) const { InspectFunc<char> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctString); } };
template <> struct InspectFunc<WString>									{ void Fill(TypeDecl& ioInfo) const { InspectFunc<wchar_t> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctString); } };

template <typename T> struct InspectFunc <T*>							{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctPointer);	} };


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
		// fill description through static analysis
		CompoundReflectionInfo description;
		ObjectInspector reflect_inspector(description);
		reflect_inspector.StaticAnalysis<T>();

		// add to hash-map, hashed on typename
		ClassInfoEntry entry;
		entry.first = description.mName;
		entry.second = description;
		mItems.insert(entry);
	}

	// find by name
	const CompoundReflectionInfo*			FindClassInfo(const ClassName& inClassName) const;

	// find by static type
	template <typename T>
	const CompoundReflectionInfo*			FindClassInfo() const
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

