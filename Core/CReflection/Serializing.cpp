#include <CorePCH.h>
#include <CReflection/Serializing.h>
#include <CUtils/StringUtils.h>


bool ObjectStreamer::WriteInstance(const TypedPointer& inTypedPointer)
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
				if (!tp.mType.IsNakedPrimitive() && !tp.mType.GetOuterDecoration() == ctPointerTo) mOutStream << "\n";
				WriteInstance(tp);
				if (tp.mType.IsNakedPrimitive() || tp.mType.GetOuterDecoration() == ctPointerTo) mOutStream << ";\n";
			}
			mOutStream << IndentStop() << "}\n";
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
			mOutStream << IndentStart() << "[\n";
			for (size64 c = 0; c < elem_count; c++)
			{
				WriteInstance(inTypedPointer.GetContainerElement(c)); 
			}
			mOutStream << IndentStop() << "]\n";
			return true;
		}
		else // ctStringOf:
		{
			TypeDecl peeled;
			inTypedPointer.mType.GetPeeled(peeled);
			// only support char and wchar types in a string
			gAssert(peeled.IsNakedPrimitive());
			if (peeled.mNakedType == etChar)	{ mOutStream << (*((String*)inTypedPointer.mPointer)); return 1; }
//			else if (peeled.mNakedType == etWChar)	{ mOutStream << (*((WString*)inTypedPointer.mPointer)); return 1; }
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

