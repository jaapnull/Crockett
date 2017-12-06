#pragma once

#include <CMath/Vector.h>
#include <CCore/Frame.h>
#include <CCore/Array.h>

#include "Palette.h"

struct HexCoordBase
{
	int		mHexWidth	= 30;
	int		mRowHeight	= 15;
	int		mRowShift	= 15;
};

struct TileEntry
{
	TileEntry() {}
	TileEntry(Palette* inP, const ivec2& inIndex) : mPalette(inP), mIndex(inIndex) {}
	
	Palette*	mPalette	= nullptr;
	ivec2		mIndex		= ivec2(0,0);
};

class Document
{
public:
	const HexCoordBase&		GetCoordBase() const					{ return mCoordBase; }
	void					Resize(uint inWidth, uint inHeight)		{ mTileMap.Resize(inWidth, inHeight); }
	const DataFrame<TileEntry>&	GetMap() const						{ return mTileMap; }
	DataFrame<TileEntry>&	GetMap()								{ return mTileMap; }

private:
	HexCoordBase			mCoordBase;
	ivec2					mTileDimensions;
	DataFrame<TileEntry>	mTileMap;
};


extern Array<Palette*>	gPalettes;
extern Document			gCurrentDocument;

