#include <CCore/String.h>
#include <CMath/Math.h>
#include <CMath/Vector.h>
#include <CUtils/DebugFont.h>
#include <CGeo/HalfSpace2.h>
#include <CGeo/LineSegment2.h>
#include <CGeo/Polygon2.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CUtils/Sort.h>

#include <unordered_map>

#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>
#include <WCommon/Keyboard.h>


#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif



struct Palette
{
	WString				mPaletteBitmap;
};


struct TileImage
{
	Palette				mPalette;
	ivec2				mTileDimensions;
	DataFrame<ivec2>	mMap;
} gCurrentImage;

static void sSetBg(DIB& inDib)
{
	//DIBColor* dc = inDib.GetBaseData();


	for (uint y = 0; y < inDib.GetHeight(); y++)
	{
		for (uint x = 0; x < inDib.GetWidth(); x++)
		{
			uint t = ((x + y) & 31);
			inDib.Set(x,y,DIBColor(128 + t, 128 + t, 128 + t));
		}
		//gOffsetPointer(dc, inDib.GetPitch());
	}
}

class PaletteWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{
public:
	PaletteWindow() : Window(), mCanvas(*this), mMouseHandler(*this), mKeyboard(*this)
	{
		AddHandler(&mCanvas);
		mCanvas.SetDefaultColor(DIBColor(255, 255, 255));
		AddHandler(&mMouseHandler);
		AddHandler(&mKeyboard);
	}

	virtual void OnSize(const ivec2& inNewSize) override
	{
		if (mDrawnScale != GetScale())
			Invalidate();
	}

	virtual void OnMouseLeave() override
	{
		mMouseOverIdx = mSelectedIdx;
	}

	IRect GetDrawRectFromIndex(ivec2 inIndex)
	{
		ivec2 tile_dim = gCurrentImage.mTileDimensions;
		return IRect(	inIndex.x * tile_dim.x * mDrawnScale,
						inIndex.y * tile_dim.y* mDrawnScale,
						(inIndex.x * tile_dim.x + tile_dim.x) * mDrawnScale,
						(inIndex.y * tile_dim.y + tile_dim.y) * mDrawnScale);
	}

	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		if (mDrawnScale == 0)
			return; 

		ivec2 prev_idx = mMouseOverIdx; 
		mMouseOverIdx.x = inPosition.x / (gCurrentImage.mTileDimensions.x * mDrawnScale);
		mMouseOverIdx.y = inPosition.y / (gCurrentImage.mTileDimensions.y * mDrawnScale);
		Invalidate(GetDrawRectFromIndex(prev_idx));
		Invalidate(GetDrawRectFromIndex(mMouseOverIdx));
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		if (mDrawnScale == 0)
			return;

		ivec2 prev_idx = mSelectedIdx;
		mSelectedIdx.x = inPosition.x / (gCurrentImage.mTileDimensions.x * mDrawnScale);
		mSelectedIdx.y = inPosition.y / (gCurrentImage.mTileDimensions.y * mDrawnScale);
		Invalidate(GetDrawRectFromIndex(prev_idx));
		Invalidate(GetDrawRectFromIndex(mSelectedIdx));
	}

	virtual void OnUpdate(DIB& inDib, const IRect& inRegion)
	{
		mDrawnScale = GetScale();
		sSetBg(inDib);

		IRect r = inRegion;


		r.mLeft = gMin<uint>((inRegion.mLeft / mDrawnScale) * mDrawnScale, mLoadedPaletteBitmap.GetWidth() * mDrawnScale);
		r.mTop = gMin<uint>((inRegion.mTop / mDrawnScale) * mDrawnScale, mLoadedPaletteBitmap.GetHeight() * mDrawnScale);

		r.mRight = gMin<uint>((inRegion.mRight / mDrawnScale) * mDrawnScale, mLoadedPaletteBitmap.GetWidth() * mDrawnScale);
		r.mBottom = gMin<uint>((inRegion.mBottom / mDrawnScale) * mDrawnScale, mLoadedPaletteBitmap.GetHeight() * mDrawnScale);

		gAssert(r.mLeft % mDrawnScale == 0);
		gAssert(r.mRight % mDrawnScale == 0);
		gAssert(r.mTop % mDrawnScale == 0);
		gAssert(r.mBottom % mDrawnScale == 0);
		inDib.DrawImageStretched(r.mLeft, r.mTop, r.GetWidth(), r.GetHeight(), mLoadedPaletteBitmap, r.mLeft / mDrawnScale, r.mTop / mDrawnScale, r.GetWidth()/mDrawnScale, r.GetHeight() / mDrawnScale);
		
		ColorPen<DIBColor> pen(inDib);
		
		IRect selec = GetDrawRectFromIndex(mMouseOverIdx);
		pen.SetColor(DIBColor(0, 0, 0));
		pen.DrawSquare(selec.GetGrown(-1));
		pen.SetColor(DIBColor(255, 255, 255));
		pen.DrawSquare(selec.GetGrown(-2));

		selec = GetDrawRectFromIndex(mSelectedIdx);
		pen.SetColor(DIBColor(255, 0, 0));
		pen.DrawSquare(selec.GetGrown(-1));

	}

	void SetPalette(const Palette& inPalette)
	{
		mCurrentPalette = inPalette;
		mLoadedPaletteBitmap.LoadFromFile(inPalette.mPaletteBitmap);
		Resize(mLoadedPaletteBitmap.GetWidth(), mLoadedPaletteBitmap.GetHeight());
		mCanvas.GetDib() = mLoadedPaletteBitmap;
		
	}

	uint GetScale()
	{
		return gMax<uint>(1, gMin<uint>(GetWidth() / mLoadedPaletteBitmap.GetWidth(), GetHeight() / mLoadedPaletteBitmap.GetHeight()));
	}

private:
	uint						mDrawnScale = 0;
	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;
	DIB							mLoadedPaletteBitmap;
	Palette						mCurrentPalette;
	ivec2						mMouseOverIdx;
	ivec2						mSelectedIdx;
};


class TilerdWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{

public:

	TilerdWindow() : Window(), mCanvas(*this), mMouseHandler(*this), mKeyboard(*this)
	{
		AddHandler(&mCanvas);
		mCanvas.SetDefaultColor(DIBColor(255, 255, 255));
		AddHandler(&mMouseHandler);
		AddHandler(&mKeyboard);
	}

	virtual void OnSize(const ivec2& inNewSize) override
	{

	}

	virtual void OnMouseLeftUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
	}

	virtual void OnMouseLeave() override
	{
	}

	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
	}

	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
	}

private:

	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;
	ivec2						mTileSize	= ivec2(0,0);

} gMainWindow;

int main()
{
	gCurrentImage.mPalette.mPaletteBitmap = L".\\palette.bmp";
	gCurrentImage.mTileDimensions = ivec2(16, 16);
	gCurrentImage.mMap.Resize(32, 32);

	PaletteWindow w;
	w.Create(L"Palette", 100, 100);
	w.SetPalette(gCurrentImage.mPalette);
	w.Show(true);

	//gMainWindow.Create(L"Tilerd", 1024, 1024);
	//gMainWindow.Show(true);
	//
	while (gDoMessageLoop(true))
	{
	}
	return 0;
}

