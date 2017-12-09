#pragma once

#include <CCore/Array.h>

class BitField
{
public:
	void			Resize(uint inBitCount)							{ mBlocks.Resize(sBitCountToBlockCount(inBitCount)); mBlocks.SetAll(0); mBitCount = inBitCount; }
	void			Clear()											{ mBlocks.SetAll(0); }
	bool			GetBit(uint inBit) const						{ gAssert(inBit < mBitCount); return (mBlocks[inBit / 64] & (1ll << (inBit & 63))) != 0; }
	void			SetBit(uint inBit)								{ gAssert(inBit < mBitCount); mBlocks[inBit / 64] |= (1ll << (inBit & 63)); }
	void			ResetBit(uint inBit)							{ gAssert(inBit < mBitCount); mBlocks[inBit / 64] &= (~(1ll << (inBit & 63))); }
	void			Invert()										{ for (uint c = 0; c < sBitCountToBlockCount(mBitCount); c++) mBlocks[c] = ~mBlocks[c]; }
	uint			GetBitCount() const								{ return mBitCount; }
	uint			FindFirstOne(uint inStart) const;
	uint			FindFirstZero(uint inStart) const;

private:
	static uint		sBitCountToBlockCount(uint inBit)				{ return (inBit+63) / 64; }
	Array<uint64>	mBlocks;										/// lsb = lowest index, msb = highest index
	uint			mBitCount;										/// amount of available bits (not rounded to 64)
};