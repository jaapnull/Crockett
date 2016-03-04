#include <CCore/Assert.h>
#include <CCore/Array.h>
#include <CCore/String.h>

enum EType { etCompound, etInt, etFloat, etShort, etUShort, etChar, etUInt, etUChar, etDouble, etBool, etWChar, etNullptr, etCount, etInvalid = etCount };
enum ETypeDecoration { ctNone = 0, ctPointerTo = 1, ctArrayOf = 2, ctStringOf = 3 };


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
		ETypeDecoration				Outer()								const	{ return ETypeDecoration(mData & 0x3); }
		ETypeDecoration				operator[](uint inIndex)			const	{ return ETypeDecoration((mData >> (inIndex * 2)) & 0x3); }
		bool						IsOnly(ETypeDecoration inContainer)	const	{ return (mData == inContainer); }
		bool						IsEmpty()							const	{ return mData == 0; }
		void						Pop()										{ mData >>= 2; }
		void						Push(ETypeDecoration inContainer)			{ gAssert(inContainer != ctNone); mData <<= 2; mData |= uint32(inContainer); }
		bool						OuterIsArrayOf()					const	{ return (Outer() == ctArrayOf); }
		bool						OuterIsStringOf()					const	{ return (Outer() == ctStringOf); }
		bool						OuterIsPointerTo()					const	{ return (Outer() == ctPointerTo); }
		bool						OuterIsContainer()					const	{ return (Outer() == ctArrayOf || Outer() == ctStringOf); }
		uint						GetLength()							const	{ for (uint s = 0; s < 16; s++) if ((mData >> (s * 2)) == 0) return s; return 16; }
		void						Clear()										{ mData = 0; }
	private:
		uint32						mData;										// Fixed size stack containing the various modifiers (outer modifier is in LSB)
	};

	TypeDecl(EType inType) :
		mNakedType(inType),
		mCompoundInfo(nullptr)													{ gAssert(IsValid() ); 
	}

	TypeDecl() :
		mNakedType(etInvalid),
		mCompoundInfo(nullptr) {}

	bool							IsValid()							const	{ return (mNakedType < etCount) && (mNakedType != etCompound || mCompoundInfo != nullptr); }
	bool							IsNakedPrimitive()					const	{ return mModifiers.IsEmpty() && mNakedType != etCompound; }
	bool							IsNakedCompound()					const	{ return mModifiers.IsEmpty() && mNakedType == etCompound; }
	void							GetPeeled(TypeDecl& outType)		const	{ gAssert(!mModifiers.IsEmpty()); outType = *this; outType.mModifiers.Pop(); }
	ETypeDecoration					GetOuterDecoration()				const	{ return mModifiers.Outer(); }
	EType							GetNakedType()						const	{ return mNakedType; }
	String							ToString() const;

	size64							GetSizeInBytes() const;
	size64							GetAlignment() const;

	const CompoundReflectionInfo*	mCompoundInfo;								// pointer to info about compound if nakedtype is a compound
	EType							mNakedType;									// naked base class (naked class of Array<int>*, int* or Array<int*> would all be int)
	DecorationStack					mModifiers;									// stack of modifieds like pointer-to, array-of etc.
};

struct ClassMember
{
	TypeDecl						mType;
	String							mName;
	offset64						mOffset;
};


struct CompoundReflectionInfo
{
public:
	typedef void*					(ReflectInstanceFunction)(void*);				// InstanceFunction is used to create a "clean" instance for serialization
	typedef void					(ReflectCopyFunction)(void*, const void*);		// CopyFunction is used to copy one object onto the other
	typedef void					(ReflectAssignFunction)(void*, const void*);	// CopyFunction is used to copy one object onto the other
	typedef void					(ReflectDestructFunction)(void*, bool);			// DestructFunction is used to delete a created object, either releasing or keeping memory intact

	ClassName						mName;
	uint							mSize;
	uint							mAlign;
	ReflectInstanceFunction*		mInstanceFunction;
	ReflectAssignFunction*			mAssignFunction;
	ReflectCopyFunction*			mCopyFunction;
	ReflectDestructFunction*		mDestructorFunction;
	Array<ClassMember>				mMembers;
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

	const TypedPointer				DerefPointer() const;							// Deref pointer
	const size64					GetContainerElementCount() const;				// Get element count in case of container type
	TypedPointer					GetContainerElement(size64 inIndex) const;		// Get element from container type
	TypedPointer					GetCompoundMember(const String& inMemberName) const;	// Get element from container type
	TypedPointer					GetObjectAtPath(const String& inPath);			// Evaluate a path and return object that is pointed to

	TypeDecl						mType;
	void*							mPointer;
};

