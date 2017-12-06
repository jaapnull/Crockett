#include "Palette.h"


Palette* Palette::sCreateFromFile(const WString& inFile, ivec2 inTileDimensionInPixels)
{
	Palette* new_palette = new Palette();
	new_palette->mImage.LoadFromFile(inFile);
	gAssert(new_palette->mImage.IsValid());
	new_palette->mTileDimensionInPixels = inTileDimensionInPixels;
	new_palette->mOffsetInPixels = ivec2::sZero();
	new_palette->mName = inFile;
	return new_palette;
}

IRect Palette::GetValidTileRange() const
{
	return !mImage.IsValid() ? IRect::sEmpty() : IRect(0, 0,
		(mImage.GetWidth() - mOffsetInPixels.x) / mTileDimensionInPixels.x,
		(mImage.GetHeight() - mOffsetInPixels.y) / mTileDimensionInPixels.y);
}
