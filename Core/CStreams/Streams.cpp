#pragma once

#include <CStreams/Streams.h>


DeviceRegistery gDevices;

void DeviceRegistery::RegisterDevice(StreamDevice* inDevice)
{
	gAssert(inDevice != nullptr);
	gAssert(mDeviceMap.find(inDevice->GetName()) == mDeviceMap.end());
	mDeviceMap.insert(DeviceEntry(inDevice->GetName(), inDevice));
}


StreamDevice* DeviceRegistery::FindDevice(const String& inName)
{
	gAssert(!inName.IsEmpty());
	DeviceMap::iterator i =  mDeviceMap.find(inName);
	
	if (i != mDeviceMap.end())
		return i->second;
	return nullptr;
}
