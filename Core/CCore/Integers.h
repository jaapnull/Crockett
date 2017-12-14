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

