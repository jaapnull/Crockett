#pragma once

// x64 only ? see:
// https://msdn.microsoft.com/en-us/library/hh977022.aspx
#include <intrin.h>
#include <windows.h>
#include <CCore/Types.h>

class AtomicInt64
{
public:

	AtomicInt64()								{ }
	AtomicInt64(int64 inValue) : mNugget(inValue) { }

	operator uint64()							{ return mNugget; }

	int64 Add(int64 inValue)					{ return _InterlockedAdd64(&mNugget, inValue) - inValue; }
	
	int64 operator++ ()		/* prefix  */		{ return _InterlockedIncrement64(&mNugget) + 1; }
	int64 operator++ (int)	/* postfix */		{ return _InterlockedIncrement64(&mNugget); }
	int64 operator-- ()		/* prefix  */		{ return _InterlockedDecrement64(&mNugget) - 1; }
	int64 operator-- (int)	/* postfix */		{ return _InterlockedDecrement64(&mNugget); }

	bool operator==(int64 inTestValue) const	{ return mNugget == inTestValue; }
	bool operator!=(int64 inTestValue) const	{ return mNugget != inTestValue; }
	void operator=(int64 inNewValue) 			{ _InterlockedExchange64(&mNugget, inNewValue); }

private:
	volatile int64 mNugget = 0;

};

