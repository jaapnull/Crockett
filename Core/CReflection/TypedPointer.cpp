#include <CorePCH.h>
#include <CCore/Memory.h>
#include <CMath/Math.h>
#include <CUtils/StringUtils.h>
#include <CReflection/TypedPointer.h>
#include <CReflection/Reflection.h>


const TypedPointer TypedPointerPointer::DerefPointer() const
{
	gAssert(mType.GetOuterDecoration() == ctPointerTo);
	TypeDecl peeled_type = mType.GetPeeled();
	return TypedPointer(peeled_type, *((void**)mPointer));
}


const size64 TypedArrayPointer::GetContainerElementCount() const
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


TypedPointer TypedArrayPointer::GetContainerElement(size64 inIndex) const
{
	if (mType.GetOuterDecoration() == ctArrayOf)
	{
		Array<byte>* byte_arryay = (Array<byte>*) mPointer;
		void* byte_count = (void*)byte_arryay->GetData();
		TypeDecl peeled_type = mType.GetPeeled();
		return TypedPointer(peeled_type, (((byte*)(byte_count)) + peeled_type.GetSizeInBytes() * inIndex));
	}
	else
	{
		gAssert(false);
		return TypedPointer();
	}
}


TypedPointer TypedArrayPointer::CreateNewArrayItem()
{
	TypedPointer tp;
	TypeDecl element_type		= mType.GetPeeled();
	size64 element_size			= element_type.GetSizeInBytes();
	size64 item_size			= element_type.GetSizeInBytes();
	Array<byte>* this_array		= (Array<byte>*)mPointer;

	// This is a specialized piece of code that works outside of array code, but is very much intertwined with it
	void* begin_data			= this_array->GetData();
	void* end_data				= this_array->_GetEndValid();
	void* end_reserved			= this_array->_GetEndReserved();
	size64 element_count		= ((byte*)end_data - (byte*)begin_data) / element_size;
	size64 bytes_left			= (byte*)end_reserved - (byte*)end_data;

	// Set the new pointers to the naive case of an easy push without resize
	void* new_begin_data		= begin_data;
	void* new_end_data			= gOffsetPointer(end_data, element_size);
	void* new_end_reserved		= end_reserved;
	void* new_element_ptr		= end_data;

	if (bytes_left < element_type.GetSizeInBytes()) // need resize/relocate
	{
		size64 alloc_size		= element_size * gMax<size64>((element_count * 2), 8);
		new_begin_data			= LinearAllocator<byte>::sRawAlloc(alloc_size);
		new_end_data			= gOffsetPointer(new_begin_data, (element_count + 1) * element_size);
		new_element_ptr			= gOffsetPointer(new_begin_data, element_count * element_size);
		new_end_reserved		= gOffsetPointer(new_begin_data, alloc_size);

		// copy over old data
		// we can copy anything but naked compounds; they need proper copy/move
		if (element_type.IsNakedCompound())
		{
			for (size64 i = 0; i < element_count; i++)
			{
				element_type.mCompoundInfo->mCopyFunction(gOffsetPointer(new_begin_data, element_size * i), gOffsetPointer(begin_data, element_size * i));
				element_type.mCompoundInfo->mDestructorFunction(gOffsetPointer(begin_data, element_size * i), true);
			}
		}
		else if (element_count > 0)
		{
			// everything else can be memcpied
			memcpy(new_begin_data, begin_data, element_count * element_size);
		}

		// Delete old data
		if (begin_data != nullptr)
		{
			LinearAllocator<byte>::sRawFree((byte*)begin_data);
		}

	}

	if (element_type.IsNakedCompound())
	{
		element_type.mCompoundInfo->mInstanceFunction(new_element_ptr);
	} 
	else if (element_type.GetOuterDecoration() == ctArrayOf)
	{
		new (new_element_ptr) Array<byte>();
	}
	else
	{
		memset(new_element_ptr, 0, element_size);
	}

	this_array->_SetPointer(new_begin_data, new_end_data, new_end_reserved);

	return TypedPointer(element_type, new_element_ptr);
}


TypedPointer TypedCompoundPointer::GetCompoundMember(const String& inMemberName) const
{
	gAssert(mType.IsNakedCompound());
	for (const ClassMember& m : mType.mCompoundInfo->mMembers)
	{
		if (m.mName == inMemberName)
		{
			return TypedPointer(m.mType, gOffsetPointer<void>(mPointer, m.mOffset));
		}
	}
	return TypedPointer(TypeDecl(etNullptr), nullptr);
}


uint32 TypedCompoundPointer::GetCompoundMemberIndex(const String& inMemberName) const
{
	gAssert(mType.IsNakedCompound());

	
	for (uint32 i = 0; i < mType.mCompoundInfo->mMembers.GetLength(); i++)
	{
		if (mType.mCompoundInfo->mMembers[i].mName == inMemberName)
		{
			return i;
		}
	}
	return -1;
}


TypedPointer TypedPointer::GetObjectAtPath(const Array<ReflectPathPart>& inPath)
{
	TypedPointer current = *this;
	for (const ReflectPathPart& path : inPath)
	{
		if (current.mType.IsNakedArray())
		{
			current = TypedArrayPointer(current).GetContainerElement(path.mOffset);
		}
		else
		{
			current = TypedCompoundPointer(current).GetCompoundMemberByIndex(path.mOffset);
		}
	}
	return current;
}


TypedPointer TypedPointer::GetObjectAtStringPath(const String& inPath)
{
	Array<String> parts;
	gExplodeString(parts, inPath, String(".[]"));
	TypedPointer current = *this;
	for (const String& s : parts)
	{
		if (gIsNumeric(s[0])) // array index
		{
			gAssert(current.mType.GetOuterDecoration() == ctArrayOf);
			current = TypedArrayPointer(current).GetContainerElement(gStringToInt(s));
		}
		else
		{
			gAssert(current.mType.IsNakedCompound());
			current = TypedCompoundPointer(current).GetCompoundMember(s);
		}
	}
	return current;
}


String TypedPointer::ResolvePathToString(const Array<ReflectPathPart>& inPath) const
{
	String path;
	TypedPointer current = *this;
	for (const ReflectPathPart& p : inPath)
	{
		if (current.mType.mModifiers.OuterIsArrayOf())
		{
			path.Append('[');
			path.Append(gToString(p.mOffset));
			path.Append(']');
			current = TypedArrayPointer(current).GetContainerElement(p.mOffset);

		}
		else if (current.mType.IsNakedCompound())
		{
			path.Append('.');
			path.Append(TypedCompoundPointer(current).mType.mCompoundInfo->mMembers[p.mOffset].mName);
			current = TypedCompoundPointer(current).GetCompoundMemberByIndex(p.mOffset);
		}
	}
	return path;
}

