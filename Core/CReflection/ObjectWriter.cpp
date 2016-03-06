#include <CorePCH.h>
#include <CReflection/ObjectWriter.h>
#include <CUtils/StringUtils.h>

static bool sIsInline(const TypeDecl& inDecl)
{
	return inDecl.IsNakedPrimitive() || inDecl.GetOuterDecoration() == ctPointerTo;
}


bool ObjectWriter::WriteResource(Resource& inResource)
{
	gAssert(!inResource.mName.IsEmpty());
	TypeDecl type = gInspectObject(inResource);

	mOutStream << Indent() << type.ToString() << ":" << inResource.mName << " = ";
	TypedPointer tp(type, &inResource);
	return WriteObject(tp);
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
				TypedPointer tp(tm.mType, (void*)(((byte*)inTypedPointer.mPointer) + tm.mOffset));
				mOutStream << Indent() << tm.mName << " = ";
				if (sIsInline(tp.mType)) 
				{
					WriteObject(tp); mOutStream << ";\n"; }
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
			TypedPointer deref = inTypedPointer.DerefPointer();
			if (deref.mPointer == nullptr)
			{
				mOutStream << "<null>";
			}
			else
			{
				mOutStream << "<some " << deref.mType.ToString() << ">";
			}
			return true;
		}
		
		size64 elem_count = inTypedPointer.GetContainerElementCount();
		if (outer_container == ctArrayOf)
		{
			TypeDecl peeled_type = inTypedPointer.mType.GetPeeled();
			mOutStream << IndentStart() << "[\n";
			for (size64 c = 0; c < elem_count; c++)
			{
				if (sIsInline(peeled_type))
				{
					mOutStream << Indent();
					WriteObject(inTypedPointer.GetContainerElement(c));
				}
				else
				{
					WriteObject(inTypedPointer.GetContainerElement(c));
				}
				if (c != elem_count-1) 
					mOutStream << ",\n";
				else
					mOutStream << "\n";

			}
			mOutStream << IndentStop() << "]";
			return true;
		}
		else // ctStringOf:
		{
			TypeDecl peeled = inTypedPointer.mType.GetPeeled();
			// only support char and wchar types in a string
			gAssert(peeled.IsNakedPrimitive());
			if (peeled.mNakedType == etChar)
			{ 
				mOutStream << (*((String*)inTypedPointer.mPointer)); 
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

