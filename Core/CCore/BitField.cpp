#include "BitField.h"
#include <CCore/Integers.h>

uint BitField::FindFirstOne(uint inStart) const
{
	uint curr_block = (inStart / 64);
	uint bit_start = (inStart & 63);
	
	// start block has additional logic to clamp out lower bits up to inStart by filling them with zeros
	if (bit_start != 0)
	{
		uint64 mask = ((1ll << (bit_start)) - 1);
		if (mBlocks[curr_block] & ~mask)
		{
			uint bit_set = gGetLowestBitSet(mBlocks[curr_block] & ~mask) + curr_block * 64;
			if (bit_set < mBitCount) // clamp out any trailing bits in the last block
				return bit_set;
		}
		curr_block++;
	}

	for (; curr_block < sBitCountToBlockCount(mBitCount); curr_block++)
	{
		uint64 b = mBlocks[curr_block];
		if (b == 0)
			continue;
		else
		{
			uint bit_set = gGetLowestBitSet(b) + curr_block * 64;
			if (bit_set < mBitCount) // clamp out any trailing bits in the last block
				return bit_set;
		}
	}
	return cMaxUint;
}


uint BitField::FindFirstZero(uint inStart) const
{
	// findfirst zero logic copies find_first_one logic, but inverts values before gGetLowestBitSet
	uint curr_block = (inStart / 64);
	uint bit_start = (inStart & 63);

	// start block has additional logic to clamp out lower bits up to inStart by filling them with ones
	if (bit_start != 0)
	{
		uint64 mask = ((1ll << (bit_start)) - 1);
		if (((mBlocks[curr_block] | mask) != cMaxSize64))
		{
			uint bit_set = gGetLowestBitSet(~(mBlocks[curr_block] | mask)) + curr_block * 64;
			if (bit_set < mBitCount) // clamp out any trailing bits in the last block
				return bit_set;
		}
		curr_block++;
	}

	for (; curr_block < sBitCountToBlockCount(mBitCount); curr_block++)
	{
		uint64 b = mBlocks[curr_block];
		if (b == cMaxSize64)
			continue;
		else
		{
			uint bit_set = gGetLowestBitSet(~b) + curr_block * 64;
			if (bit_set < mBitCount) // clamp out any trailing bits in the last block
				return bit_set;
		}
	}
	return cMaxUint;
}
