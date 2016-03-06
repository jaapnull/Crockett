#include <CorePCH.h>
#include <CCore/Memory.h>
#include <CUtils/StringUtils.h>
#include <CReflection/TypedPointer.h>


const TypedPointer TypedPointer::DerefPointer() const
{
	gAssert(mType.GetOuterDecoration() == ctPointerTo);
	TypeDecl peeled_type = mType.GetPeeled();
	return TypedPointer(peeled_type, *((void**)mPointer));
}


const size64 TypedPointer::GetContainerElementCount() const
{
	if (mType.GetOuterDecoration() == ctArrayOf)
	{
		Array<byte>* byte_arryay = (Array<byte>*) mPointer;
		size64 byte_count = byte_arryay->GetLength();
		TypeDecl peeled_type = mType.GetPeeled();
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
		TypeDecl peeled_type = mType.GetPeeled();
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