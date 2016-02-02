#include "StdAfx.h"
#include "Reflection.h"


TypedPointer ReflectionHost::CopyClass(const ClassName& inClassName, void* inDest, const void* inSource)
{
	const ClassReflectionInfo* info = FindClassInfo(inClassName);
	TypedPointer typed_ptr;
	typed_ptr.mType.mBaseType = etClass;
	typed_ptr.mType.mModifiers.Clear();
	typed_ptr.mType.mName = inClassName;

	if (info != 0)
	{
		info->mCopyFunction(inDest, inSource);
	}
	else
	{
		gAssert(false); // cannot find info on this class name
	}
	return typed_ptr;
}

TypedPointer ReflectionHost::AssignClass(const ClassName& inClassName, void* inDest, const void* inSource)
{
	const ClassReflectionInfo* info = FindClassInfo(inClassName);
	TypedPointer typed_ptr;
	typed_ptr.mType.mBaseType = etClass;
	typed_ptr.mType.mModifiers.Clear();
	typed_ptr.mType.mName = inClassName;

	if (info != 0)
	{
		info->mAssignFunction(inDest, inSource);
	}
	else
	{
		gAssert(false); // cannot find info on this class name
	}
	return typed_ptr;
}

TypedPointer ReflectionHost::CreateClass(const ClassName& inClassName, void* inInstanceData)
{
	const ClassReflectionInfo* info = FindClassInfo(inClassName);
	TypedPointer typed_ptr;
	typed_ptr.mType.mBaseType = etClass;
	typed_ptr.mType.mModifiers.Clear();
	typed_ptr.mType.mName = inClassName;

	if (info != 0)
	{
		typed_ptr.mPointer = info->mInstanceFunction(inInstanceData);
	}
	else
	{
		gAssert(false); // cannot find info on this class name
	}
	return typed_ptr;
}


TypedPointer ReflectionHost::DestructClass(const ClassName& inClassName, void* inInstanceData)
{
	const ClassReflectionInfo* info = FindClassInfo(inClassName);
	TypedPointer typed_ptr;
	typed_ptr.mType.mBaseType = etClass;
	typed_ptr.mType.mModifiers.Clear();
	typed_ptr.mType.mName = inClassName;

	if (info != 0)
	{
		info->mDestructorFunction(inInstanceData, true);
	}
	else
	{
		gAssert(false); // cannot find info on this class name
	}
	return typed_ptr;
}



TypedPointer ReflectionHost::sAssignInstance(const FullType& inType, void* inDest, const void* inSource)
{
	gAssert(inDest != nullptr && inSource != nullptr);

	TypedPointer typed_ptr;
	typed_ptr.mType = inType;

	if (inType.mModifiers.IsContainer())
	{
		static_cast<AnonymousArray*>(inDest)->Clear(inType);
		static_cast<AnonymousArray*>(inDest)->Copy(inType, *(reinterpret_cast<const AnonymousArray*>(inSource)));
	}
	else if (inType.mModifiers.IsPointer())
	{
		*((void**)inDest) = *((void**)inSource);
	}
	else if (inType.IsPrimitiveType())
	{
		// simple mem-copy
		memcpy(inDest, inSource, inType.GetSize());
	}
	else
	{
		gAssert(!inType.mName.IsEmpty());
		ReflectionHost::sGetInstance().AssignClass(inType.mName, inDest, inSource);
	}
	return typed_ptr;
}



TypedPointer ReflectionHost::sCopyInstance(const FullType& inType, void* inDest, const void* inSource)
{
	gAssert(inDest != nullptr && inSource != nullptr);

	TypedPointer typed_ptr;
	typed_ptr.mType = inType;

	if (inType.mModifiers.IsContainer())
	{
		static_cast<AnonymousArray*>(inDest)->Clear(inType);
		static_cast<AnonymousArray*>(inDest)->Copy(inType, *(reinterpret_cast<const AnonymousArray*>(inSource)));
	}
	else if (inType.mModifiers.IsPointer())
	{
		*((void**)inDest) = *((void**) inSource);
	}
	else if (inType.IsPrimitiveType())
	{
		// simple mem-copy
		memcpy(inDest, inSource, inType.GetSize());
	}
	else
	{
		gAssert(!inType.mName.IsEmpty());
		ReflectionHost::sGetInstance().CopyClass(inType.mName, inDest, inSource);
	}
	return typed_ptr;
}



TypedPointer ReflectionHost::sDestructInstance(const FullType& inType, void* inInstanceData)
{
	gAssert(inInstanceData != NULL);

	TypedPointer typed_ptr;
	typed_ptr.mType = inType;

	if (inType.mModifiers.IsContainer())
	{
		static_cast<AnonymousArray*>(inInstanceData)->Clear(inType);
		static_cast<AnonymousArray*>(inInstanceData)->~AnonymousArray();
	}
	else if (inType.mModifiers.IsPointer())
	{
		*((void**)inInstanceData) = 0;	// simply put pointer to zero (no ref-counting)
	}
	else if (inType.IsPrimitiveType())
	{
		// do not initialize nothing for primitives; let the memory be
	}
	else
	{
		gAssert(!inType.mName.IsEmpty());
		ReflectionHost::sGetInstance().DestructClass(inType.mName, inInstanceData);
	}
	return typed_ptr;
}

TypedPointer ReflectionHost::sCreateInstance(const FullType& inType, void* inInstanceData)
{
	TypedPointer typed_ptr;
	typed_ptr.mType = inType;

	if (inType.mModifiers.IsContainer())
	{
		FullType internal_type = inType;
		internal_type.mModifiers.Pop();
		size64 internal_size = internal_type.GetSize();
		if (inType.mModifiers.IsArray())	typed_ptr.mPointer = static_cast<void*>(inInstanceData != 0 ? new (inInstanceData) Array<byte>() : new Array<byte>());
		if (inType.mModifiers.IsString())	typed_ptr.mPointer = static_cast<void*>(inInstanceData != 0 ? new (inInstanceData) BaseString<byte>() : new BaseString<byte>());
	}
	else if (inType.mModifiers.IsPointer())
	{
		typed_ptr.mPointer = static_cast<void*>(inInstanceData != 0 ? new (inInstanceData) void*(0) : new void*(0));
	}
	else if (inType.IsPrimitiveType())
	{
		if (inInstanceData == NULL)
		{
			assert(false); // not implemented the creation of single primitives
		}
		else
		{
			// do not initialize nothing for primitives; let the memory be
		}
		
	}
	else
	{
		gAssert(!inType.mName.IsEmpty());
		typed_ptr = ReflectionHost::sGetInstance().CreateClass(inType.mName, inInstanceData);
	}
	return typed_ptr;
}



const ClassReflectionInfo* ReflectionHost::FindClassInfo(const ClassName& inClassName) const
{
	stdext::hash_map<ClassName, ClassReflectionInfo>::const_iterator i = mItems.find(inClassName);
	if (i == mItems.end()) return 0;
	return &(i->second);
}

TypedPointer TypedPointer::Dereferenced() const
{
	ETypeModifier ct = mType.mModifiers.Back();
	assert(ct == ctPointer || ct == ctArray || ct == ctString);
	TypedPointer tp(*this);
	tp.mType.mModifiers.Pop();
	if (ct == ctPointer)
	{
		tp.mPointer = *((void**) tp.mPointer);
		return tp;
	}
	else if (ct == ctArray)
	{
		Array<byte>* base_vector = (Array<byte>*)mPointer;
		tp.mPointer = base_vector->GetData();
		return tp;
	}
	else // ct_string
	{
		BaseString<byte>* base_string = (BaseString<byte>*)mPointer;
		tp.mPointer = base_string->GetData();
		return tp;
	}
}



void TypedPointer::ClearArray()
{

}

TypedPointer TypedPointer::PushNewArrayItem()
{
	gAssert(mType.mModifiers.IsArray());
	FullType internal_type;
	mType.GetInternalType(internal_type);
	AnonymousArray* array = (AnonymousArray*) mPointer;
	array->Resize(internal_type, array->GetSize() + 1);
	return array->GetElement(internal_type, array->GetSize() - 1);
}

TypedPointer TypedPointer::GetMember(const String& inMemberName)
{
	// can only get member on non-reffed, non-array etc. only simple struct
	gAssert(!mType.IsPrimitiveType() && mType.mModifiers.IsOnly(ctNone));
	const ClassReflectionInfo*  class_info = ReflectionHost::sGetInstance().FindClassInfo(mType.mName);
	gAssert(class_info != 0);

	TypedPointer typed_ptr;
	for (int i = 0; i < class_info->mMembers.GetLength(); i++)
	{
		if (class_info->mMembers[i].mName.Equals(inMemberName))
		{
			typed_ptr.mType = class_info->mMembers[i].mType;
			typed_ptr.mPointer = (void*)(offset64(mPointer) + class_info->mMembers[i].mOffset);
			return typed_ptr;
		}
	}
	gAssert(false); // typed_ptr not found
	return typed_ptr;
}

size64 TypedPointer::GetElementCount() const
{
	ETypeModifier ct = mType.mModifiers.Back();
	assert(ct == ctString || ct == ctArray);
	if (ct == ctArray)
	{
		Array<byte>* base_vector = (Array<byte>*)mPointer;
		return base_vector->GetLength();
	}
	else
	{
		BaseString<byte>* base_string = (BaseString<byte>*)mPointer;
		return base_string->GetLength();
	}
}

size64 FullType::GetSize() const
{
	if (mModifiers.IsEmpty())
	{
		if (mBaseType == etClass)
		{
			const ClassReflectionInfo* desc = ReflectionHost::sGetInstance().FindClassInfo(mName);
			return (desc == 0) ? 0: desc->mSize;
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


size64 FullType::GetAlignment() const
{
	if (mModifiers.IsEmpty())
	{
		if (mBaseType == etClass)
		{
			const ClassReflectionInfo* desc = ReflectionHost::sGetInstance().FindClassInfo(mName);
			return (desc == 0) ? 0 : desc->mAlign;
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

String FullType::ToString() const
{
	String str;
	if (mBaseType == etClass)
	{
		str = mName;
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
void FullType::FromString(const String& inString)
{
	mModifiers.Clear();
	mName.Clear();
	for (uint c = 0; c < inString.GetLength(); c++)
	{
		char t = inString[c];
		if		(t == '[') { mModifiers.Push(ctArray); c++; }
		else if	(t == '(') { mModifiers.Push(ctString); c++; }
		else if	(t == '*') { mModifiers.Push(ctPointer); }
		else mName.Append(t);
	}
}