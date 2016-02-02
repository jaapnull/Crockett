#include "stdafx.h"
#include "Streaming.h"

bool ObjectWriter::WriteInstanceBase(const TypedPointer& inTypedPointer, const String& inIdentifier)
{
	WriteIdentifier(inTypedPointer, inIdentifier);
	return WriteInstanceInternal(inTypedPointer);
}

bool ObjectWriter::WriteInstanceInternal(const TypedPointer& inTypedPointer)
{
	if (inTypedPointer.mType.mModifiers.IsEmpty())
	{
		if (inTypedPointer.mType.mBaseType == etClass)
		{
			const ClassReflectionInfo* info = ReflectionHost::sGetInstance().FindClassInfo(inTypedPointer.mType.mName);
			if (info == 0)
			{
				assert(false);
				return 0; // unknown class type defined
			}
			WriteBeginComplexClass(*info);
			for (uint m = 0; m < info->mMembers.GetLength(); m++)
			{
				ClassMember tm = info->mMembers[m];
				TypedPointer tp(tm.mType, (void*)(((byte*)inTypedPointer.mPointer) + tm.mOffset));
				WriteMember(tm);
				WriteInstanceInternal(tp);
			}
			WriteEndComplexClass();
			return true;
		}
		else
		{
			void* ptr = inTypedPointer.mPointer;
			switch (inTypedPointer.mType.mBaseType)
			{
				case etInt:		WritePrimitive(*(int*)ptr); return true;
				case etFloat:	WritePrimitive(*(float*)ptr); return true;
				case etShort:	WritePrimitive(*(short*)ptr); return true;
				case etUShort:	WritePrimitive(*(ushort*)ptr); return true;
				case etChar:	WritePrimitive(*(char*)ptr); return true;
				case etUInt:	WritePrimitive(*(uint*)ptr); return true;
				case etUChar:	WritePrimitive(*(uchar*)ptr); return true;
				case etDouble:	WritePrimitive(*(double*)ptr); return true;
				case etBool:	WritePrimitive(*(bool*)ptr); return true;
				case etWChar:	WritePrimitive(*(wchar_t*)ptr); return true;
				default:		return false;
			}
		}
	}
	else
	{
		ETypeModifier outer_container = inTypedPointer.mType.mModifiers.Back();
		// Write dependency reference
		if (outer_container == ctPointer)
		{
			const Dependency& dep = AddDependency(inTypedPointer.Dereference());
			if (dep.mLocation.Equals(mCurrentLocation))
			{
				WriteInternalDependencyReference(dep);
			}
			else
			{
				WriteExternalDependencyReference(dep);
			}
			
			return true;
		}
		TypedPointer dereffed = inTypedPointer.Dereference();
		uint elem_count = inTypedPointer.GetElementCount();
		uint elem_size = dereffed.mType.GetSize();
		byte* base_data = (byte*)dereffed.mPointer;
		if (outer_container == ctArray)
		{
			WriteBeginArray(inTypedPointer);
			for (uint c = 0; c < elem_count; c++)
			{
				dereffed.mPointer = (base_data + c * elem_size);
				WriteInstanceInternal(dereffed);
			}
			WriteEndArray();
			return true;
		}
		else // ctString:
		{
			if (dereffed.mType.mBaseType == etChar  && dereffed.mType.mModifiers.IsEmpty()) { WritePrimitive(*((String*)inTypedPointer.mPointer)); return 1; }
			if (dereffed.mType.mBaseType == etWChar && dereffed.mType.mModifiers.IsEmpty()) { WritePrimitive(*((WString*)inTypedPointer.mPointer)); return 1; }

			WriteBeginString(inTypedPointer);
			for (uint c = 0; c < elem_count; c++)
			{
				dereffed.mPointer = (base_data + c * elem_size);
				WriteInstanceInternal(dereffed);
			}
			WriteEndString();
			return true;
		}
	}
	return false;
}


const Dependency& ObjectWriter::AddDependency(const TypedPointer& inTypedPointer)
{
	Dependency d;
	d.mObject = inTypedPointer;
	const Serializable* s = dynamic_cast<Serializable*>((Reflected*)inTypedPointer.mPointer);
	if (s)
	{
		d.mName = s->GetName();
		d.mLocation = s->GetLocation();
	}
	else
	{
		d.mName = inTypedPointer.mType.mName;
		d.mName.Append(':');
		d.mName.Append(gToString(mDependencies.GetLength()));
	}

	if (d.mLocation.IsEmpty())
	{
		d.mLocation = mCurrentLocation;
	}

	mDependencies.Append(d);
	return mDependencies.Back();
}

bool ObjectWriter::WriteStreamSet(const StreamSet& inSet)
{
	assert(mDependencies.IsEmpty());
	for (uint o = 0; o < inSet.GetLength(); o++)
	{
		// convert stream set to TypedPointers
		const ClassReflectionInfo* root_desc = inSet[o]->GetClassInfo();
		if (root_desc == 0) return false;
		FullType d;
		d.mName = root_desc->mName;
		d.mBaseType = etClass;
		TypedPointer typed_pointer(d, (void*)inSet[o]);
		
		// set location of object
		inSet[o]->SetLocation(mCurrentLocation);

		WriteInstanceBase(typed_pointer, inSet[o]->GetName());
	}

	for (uint i = 0; i < mDependencies.GetLength(); i++)
	{
		Dependency& d(mDependencies[i]);

		if (d.mLocation.Equals(mCurrentLocation)) // we have a dependency that is not stored anywhere (or is not a resource); save it inside the file
		{
			WriteInstanceBase(d.mObject, d.mName);
		}
		else
		{
			// we assume the dependency exists and will ignore for now
		}
	}

	mCurrentLocation.Clear();
	return true;
}

