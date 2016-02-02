// ------------------------------------------------------------------------------------------------------------
// Enum mask is a wrapper around enums to enable easy masking operations such as AND/OR, bit checking etc.
// ------------------------------------------------------------------------------------------------------------
#pragma once

template <class T_Enum>
class EnumMask
{
	uint32																mMask;
public:

	void operator<<(const T_Enum& inMask) 								{ mMask |= (uint32)inMask; }
	void operator>>(const T_Enum& inMask)								{ mMask &= ~((uint32)inMask); }

	EnumMask<T_Enum>(const EnumMask<T_Enum>& other)						{ mMask = other.mMask; }
	EnumMask<T_Enum>(const T_Enum& other)								{ mMask = (uint32)other; }
	EnumMask<T_Enum>(uint32 bitmask)									{ mMask = bitmask; }

	const EnumMask<T_Enum>& operator=(const EnumMask<T_Enum>& other)	{ mMask = other.mMask; return *this; }
	const EnumMask<T_Enum>& operator=(const T_Enum& other)				{ mMask = (uint32)other; return *this; }

	bool operator==(const EnumMask<T_Enum>& other)				const	{ return other.mMask == mMask; }

	void Inverse()														{ mMask = ~mMask; }
	void Clear()														{ mMask = 0; }
	uint32 GetValue()													{ return mMask; }
	bool Contains(const T_Enum& inMask)							const	{ return (mMask & ((uint32)inMask)) != 0; }
	bool operator[] (const T_Enum& inMask)						const	{ return contains(inMask); }

	EnumMask(void) : mMask(0)											{}
	virtual ~EnumMask(void)												{}
};
