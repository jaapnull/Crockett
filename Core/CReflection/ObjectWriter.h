#pragma once
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/Serializing.h>

// Helper class that writes out reflected objects to (file) stream
class ObjectWriter
{
public:
	ObjectWriter(Stream& outStream) : mOutStream(outStream), mIndent(0)	{}
	const Path&			GetTargetLocation()									{ return mOutStream.GetPath(); }
	String				IndentStart()										{ mIndent++; return String((mIndent - 1) * 2, ' '); }
	String				IndentStop()										{ mIndent--; return String((mIndent) * 2, ' '); }
	String				Indent()											{ return String((mIndent)* 2, ' '); }
	bool				WriteObject(const TypedPointer& inTypedPointer, bool inOutputDebugFields);

	template <class T>
	bool				WriteResource(T& inResource, bool inOutputDebugFields)
	{
		TypedPointer tp = gInspectObject(inResource);
		String* name = gGetDebugField<String>(inResource, "!name");
		gAssert(name != nullptr);
		mOutStream << Indent() << tp.mType.ToString() << ":" << *name << " = ";
		return WriteObject(tp, true);
		return false;
	}

	protected:
	Array<Dependency>	mDependencies;
	Stream&				mOutStream;
	int					mIndent;

};
