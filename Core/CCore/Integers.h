#pragma once
#include <CCore/Integers.h>
#include <CCore/Types.h>

/// lots of snippets from https://graphics.stanford.edu/~seander/bithacks.html

inline uint gGetHighestBitSet(uint64 inValue)
{
	const uint64 bitmasks[]		= { 0xFFFFFFFF00000000, 0xFFFF0000, 0xFF00, 0xF0, 0xC, 0x2 };
	const uint shifts[]			= { 32, 16, 8, 4, 2, 1 };
	uint highest_bit			= 0;
	for (uint i = 0; i < 6; i++)
	{
		if (inValue & bitmasks[i])
		{
			inValue >>= shifts[i];
			highest_bit += shifts[i];
		}
	}
	return highest_bit;
}

inline uint gGetLowestBitSet(uint64 inValue)
{
	const uint64 bitmasks[]		= { 0xFFFFFFFF, 0xFFFF, 0xFF, 0xF, 0x3, 0x1 };
	const uint shifts[]			= { 32, 16, 8, 4, 2, 1 };
	uint lowest_bit				= 0;
	for (uint i = 0; i < 6; i++)
	{
		if ((inValue & bitmasks[i]) == 0)
		{
			inValue >>= shifts[i];
			lowest_bit += shifts[i];
		}
	}
	return lowest_bit;
}

inline bool gIsPowerOf2(uint64 inValue)
{
	return ((inValue - 1) & inValue) == 0;
}

inline uint gCountBits(uint32 inValue)
{
	uint32 r = inValue;
	r = (r & 0x55555555) + ((r & 0xAAAAAAAA) >> 1);
	r = (r & 0x33333333) + ((r & 0xCCCCCCCC) >> 2);
	r = (r & 0x0F0F0F0F) + ((r & 0xF0F0F0F0) >> 4);
	r = (r & 0x00FF00FF) + ((r & 0xFF00FF00) >> 8);
	r = (r & 0x0000FFFF) + ((r & 0xFFFF0000) >> 16);
	return r;
}

inline uint gCountBits(uint64 inValue)
{
	uint64 r = inValue;
	r = (r & 0x5555555555555555) + ((r & 0xAAAAAAAAAAAAAAAA) >> 1);
	r = (r & 0x3333333333333333) + ((r & 0xCCCCCCCCCCCCCCCC) >> 2);
	r = (r & 0x0F0F0F0F0F0F0F0F) + ((r & 0xF0F0F0F0F0F0F0F0) >> 4);
	r = (r & 0x00FF00FF00FF00FF) + ((r & 0xFF00FF00FF00FF00) >> 8);
	r = (r & 0x0000FFFF0000FFFF) + ((r & 0xFFFF0000FFFF0000) >> 16);
	r = (r & 0x00000000FFFFFFFF) + ((r & 0xFFFFFFFF00000000) >> 32);
	return (uint)r;
}

inline uint64 gEndianSwap(uint64 inData) // *shame*
{
	uint64 v = ((inData & 0xff00000000000000ll) >> 56);
	v |= ((inData & 0x00ff000000000000ll) >> 40);
	v |= ((inData & 0x0000ff0000000000ll) >> 24);
	v |= ((inData & 0x000000ff00000000ll) >> 8);
	v |= ((inData & 0x00000000ff000000ll) << 8);
	v |= ((inData & 0x0000000000ff0000ll) << 24);
	v |= ((inData & 0x000000000000ff00ll) << 40);
	v |= ((inData & 0x00000000000000ffll) << 56);
	return v;
}

