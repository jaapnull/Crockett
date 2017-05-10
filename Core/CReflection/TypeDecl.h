#pragma once
#include <CCore/types.h>
#include <CCore/string.h>
#include <CCore/array.h>

enum EType				{ etCompound, etInt, etFloat, etShort, etUShort, etChar, etUInt, etUChar, etDouble, etBool, etWChar, etNullptr, etCount, etInvalid = etCount };
enum ETypeDecoration	{ ctNone = 0, ctPointerTo = 1, ctArrayOf = 2, ctStringOf = 3 };

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

struct CompoundReflectionInfo;

struct TypeDecl
{
	class DecorationStack
	{
	public:
		DecorationStack() : mData(0) {}
		ETypeDecoration				Outer()								const { return ETypeDecoration(mData & 0x3); }
		ETypeDecoration				operator[](uint inIndex)			const { return ETypeDecoration((mData >> (inIndex * 2)) & 0x3); }
		bool						operator==(const DecorationStack& s)const { return mData == s.mData; }
		bool						IsOnly(ETypeDecoration inContainer)	const { return (mData == inContainer); }
		bool						IsEmpty()							const { return mData == 0; }
		void						Pop() { mData >>= 2; }
		void						Push(ETypeDecoration inContainer) { gAssert(inContainer != ctNone); mData <<= 2; mData |= uint32(inContainer); }
		bool						OuterIsArrayOf()					const { return (Outer() == ctArrayOf); }
		bool						OuterIsStringOf()					const { return (Outer() == ctStringOf); }
		bool						OuterIsPointerTo()					const { return (Outer() == ctPointerTo); }
		bool						OuterIsContainer()					const { return (Outer() == ctArrayOf || Outer() == ctStringOf); }
		uint						GetLength()							const { for (uint s = 0; s < 16; s++) if ((mData >> (s * 2)) == 0) return s; return 16; }
		void						Clear() { mData = 0; }
	private:
		uint32						mData;										// Fixed size stack containing the various modifiers (outer modifier is in LSB)
	};

	TypeDecl(EType inType) :
		mNakedType(inType),
		mCompoundInfo(nullptr) {
		gAssert(IsValid());
	}

	TypeDecl(const CompoundReflectionInfo*	inCompoundInfo) :
		mNakedType(etCompound),
		mCompoundInfo(inCompoundInfo) {
		gAssert(IsValid());
	}

	TypeDecl() :
		mNakedType(etInvalid),
		mCompoundInfo(nullptr) {}

	bool							IsValid()					const { return (mNakedType < etCount) && (mNakedType != etCompound || mCompoundInfo != nullptr); }
	bool							IsNakedPrimitive()			const { return mModifiers.IsEmpty() && mNakedType != etCompound; }
	bool							IsNakedArray()				const { return mModifiers.IsOnly(ctArrayOf); }
	bool							IsNakedString()				const { return mModifiers.IsOnly(ctStringOf); }
	bool							IsNakedPointer()			const { return mModifiers.IsOnly(ctPointerTo); }
	bool							IsNakedCompound()			const { return mModifiers.IsEmpty() && mNakedType == etCompound; }
	bool							IsCharString()				const { return mModifiers.OuterIsStringOf() && mNakedType == etChar && mModifiers.GetLength() == 1; }

	bool							operator==(const TypeDecl& inOtherType) const
	{
		return	mCompoundInfo == inOtherType.mCompoundInfo &&
				mNakedType == inOtherType.mNakedType &&
				mModifiers == inOtherType.mModifiers;
	}

	void							Clear()								{ mCompoundInfo = nullptr; mNakedType = etInvalid; mModifiers.Clear(); }
	TypeDecl						GetPeeled()					const	{ TypeDecl td = *this;  gAssert(!mModifiers.IsEmpty()); td.mModifiers.Pop(); return td; }
	ETypeDecoration					GetOuterDecoration()		const	{ return mModifiers.Outer(); }
	EType							GetNakedType()				const	{ return mNakedType; }
	String							ToString() const;

	size64							GetSizeInBytes() const;
	size64							GetAlignment() const;

	const CompoundReflectionInfo*	mCompoundInfo =	nullptr;	// pointer to info about compound if nakedtype is a compound
	EType							mNakedType =	etInvalid;	// naked base class (naked class of Array<int>*, int* or Array<int*> would all be int)
	DecorationStack					mModifiers;					// stack of modifieds like pointer-to, array-of etc.
};

struct ClassMember
{
	TypeDecl						mType;
	String							mName;
	size64						mOffset;
};


struct CompoundReflectionInfo
{
public:
	typedef const type_info&		(ReflectInfoFunction)(void*);					// InstanceFunction is used to create a "clean" instance for serialization
	typedef void*					(ReflectInstanceFunction)(void*);				// InstanceFunction is used to create a "clean" instance for serialization
	typedef void					(ReflectCopyFunction)(void*, const void*);		// CopyFunction is used to copy one object onto the other
	typedef void					(ReflectAssignFunction)(void*, const void*);	// CopyFunction is used to copy one object onto the other
	typedef void					(ReflectDestructFunction)(void*, bool);			// DestructFunction is used to delete a created object, either releasing or keeping memory intact

	bool							IsDeclaredOnly()						const	{ return !mName.IsEmpty() && mSize == 0 && mAlign == 0; }

	ClassName						mName;
	uint							mSize						= 0;
	uint							mAlign						= 0;
	ReflectInfoFunction*			mInfoFunction				= nullptr;
	ReflectInstanceFunction*		mInstanceFunction			= nullptr;
	ReflectAssignFunction*			mAssignFunction				= nullptr;
	ReflectCopyFunction*			mCopyFunction				= nullptr;
	ReflectDestructFunction*		mDestructorFunction			= nullptr;
	Array<ClassMember>				mMembers;
};


// Crazy template stuff that generates a TypedDecl from a static type of object
template <typename T> 
TypeDecl gInspectDeclaration() { TypeDecl info;  InspectFunc<T> p; p.Fill(info); return info;  }

// Various templates to generate various types of type-specific stuff
template <typename T>	struct InspectFunc					{ void Fill(TypeDecl& ioInfo) const { ioInfo.mCompoundInfo = ReflectionHost::sGetReflectionHost().FindOrCreateCompoundInfoStatic<T>(); ioInfo.mNakedType = etCompound; } };
template <>				struct InspectFunc<int>				{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etInt; } };
template <>				struct InspectFunc<float>			{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etFloat; } };
template <>				struct InspectFunc<bool>			{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etBool; } };
template <>				struct InspectFunc<wchar_t>			{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etWChar; } };
template <>				struct InspectFunc<char>			{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etChar; } };
template <>				struct InspectFunc<unsigned char>	{ void Fill(TypeDecl& ioInfo) const { ioInfo.mNakedType = etUChar; } };
template <typename T>	struct InspectFunc<Array<T>>		{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctArrayOf); } };
template <typename T>	struct InspectFunc<BaseString<T>>	{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf); } };

// specialized container overloads; kinda sucks but couldn't get to pick up overloaded versions; of course can change/adjust later
template <>				struct InspectFunc<String>			{ void Fill(TypeDecl& ioInfo) const { InspectFunc<char> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf); } };
template <>				struct InspectFunc<WString>			{ void Fill(TypeDecl& ioInfo) const { InspectFunc<wchar_t> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf); } };
template <typename T>	struct InspectFunc <T*>				{ void Fill(TypeDecl& ioInfo) const { InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctPointerTo); } };

// Crazy template stuff that generates a TypedDecl from a static type of object
template <typename T>
TypeDecl gLookupTypeForObject(T& inObject)
{ 
	TypeDecl info;  
	DynamicInspectFunc<T> p;
	p.Fill(info, inObject); 
	return info; 
}

// Various templates to generate various types of type-specific stuff
template <typename T>	struct DynamicInspectFunc					{ void Fill(TypeDecl& ioInfo, const T& inObject) const				{ ioInfo.mCompoundInfo = ReflectionHost::sGetReflectionHost().FindCompoundInfoDynamic<T>(inObject); ioInfo.mNakedType = etCompound; } };
template <>				struct DynamicInspectFunc<int>				{ void Fill(TypeDecl& ioInfo, const int& inObject) const			{ ioInfo.mNakedType = etInt; } };
template <>				struct DynamicInspectFunc<float>			{ void Fill(TypeDecl& ioInfo, const float& inObject) const			{ ioInfo.mNakedType = etFloat; } };
template <>				struct DynamicInspectFunc<bool>				{ void Fill(TypeDecl& ioInfo, const bool& inObject) const			{ ioInfo.mNakedType = etBool; } };
template <>				struct DynamicInspectFunc<wchar_t>			{ void Fill(TypeDecl& ioInfo, const wchar_t& inObject) const		{ ioInfo.mNakedType = etWChar; } };
template <>				struct DynamicInspectFunc<char>				{ void Fill(TypeDecl& ioInfo, const char& inObject) const			{ ioInfo.mNakedType = etChar; } };
template <>				struct DynamicInspectFunc<unsigned char>	{ void Fill(TypeDecl& ioInfo, const unsigned char& inObject) const	{ ioInfo.mNakedType = etUChar; } };
template <typename T>	struct DynamicInspectFunc<Array<T>>			{ void Fill(TypeDecl& ioInfo, const Array<T>& inObject) const		{ InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctArrayOf); } };
template <typename T>	struct DynamicInspectFunc<BaseString<T>>	{ void Fill(TypeDecl& ioInfo, const BaseString<T>& inObject) const	{ InspectFunc<T> p; p.Fill(ioInfo); ioInfo.mModifiers.Push(ctStringOf); } };

// specialized container overloads; kinda sucks but couldn't get to pick up overloaded versions; of course can change/adjust later
template <>				struct DynamicInspectFunc<String>			{ void Fill(TypeDecl& ioInfo, const String& inObject) const			{ ioInfo.mNakedType = etChar; ioInfo.mModifiers.Push(ctStringOf); } };
template <>				struct DynamicInspectFunc<WString>			{ void Fill(TypeDecl& ioInfo, const WString& inObject) const		{ ioInfo.mNakedType = etWChar; ioInfo.mModifiers.Push(ctStringOf); } };
// this should have a fill(..const T*& but I coudn't get it to compile; so dropping the reference &
template <typename T>	struct DynamicInspectFunc<T*>				{ void Fill(TypeDecl& ioInfo, const T* inObject) const				{ DynamicInspectFunc<T> p;			p.Fill(ioInfo, *inObject); ioInfo.mModifiers.Push(ctPointerTo); } };



