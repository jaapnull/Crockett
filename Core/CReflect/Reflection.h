#pragma once
#include <Stdafx.h>
#include <CCore/types.h>
#include <CCore/string.h>
// the only macro used
#define registered_class(c) class c; AutoReflection<c> reg__##c; class c
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

inline size_t hash_value(const ClassName& inClassName)
{
	return inClassName.GetHash();
}

enum EType						{ etClass, etInt, etFloat, etShort, etUShort, etChar, etUInt, etUChar, etDouble, etBool, etWChar, etCount, etInvalid = etCount };
enum ETypeModifier				{ ctNone = 0, ctPointer = 1, ctArray = 2, ctString = 3 };

template <typename T>
static ClassName				gGetNameOfType()							{ return ClassName(typeid(T)); }

class IReflected
{
public:
	typedef void*				(InstanceFunction)(void*);						// InstanceFunction is used to create a "clean" instance for serialization
	typedef void				(CopyFunction)(void*, const void*);				// CopyFunction is used to copy one object onto the other
	typedef void				(AssignFunction)(void*, const void*);			// AssignFunction is used to assign A to B
	typedef void				(DestructFunction)(void*, bool);				// DestructFunction is used to delete a created object, either releasing or keeping memory intact

	template <typename T>
	static void* sConstructInstance(void* inPlacement)
	{ 
		void* new_object = (inPlacement == 0) ? new T() : new (inPlacement)T();
		return new_object;
	}

	template <typename T>
	static void* sAssignInstance(void* inDest, const void* inSource)
	{
		*((T*)inDest) = *((T*inSource));
		return new_object;
	}

	template <typename T>
	static void sCopyInstance(void* inDest, const void* inSource)
	{
		*((T*)inDest) = *((T*)inSource);
	}

	template <typename T>
	static void sDestructInstance(void* inPlacement, bool inKeepMemory)
	{
		if (inKeepMemory)
		{
			((T*)inPlacement)->~T();
		}
		else
		{
			delete inPlacement;
		}
	}
};


class TypeModifierStack
{
public:
					TypeModifierStack() : mData(0)				{}
	ETypeModifier	Back()								const	{ return ETypeModifier(mData & 0x3); }
	ETypeModifier	operator[](uint inIndex)			const	{ return ETypeModifier((mData >> (inIndex*2)) & 0x3); }
	bool			IsOnly(ETypeModifier inContainer)	const	{ return (mData == inContainer); }
	bool			IsEmpty()							const	{ return mData == 0; }
	void			Pop()										{ mData >>= 2; }
	void			Push(ETypeModifier inContainer)
	{
		gAssert(inContainer != ctNone);
		mData <<= 2; mData |= uint32(inContainer);
	}
	bool			IsArray()							const	{ return (Back() == ctArray); }
	bool			IsString()							const	{ return (Back() == ctString); }
	bool			IsPointer()							const	{ return (Back() == ctPointer); }
	bool			IsContainer()						const	{ return (Back() == ctArray || Back() == ctString); }
	uint			GetLength()							const	{ for (uint s = 0; s < 16; s++) if ((mData >> (s * 2)) == 0) return s; return 16; }
	void			Clear()										{ mData = 0; }

private:
	uint32			mData;
};

class TypeDecl
{
public:
								TypeDecl() : mBaseType(etInvalid)					{}
	size64						GetSize() const;
	size64						GetAlignment() const;
	bool						IsPrimitiveType() const								{ return mModifiers.IsEmpty() && mBaseType != etClass; }
	bool						IsClassType() const									{ return mModifiers.IsEmpty() && mBaseType == etClass; }
	String						ToString() const;
	void						FromString(const String& inString);
private:
	const ClassReflectionInfo*	mBaseClass;
	EType						mBaseType;
	TypeModifierStack			mModifiers;
};

class ClassMember
{
public:
	FullType						mType;
	String							mName;
	uint							mOffset;
};

class TypedPointer
{
public:
					TypedPointer() : mPointer(0) {}
					TypedPointer(const TypedPointer& inOther) :
						mType(inOther.mType),
						mPointer(inOther.mPointer)						{}
					TypedPointer(const FullType& inType, void* inPointer)
						: mType(inType),
						mPointer(inPointer)								{}
	TypedPointer	Dereferenced() const;
	size64			GetElementCount() const;
	TypedPointer	GetMember(const String& inMemberName);
	TypedPointer	PushNewArrayItem();
	void			ClearArray();
	bool			IsValidPointer() const								{ return mPointer != nullptr; }

	FullType		mType;
	void*			mPointer;
};

class ClassReflectionInfo
{
public:
	ClassName						mName;
	uint							mSize;
	uint							mAlign;
	Reflected::InstanceFunction*	mInstanceFunction;
	Reflected::AssignFunction*		mAssignFunction;
	Reflected::CopyFunction*		mCopyFunction;
	Reflected::DestructFunction*	mDestructorFunction;
	Array<ClassMember>				mMembers;
};

// Inspect code to generate a TypeInfo from a class <T>
template <typename T> void gInspectDeclaration(FullType& ioInfo)		{ InspectFunc<T> p; p.Fill(ioInfo); }
// Various templates to generate various types of type-specific stuff
template <typename T> struct InspectFunc								{ void Fill(FullType& ioInfo) const { ioInfo.mName = gGetNameOfType<T>(); ioInfo.mBaseType = etClass; } };
template <> struct InspectFunc<int>										{ void Fill(FullType& ioInfo) const { ioInfo.mBaseType = etInt;		} };
template <> struct InspectFunc<float>									{ void Fill(FullType& ioInfo) const { ioInfo.mBaseType = etFloat;	} };
template <> struct InspectFunc<bool>									{ void Fill(FullType& ioInfo) const { ioInfo.mBaseType = etBool;	} };
template <> struct InspectFunc<wchar_t>									{ void Fill(FullType& ioInfo) const { ioInfo.mBaseType = etWChar;	} };
template <> struct InspectFunc<char>									{ void Fill(FullType& ioInfo) const { ioInfo.mBaseType = etChar;	} };
template <> struct InspectFunc<unsigned char>							{ void Fill(FullType& ioInfo) const { ioInfo.mBaseType = etUChar;	} };
template <typename T> struct InspectFunc<Array<T>>						{ void Fill(FullType& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctArray);	} };
template <typename T> struct InspectFunc<BaseString<T>>					{ void Fill(FullType& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctString);	} };

// specialized container overloads; kinda sucks but couldn't get to pick up overloaded vers.
template <> struct InspectFunc<String>									{ void Fill(FullType& ioInfo) const { InspectFunc<char> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctString); } };
template <> struct InspectFunc<WString>									{ void Fill(FullType& ioInfo) const { InspectFunc<wchar_t> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctString); } };

template <typename T> struct InspectFunc <T*>							{ void Fill(FullType& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctPointer);	} };

class ObjectInspector
{
private:
	ClassReflectionInfo& mTargetDescription;
public:

	ObjectInspector(ClassReflectionInfo& inDescription) : mTargetDescription(inDescription) {}
	
	// static analysis entry point
	template <typename T> void StaticAnalysis(void)
	{
		mTargetDescription.mName = gGetNameOfType<T>();
		mTargetDescription.mSize = sizeof(T);
		mTargetDescription.mAlign= __alignof(T);
		mTargetDescription.mInstanceFunction = &(Reflected::sConstructInstance<T>);
		mTargetDescription.mCopyFunction = &(Reflected::sCopyInstance<T>);
		mTargetDescription.mDestructorFunction = &(Reflected::sDestructInstance<T>);
		((T*)0)->T::Inspect(*this);
	}

	// Visit callback during an inspection of an object
	template <typename T>
	void Inspect(T& inMemberObject, const String& inIdentifier)
	{
		ClassMember member_info;
		gInspectDeclaration<T>(member_info.mType);
		member_info.mOffset = ( uint)&inMemberObject;	// since inMemberObject is a member of a null-object, each offset is relative to zero
		member_info.mName = inIdentifier;
		mTargetDescription.mMembers.Append(member_info);
	}
};


// The register that keeps track of all Serializable classes and their inspected bits and pieces. Basically a reflection symbol table
class ReflectionHost
{
public:
	ReflectionHost() {}
	template<typename T>
	void								RegisterClassType()						// Registers a class, giving instance func and id
	{
		// fill description through static analysis
		ClassReflectionInfo description;
		ObjectInspector reflect_inspector(description);
		reflect_inspector.StaticAnalysis<T>();

		// add to hash-map, hashed on typename
		ItemEntry entry;
		entry.first = description.mName;
		entry.second = description;
		mItems.insert(entry);
	}

	TypedPointer						DestructClass(const ClassName& inClassName, void* inInstanceData);					// Calls destructor in single instance described by classname
	TypedPointer						CreateClass(const ClassName& inClassName, void* inDestination);						// Creates object in memory
	TypedPointer						CopyClass(const ClassName& inClassName, void* inDest, const void* inSource);		// calls copy constructor
	TypedPointer						AssignClass(const ClassName& inClassName, void* inDest, const void* inSource);		// calls assignment op

	// static classes to construct and destruct pointer/type tuples
	static TypedPointer					sCreateInstance(const FullType& inType, void* inInstanceData);
	static TypedPointer					sDestructInstance(const FullType& inType, void* inInstanceData);
	static TypedPointer					sCopyInstance(const FullType& inType, void* inDest, const void* inSource);
	static TypedPointer					sAssignInstance(const FullType& inType, void* inDest, const void* inSource);

	// find by name
	const ClassReflectionInfo*			FindClassInfo(const ClassName& inClassName) const;
	// find by static type
	template <typename T>
	const ClassReflectionInfo*			FindClassInfo() const
	{
		ClassName type_name = gGetNameOfType<T>();
		return FindClassInfo(type_name);
	}


private:
	typedef std::pair<ClassName, ClassReflectionInfo>	ItemEntry;
	std::unordered_map<ClassName, ClassReflectionInfo>	mItems;										// a map from id to instance function		
};

template<class T>
class AutoReflection
{
public:	
	AutoReflection()
	{
		ReflectionHost::sGetInstance().RegisterClassType<T>();
	}
};





// Class should be binary compatible with Array<everythingelse>
// only has a resize option; everything else is done by external code
class AnonymousArray
{
public:

	AnonymousArray()
	{
		mData = nullptr;
		mElementCount = 0;
		mReservedCount = 0;
	}

	TypedPointer GetElement(const FullType&  inType, size64 inElement)
	{
		gAssert(inElement < mReservedCount);
		return TypedPointer(inType, gOffsetPointer<void>(mData, inType.GetSize() * inElement));
	}

	size64 GetSize() const 
	{
		return mElementCount; 
	}

	void Resize(const FullType& inType, size64 inNewElementCount)
	{
		if (inNewElementCount == mElementCount)
			return;

		size64 size = inType.GetSize();
		if (inNewElementCount > mElementCount)
		{
			if (inNewElementCount > mReservedCount)
			{
				// re-alloc and copy over
				void* new_data = gAllocAligned(size * inNewElementCount, inType.GetAlignment());

				// copy over old instances
				for (uint64 i = 0; i < mElementCount; i++)
				{
					// first create one
					ReflectionHost::sCreateInstance(inType, reinterpret_cast<void*>(uint64(new_data) + size * i));
					// copy into
					ReflectionHost::sCopyInstance(inType, reinterpret_cast<void*>(uint64(new_data) + size * i), reinterpret_cast<void*>(uint64(mData) + size * i));
				}

				// Create fresh ones
				for (uint64 i = mElementCount; i < inNewElementCount; i++)
				{
					ReflectionHost::sCreateInstance(inType, reinterpret_cast<void*>(uint64(new_data) + size * i));
				}

				// Destruct old ones
				for (uint64 i = 0; i < mElementCount; i++)
				{
					ReflectionHost::sDestructInstance(inType, reinterpret_cast<void*>(uint64(mData) + size * i));
				}
				if (mData)
				{
					gFreeAligned(mData);
				}
				
				mData = new_data;
				mElementCount = inNewElementCount;
				mReservedCount = inNewElementCount;
			}
			else
			{
				// create new instances without re-alloc
				for (uint64 i = mElementCount; i < inNewElementCount; i++)
				{
					ReflectionHost::sCreateInstance(inType, reinterpret_cast<void*>(uint64(mData) + size * i));
				}
				mElementCount = inNewElementCount;
			}
		}
		else // new < old
		{

			// delete without re-alloc
			for (uint64 i = inNewElementCount; i < mElementCount; i++)
			{
				ReflectionHost::sDestructInstance(inType, reinterpret_cast<void*>(uint64(mData) + size * i));
			}
			mElementCount = inNewElementCount;

			// if there are no elements, remove entire buffer
			if (mElementCount == 0)
			{
				gFreeAligned(mData);
				mData = nullptr;
				mReservedCount = 0;
			}
		}
	}

	void Copy(const FullType& inType, const AnonymousArray& inSource)
	{
		Clear(inType);
		size64 size = inType.GetSize();
		Resize(inType, inSource.mElementCount);
		for (uint64 i = 0; i < mElementCount; i++)
		{
			ReflectionHost::sCopyInstance(inType, reinterpret_cast<void*>(uint64(mData) + size * i), reinterpret_cast<void*>(uint64(inSource.mData) + size * i));
		}
	}

	void Clear(const FullType& inType)
	{
		Resize(inType, 0);
	}


	~AnonymousArray()
	{
		// need to clear the array before destructing; don't know how to destruct properly without type info
		gAssert(mData == nullptr);
	}

	// Internal data
	void*	mData;					// Array data buffer. Should be non-null if [mReservedCount] > 0
	size64	mElementCount;			// The amount of constructed and valid elements within the buffer (in consecutive memory)
	size64	mReservedCount;			// Size of databuffer in elements (not bytes(!)). Should be zero if [mData] is null.
};

