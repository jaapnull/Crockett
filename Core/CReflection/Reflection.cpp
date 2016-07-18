#include <CorePCH.h>
#include <CReflection/Reflection.h>

ReflectionHost* ReflectionHost::sReflectionHost = nullptr;

const CompoundReflectionInfo* ReflectionHost::FindClassInfo(const ClassName& inClassName) const
{
	std::unordered_map<ClassName, CompoundReflectionInfo>::const_iterator i = mItems.find(inClassName);
	if (i == mItems.end()) return 0;
	return &(i->second);
}


const CompoundReflectionInfo* ReflectionHost::FindOrCreateClassInfo(const ClassName& inClassName)
{
	std::unordered_map<ClassName, CompoundReflectionInfo>::const_iterator i = mItems.find(inClassName);
	CompoundReflectionInfo temp_info;
	temp_info.mName = inClassName;
	if (i == mItems.end())
	{
		mItems.insert(ClassInfoEntry(inClassName, temp_info));
		i = mItems.find(inClassName);
		gAssert(i != mItems.end());
	}
	return &(i->second);
}


