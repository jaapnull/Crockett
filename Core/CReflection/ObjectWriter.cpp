#include <CorePCH.h>

#include <CUtils/StringUtils.h>
#include <CReflection/ObjectWriter.h>
#include <CReflection/Serializing.h>

static bool sIsInline(const TypeDecl& inDecl)
{
	return
		(inDecl.IsNakedPrimitive()) ||
		(inDecl.GetOuterDecoration() == ctPointerTo) ||
		(inDecl.GetOuterDecoration() == ctStringOf) ||
		(inDecl.IsNakedArray() && inDecl.GetNakedType() != etCompound);
		
}

bool ObjectWriter::WriteObject(const TypedPointer& inTypedPointer)
{
	if (inTypedPointer.mType.mModifiers.IsEmpty())
	{
		if (inTypedPointer.mType.mNakedType == etCompound)
		{
			const CompoundReflectionInfo* info = inTypedPointer.mType.mCompoundInfo;
			if (info == 0)
			{
				gAssert(false); // invalid mType; every etCompound should have compoundinfo
				return 0; 
			}
			
			mOutStream << IndentStart() << "{\n";
			for (uint m = 0; m < info->mMembers.GetLength(); m++)
			{
				ClassMember tm = info->mMembers[m];
				// do not write out debug data
				if (tm.mName[0] == '!') 
					continue;

				TypedPointer tp(tm.mType, (void*)(((byte*)inTypedPointer.mPointer) + tm.mOffset));
				mOutStream << Indent() << tm.mName << " = ";
				if (sIsInline(tp.mType)) 
				{
					WriteObject(tp); mOutStream << "\n"; }
				else
				{ mOutStream << "\n";  WriteObject(tp); mOutStream << "\n"; }
			}
			mOutStream << IndentStop() << "}";
			return true;
		}
		else
		{
			void* ptr = inTypedPointer.mPointer;
			switch (inTypedPointer.mType.mNakedType)
			{
				case etInt:		mOutStream << gToString(*(int*)ptr); return true;
				case etFloat:	mOutStream << gToString(*(float*)ptr); return true;
				case etShort:	mOutStream << gToString(*(short*)ptr); return true;
				case etUShort:	mOutStream << gToString(*(ushort*)ptr); return true;
				case etChar:	mOutStream << String(1, *(char*)ptr); return true;
				case etUInt:	mOutStream << gToString(*(uint*)ptr); return true;
				case etUChar:	mOutStream << String(1, *(uchar*)ptr); return true;
				case etDouble:	mOutStream << gToString(*(double*)ptr); return true;
				case etBool:	mOutStream << gToString(*(bool*)ptr); return true;
				case etWChar:	mOutStream << gToString(*(wchar_t*)ptr); return true;
				default:		return false;
			}
		}
	}
	else
	{
		ETypeDecoration outer_container = inTypedPointer.mType.mModifiers.Outer();
		// Write dependency reference
		if (outer_container == ctPointerTo)
		{
			TypedPointer deref = TypedPointerPointer(inTypedPointer).DerefPointer();
			if (deref.mPointer == nullptr)
			{
				mOutStream << "<>";
			}
			else
			{
				TypedPointer name_field = deref.GetObjectAtStringPath("!name");
				gAssert(name_field.IsValid() && name_field.mType.IsCharString());
				TypedPointer location_field = deref.GetObjectAtStringPath("!location");
				if (location_field.IsValid())
				{
					gAssert(location_field.mType.IsCharString());
					mOutStream << '<' << *((String*)location_field.mPointer) << ":" << *((String*)name_field.mPointer) << ">";
				}
				else
				{
					mOutStream << '<' << *((String*)name_field.mPointer) << ">";
				}
			}
			return true;
		}
		
		
		if (outer_container == ctArrayOf)
		{
			TypedArrayPointer array_pointer(inTypedPointer);
			size64 elem_count = array_pointer.GetContainerElementCount();
			TypeDecl peeled_type = inTypedPointer.mType.GetPeeled();
			bool array_inline = sIsInline(inTypedPointer.mType);
			if (!array_inline)
				mOutStream << IndentStart();
			mOutStream << "[";
			if (!array_inline) mOutStream << '\n';
			for (size64 c = 0; c < elem_count; c++)
			{
				if (sIsInline(peeled_type))
				{
					if (!array_inline) mOutStream << Indent();
					WriteObject(array_pointer.GetContainerElement(c));
				}
				else
				{
					WriteObject(array_pointer.GetContainerElement(c));
				}
				if (c != elem_count-1) 
					mOutStream << ",";
				if (!array_inline)
					mOutStream << "\n";

			}
			if (!array_inline)
				mOutStream << IndentStop();
			mOutStream << "]";
			return true;
		}
		else // ctStringOf:
		{
			TypeDecl peeled = inTypedPointer.mType.GetPeeled();
			// only support char and wchar types in a string
			gAssert(peeled.IsNakedPrimitive());
			if (peeled.mNakedType == etChar)
			{ 
				mOutStream << '\"' << (*((String*)inTypedPointer.mPointer)) << '\"';
				return true; 
			}
			else
			{
				gAssert(false);
				return false;
			}
			return true;
		}
	}
	return false;
}

