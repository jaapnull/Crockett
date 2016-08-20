#include <CReflection/TypedPointer.h>
#include <CReflection/Serializing.h>
#include <CCore/String.h>
#include <CStreams/Streams.h>


class ObjectCollection
{
public:

	template<class T>
	void AddObject(T* inClass)
	{
		TypedCompoundPointer tp = gInspectObject(*inClass);
		mObjects.Append(tp);
	}

	template<class T>
	T* FindObject(String inName)
	{
		for (const TypedCompoundPointer& tp : mObjects)
		{
			if (tp.mType.IsNakedCompound() && TypeDecl(tp.mType.mCompoundInfo) == gInspectDeclaration<T>())
			{
				String* name = TypedCompoundPointer(tp).GetCompoundMember<String>("!name");
				if (*name == inName)
				{
					return (T*)tp.mPointer;
				}
			}
		}
		return nullptr;
	}

	void LoadFromStream(const Path& inPath);
	void SaveToStreams();

	Array<TypedCompoundPointer>&			GetObjects()		{ return mObjects; }
	const Array<TypedCompoundPointer>&		GetObjects() const	{ return mObjects; }
private:
	Array<TypedCompoundPointer>				mObjects;
};



