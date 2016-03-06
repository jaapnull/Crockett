#include <CReflection/TypeDecl.h>

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

	int mod_count = mModifiers.GetLength();
	for (uint c = mod_count; c > 0; c--)
	{
		switch (mModifiers[c-1])
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
