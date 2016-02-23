#include <StdAfx.h>
#include <CReflection/Reflection.h>

ReflectionHost* ReflectionHost::sReflectionHost = nullptr;

const CompoundReflectionInfo* ReflectionHost::FindClassInfo(const ClassName& inClassName) const
{
	std::unordered_map<ClassName, CompoundReflectionInfo>::const_iterator i = mItems.find(inClassName);
	if (i == mItems.end()) return 0;
	return &(i->second);
}


size64 TypeDecl::GetSizeInBytes() const
{
	if (mModifiers.IsEmpty())
	{
		if (mBaseType == etClass)
		{
			gAssert(mClassInfo != nullptr);
			return mClassInfo->mSize;
		}
		else
		{
			switch (mBaseType)
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
	}
	else
	{
		switch (mModifiers.Back())
		{
			case ctPointer: return sizeof(void*);
			case ctArray: return sizeof(Array<byte>);
			case ctString: return sizeof(String);
			default: return 0;
		}
	}
}


size64 TypeDecl::GetAlignment() const
{
	if (mModifiers.IsEmpty())
	{
		if (mBaseType == etClass)
		{
			return mClassInfo->mAlign;
		}
		else
		{
			switch (mBaseType)
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
		switch (mModifiers.Back())
		{
		case ctPointer: return __alignof(void*);
		case ctArray: return __alignof(Array<byte>);
		case ctString: return __alignof(String);
		default: return 0;
		}
	}
}


static char* sTypeStrings[etCount - 1] = { "int", "float", "short", "short", "char", "uint", "uchar", "double", "bool", "wchar" };

String TypeDecl::ToString() const
{
	String str;
	if (mBaseType == etClass)
	{
		gAssert(mClassInfo != nullptr);
		mClassInfo->mName;
	}
	else
	{
		str = sTypeStrings[uint(mBaseType) - 1];
	}
	
	for (uint c = 0; c < mModifiers.GetLength(); c++)
	{
		switch (mModifiers[c])
		{
			case ctArray: str.Append("[]"); break;
			case ctString: str.Append("()");  break;
			case ctPointer: str.Append('*'); break;
			default:
			case ctNone : assert(false); // shouldn't happen, ctNone cannot be on stack
		}
	}
	return str;
}

