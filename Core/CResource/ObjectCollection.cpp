#include <CResource/ObjectCollection.h>
#include <CReflection/ObjectReader.h>
#include <CReflection/ObjectWriter.h>
#include <CStreams/Streams.h>

#include <unordered_map>

void ObjectCollection::LoadFromStream(const Path& inPath)
{
	gAssert(inPath.IsFileLocation());
	gAssert(inPath.GetObjectName().IsEmpty());

	StreamDevice* device = gDevices.FindDevice(inPath.GetDeviceName());
	gAssert(device != nullptr);
	Stream* stream = device->CreateStream(inPath, smRead);
	gAssert(stream != nullptr && stream->IsValid());

	Array<Path> loaded_streams;
	Array<UnresolvedLink> links;
	loaded_streams.Append(inPath);

	std::cout << "Reading file : " << inPath << std::endl;
	ObjectReader object_reader;
	object_reader.SetDefaultDevice(*device);
	object_reader.ReadFile(*stream, mObjects, links);

	// first load in all files until no more files are referenced by external links
	for (uint i = 0; i < links.GetLength(); i++)
	{
		const UnresolvedLink& link = links[i];
		Path path = link.mTargetLocation;
		if (loaded_streams.Find(path) == cMaxSize64)
		{
			std::cout << "Reading Dependency file : " << path << std::endl;
			StreamDevice* dev = gDevices.FindDevice(path.GetDeviceName());
			Stream*str = dev->CreateStream(path.GetLocation(), smRead);
			gAssert(str != nullptr && str->IsValid());
			object_reader.ReadFile(*str, mObjects, links);
			loaded_streams.Append(path);
			device->CloseStream(str);
		}
	}

	for (UnresolvedLink& link : links)
	{
		bool resolved = false;
		for (const TypedCompoundPointer& tp : mObjects)
		{
			String* l = TypedCompoundPointer(tp).GetCompoundMember<String>("!location");
			if (l != nullptr && link.mTargetLocation == *l)
			{
				String* n = TypedCompoundPointer(tp).GetCompoundMember<String>("!name");
				if (n != nullptr && link.mTargetName == *n)
				{
					String nom = *TypedCompoundPointer(link.mLinkParentObject).GetCompoundMember<String>("!name");
					std::cout << "Resolved: " << nom << link.mLinkParentObject.ResolvePathToString(link.mReflectionPath) << "->" <<  link.mTargetLocation << ":" << link.mTargetName << std::endl;
					TypedPointerPointer link_pointer(link.mLinkParentObject.GetObjectAtPath(link.mReflectionPath));
					*((void**)link_pointer.mPointer) = tp.mPointer;
					resolved = true; 
					break;
				}
			}
		}
		gAssert(resolved == true);
	}
	
}



void ObjectCollection::SaveToStreams()
{
	typedef std::pair<String, Array<TypedCompoundPointer>>					FileEntry;
	typedef std::unordered_map<String, Array<TypedCompoundPointer>>			FileMap;
	FileMap map;

	for (TypedCompoundPointer& p : mObjects)
	{
		String* loc = p.GetCompoundMember<String>("!location");
		gAssert(loc != nullptr);
		FileMap::iterator i = map.find(*loc);
		if (i == map.end())
			map.insert(FileEntry(*loc, Array<TypedCompoundPointer>(&p, 1)));
		else
			i->second.Append(p);
	}

	for (const FileEntry& entry : map)
	{
		Path target_path(entry.first);
		StreamDevice* d = gDevices.FindDevice(target_path.GetDeviceName());
		gAssert(d != nullptr);
		Stream* s = d->CreateStream(target_path, smWrite);
		gAssert(s != nullptr);
		ObjectWriter wr(*s);
		std::cout << "Opened stream to " << target_path << std::endl;
		for (const TypedCompoundPointer& object : entry.second)
		{
			const String* name = object.GetCompoundMember<String>("!name");
			const String* loc = object.GetCompoundMember<String>("!location");
			std::cout << "   Writing: " << (loc != nullptr ? *loc : String("<noloc>")) << " : " << (name != nullptr ? *name : String("<noname>")) << std::endl;
			wr.WriteTypedCompoundPointer(object);
		}
		d->CloseStream(s);
	}
}
