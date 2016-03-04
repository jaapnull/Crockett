#include <CorePCH.h>
#include <CCore/Memory.h>
#include <CUtils/StringUtils.h>
#include <CReflection/TypedPointer.h>


size64 TypeDecl::GetSizeInBytes() const
{
	if (IsNakedCompound())
	{
		gAssert(mCompoundInfo != nullptr);
		return mCompoundInfo->mSize;
	}
	else if (IsNakedPrimitive())
	{
		switch (mNakedType)
		{
		case etInt: return sizeof(int);
		case etFloat: return sizeof(float);
		case etShort: return sizeof(short);
		case etUShort: return sizeof(ushort);
		case etChar: return sizeof(char);
		case etUInt: return sizeof(uint);
		case etUChar: return sizeof(uchar);
		case etDouble: return sizeof(double);
		case etBool: return sizeof(bool);
		case etWChar: return sizeof(wchar_t);
		default: return 0;
		}
	}
	else
	{
		gAssert(!mModifiers.IsEmpty());
		switch (mModifiers.Outer())
		{
		case ctPointerTo: return sizeof(void*);
		case ctArrayOf: return sizeof(Array<byte>);
		case ctStringOf: return sizeof(String);
		default: return 0;
		}
	}
}


size64 TypeDecl::GetAlignment() const
{
	if (mModifiers.IsEmpty())
	{
		if (mNakedType == etCompound)
		{
			return mCompoundInfo->mAlign;
		}
		else
		{
			switch (mNakedType)
			{
			case etInt: return __alignof(int);
			case etFloat: return __alignof(float);
			case etShort: return __alignof(short);
			case etUShort: return __alignof(ushort);
			case etChar: return __alignof(char);
			case etUInt: return __alignof(uint);
			case etUChar: return __alignof(uchar);
			case etDouble: return __alignof(double);
			case etBool: return __alignof(bool);
			case etWChar: return __alignof(wchar_t);
			default: return 0;
			}
		}
	}
	else
	{
		switch (mModifiers.Outer())
		{
		case ctPointerTo: return __alignof(void*);
		case ctArrayOf: return __alignof(Array<byte>);
		case ctStringOf: return __alignof(String);
		default: return 0;
		}
	}
}


static char* sTypeStrings[etCount - 1] = { "int", "float", "short", "short", "char", "uint", "uchar", "double", "bool", "wchar" };

String TypeDecl::ToString() const
{
	String str;
	if (mNakedType == etCompound)
	{
		gAssert(mCompoundInfo != nullptr);
		str = mCompoundInfo->mName;
	}
	else
	{
		str = sTypeStrings[uint(mNakedType) - 1];
	}

	for (uint c = 0; c < mModifiers.GetLength(); c++)
	{
		switch (mModifiers[c])
		{
		case ctArrayOf: str.Append("[]"); break;
		case ctStringOf: str.Append("()");  break;
		case ctPointerTo: str.Append('*'); break;
		default:
		case ctNone: assert(false); // shouldn't happen, ctNone cannot be on stack
		}
	}
	return str;
}


const TypedPointer TypedPointer::DerefPointer() const
{
	gAssert(mType.GetOuterDecoration() == ctPointerTo);
	TypeDecl peeled_type;
	mType.GetPeeled(peeled_type);
	return TypedPointer(peeled_type, *((void**)mPointer));
}


const size64 TypedPointer::GetContainerElementCount() const
{
	if (mType.GetOuterDecoration() == ctArrayOf)
	{
		Array<byte>* byte_arryay = (Array<byte>*) mPointer;
		size64 byte_count = byte_arryay->GetLength();
		TypeDecl peeled_type;
		mType.GetPeeled(peeled_type);
		return byte_count / peeled_type.GetSizeInBytes();
	}
	else
	{
		gAssert(false);
		return 0;
	}
}

TypedPointer TypedPointer::GetContainerElement(size64 inIndex) const
{
	if (mType.GetOuterDecoration() == ctArrayOf)
	{
		Array<byte>* byte_arryay = (Array<byte>*) mPointer;
		byte* byte_count = (byte*)byte_arryay->GetData();

		TypeDecl peeled_type;
		mType.GetPeeled(peeled_type);
		return TypedPointer(peeled_type, (void*)(byte_count + peeled_type.GetSizeInBytes() * inIndex));
	}
	else
	{
		gAssert(false);
		return TypedPointer();
	}
}


TypedPointer TypedPointer::GetCompoundMember(const String& inMemberName) const
{
	gAssert(mType.IsNakedCompound());
	for (const ClassMember& m : mType.mCompoundInfo->mMembers)
	{
		if (m.mName == inMemberName)
		{
			return TypedPointer(m.mType, gOffsetPointer<void>(mPointer, m.mOffset));
		}
	}
	gAssert(false);
	return TypedPointer(TypeDecl(etNullptr), nullptr);
}


TypedPointer TypedPointer::GetObjectAtPath(const String& inPath)
{
	Array<String> parts;
	gExplodeString(parts, inPath, String(".[]"));
	TypedPointer current = *this;
	for (const String& s : parts)
	{
		if (gIsNumeric(s[0])) // array index
		{
			gAssert(current.mType.GetOuterDecoration() == ctArrayOf);
			current = current.GetContainerElement(gStringToInt(s));
		}
		else
		{
			gAssert(current.mType.IsNakedCompound());
			current = current.GetCompoundMember(s);
		}
	}
	return current;
}