#pragma once

#include <CCore/Array.h>
#include <CCore/Integers.h>

class BitField
{
public:
	void			Resize(uint inBitCount)							{ mBlocks.Resize(sBitCountToBlockCount(inBitCount)); mBlocks.SetAll(0); mBitCount = inBitCount; }
	void			Clear()											{ mBlocks.SetAll(0); }
	bool			GetBit(uint inBit) const						{ gAssert(inBit < mBitCount); return (mBlocks[inBit / 64] & (1ll << (inBit & 63))) != 0; }
	bool			operator[](uint inIndex) const					{ return GetBit(inIndex); }
	void			SetBit(uint inBit, bool inValue)				{ gAssert(inBit < mBitCount); if (inValue) mBlocks[inBit / 64] |= (1ll << (inBit & 63)); else mBlocks[inBit / 64] &= (~(1ll << (inBit & 63))); }
	void			Invert()										{ for (uint c = 0; c < sBitCountToBlockCount(mBitCount); c++) mBlocks[c] = ~mBlocks[c]; }
	uint			GetBitCount() const								{ return mBitCount; }
	uint			FindFirstOne(uint inStart) const				{ return FindFirstOneInRange(inStart, mBitCount); }
	uint			FindFirstZero(uint inStart) const				{ return FindFirstZeroInRange(inStart, mBitCount); }

	uint FindFirstOne(uint inStart, uint inStop) const
	{
		if (inStart == inStop)
			return cMaxUint;
		if (inStart < inStop)
		{
			// normal range
			return FindFirstOneInRange(inStart, inStop);
		}
		else
		{
			// wrapping range
			uint t = FindFirstOneInRange(inStart, mBitCount);
			if (t == cMaxUint)
				return FindFirstOneInRange(0, inStop);
			else
				return t;
		}
	}
	uint FindFirstZero(uint inStart, uint inStop) const
	{
		if (inStart == inStop)
			return cMaxUint;
		if (inStart < inStop)
		{
			// normal range
			return FindFirstZeroInRange(inStart, inStop);
		}
		else
		{
			// wrapping range
			uint t = FindFirstZeroInRange(inStart, mBitCount);
			if (t == cMaxUint)
				return FindFirstZeroInRange(0, inStop);
			else
				return t;
		}
	}



	uint FindFirstOneInRange(uint inStart, uint inStop) const
	{
		gAssert(inStart <= inStop);

		uint first_bit_offset = (inStart & 63);
		for (uint curr_block = (inStart / 64); curr_block < sBitCountToBlockCount(inStop); curr_block++)
		{
			uint64 b = mBlocks[curr_block];
			if ((b >> first_bit_offset) != 0)
			{
				// shift out any leading bits if inStart is not multiple of 64
				uint bit_set = gGetLowestBitSet(b >> first_bit_offset) + curr_block * 64 + first_bit_offset;
				// Extra check to clamp out any trailing bits in the last block
				if (bit_set < inStop)
					return bit_set;
			}
			first_bit_offset = 0;
		}
		return cMaxUint;
	}

	uint FindFirstZeroInRange(uint inStart, uint inStop) const
	{
		gAssert(inStart <= inStop);

		// use exactly the same function as FindFirstOne, but simply invert block before checking
		uint first_bit_offset = (inStart & 63);
		for (uint curr_block = (inStart / 64); curr_block < sBitCountToBlockCount(inStop); curr_block++)
		{
			uint64 b = ~(mBlocks[curr_block]); // here is the invert!
			if ((b >> first_bit_offset) != 0)
			{
				// shift out any leading bits if inStart is not multiple of 64
				uint bit_set = gGetLowestBitSet(b >> first_bit_offset) + curr_block * 64 + first_bit_offset;
				// Extra check to clamp out any trailing bits in the last block
				if (bit_set < inStop)
				{
					gAssert(GetBit(bit_set) == false);
					return bit_set;
				}
			}
			first_bit_offset = 0;
		}
		return cMaxUint;
	}



private:
	static uint		sBitCountToBlockCount(uint inBit)				{ return (inBit+63) / 64; }
	Array<uint64>	mBlocks;										/// lsb = lowest index, msb = highest index
	uint			mBitCount;										/// amount of available bits (not rounded to 64)
};



