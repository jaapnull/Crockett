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
		TypedPointer tp = gInspectObject(*inClass);
		mObjects.Add(tp);
	}

	template<class T>
	T* FindObject(String inName)
	{
		for (const TypedPointer& tp : mObjects)
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
	void SaveToStreams(StreamDevice& ioDevice);
private:
	Array<TypedPointer>			mObjects;
};



