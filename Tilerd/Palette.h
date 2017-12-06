#pragma once
#include <CMath/Vector.h>
#include <CCore/String.h>
#include <CMath/Math.h>
#include <WCommon/DIB.h>

class Palette
{
public:
	static Palette* sCreateFromFile(const WString& inFile, ivec2 inTileDimensionInPixels);

	IRect		GetValidTileRange() const;
	ivec2		GetTileDimensions() const	{ return mTileDimensionInPixels; }
	const		DIB& GetDIB() const			{ return mImage; }
	bool		IsValid() const				{ return mImage.IsValid() && mTileDimensionInPixels != ivec2::sZero(); }
	ivec2&		Test() const				{ return ivec2(42, 42); }
	WString		GetName() const				{ return mName; }
	IRect		GetTileRect(const ivec2& inIndex) const 
	{
		ivec2 min = mOffsetInPixels + inIndex.GetMultiply(mTileDimensionInPixels);
		ivec2 max = min + mTileDimensionInPixels;
		return IRect(min, max);
	}

private:
	WString		mName;
	ivec2		mOffsetInPixels				= ivec2::sZero();
	ivec2		mTileDimensionInPixels		= ivec2::sZero();
	DIB			mImage;
};