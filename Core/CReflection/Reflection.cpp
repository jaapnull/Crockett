#include <CorePCH.h>
#include <CReflection/Reflection.h>

ReflectionHost* ReflectionHost::sReflectionHost = nullptr;

const CompoundReflectionInfo* ReflectionHost::FindClassInfo(const ClassName& inClassName) const
{
	std::unordered_map<ClassName, CompoundReflectionInfo>::const_iterator i = mItems.find(inClassName);
	if (i == mItems.end()) return 0;
	return &(i->second);
}

