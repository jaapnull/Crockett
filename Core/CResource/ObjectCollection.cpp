#include <CResource/ObjectCollection.h>
#include <CReflection/ObjectReader.h>
#include <CStreams/Streams.h>

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

	ObjectReader object_reader;
	object_reader.SetDefaultDevice(*device);
	object_reader.ReadFile(*stream, mObjects, links);

	// first load in all files until no more files are referenced by external links
	for (const UnresolvedLink& link : links)
	{
		Path path = link.mTargetLocation;
		if (loaded_streams.Find(path) == cMaxSize64)
		{
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
		for (const TypedPointer& tp : mObjects)
		{
			String* l = TypedCompoundPointer(tp).GetCompoundMember<String>("!location");
			if (l != nullptr && link.mTargetLocation.GetLocation() == *l)
			{
				String* n = TypedCompoundPointer(tp).GetCompoundMember<String>("!name");
				if (n != nullptr && link.mTargetLocation.GetObjectName() == *n)
				{
					String nom = *TypedCompoundPointer(link.mLinkParentObject).GetCompoundMember<String>("!name");
					std::cout << "Resolved: " << nom << link.mLinkParentObject.ResolvePathToString(link.mReflectionPath) << "->" <<  link.mTargetLocation << std::endl;
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



void ObjectCollection::SaveToStreams(StreamDevice& ioDevice)
{


}
