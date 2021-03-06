// ------------------------------------------------------------------------------------------------------------
// Simple 2D pixel drawing utilities wrapped in a pen class
// ------------------------------------------------------------------------------------------------------------
#pragma once
#include <PCHWind.h>
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CGeo/LineSegment2.h>
// ------------------------------------------------------------------------------------------------------------
// Brush shapes of the pen
// ------------------------------------------------------------------------------------------------------------
enum PenShape{psCircle, psSquare, psCross};


// ------------------------------------------------------------------------------------------------------------
// Pen utility class that can be used to easily draw simple things like lines, circles on 2D pixel canvases
// ------------------------------------------------------------------------------------------------------------
template<class Color_T> class ColorPen
{
public:
	void		SetShape(PenShape inShape)				{ mShape = inShape; }
	PenShape	GetShape()						const	{ return mShape; }
	void		SetPattern(uint32 inPattern)			{ mPattern = inPattern; }
	uint32		GetPattern()					const	{ return mPattern; }
	void		SetSize(int inSize)						{ mSize = inSize; }
	int			GetSize()						const	{ return mSize; }
	void		SetColor(const Color_T& inColor)		{ mColor = inColor; }
	Color_T		GetColor()						const	{ return mColor; }

	ColorPen(BaseFrame<Color_T>& inCanvas) : mSize(0), mShape(psCircle), mPattern(0xFFFFFFFF), mCanvas(inCanvas), mColor(0,0,0) {}

	inline void SetPixel( int x0, int y0)
	{
		mCanvas.Set(x0, y0, mColor);
	}

	bool RotatePattern(unsigned int steps)
	{
		for (unsigned int x = 0; x < steps; x++)
		{
			if (mPattern & 1)
			{
				mPattern >>= 1;
				mPattern |= 0x80000000;
			}
			else
			{
				mPattern >>= 1;
			}		
		}
		return (mPattern&1) != 0;
	}

	void DrawDot(int x, int y) 
	{
		if (mPattern & 1)
		{
			for (int cx = -mSize; cx <= mSize; cx++)
			{
				for (int cy = -mSize; cy <= mSize; cy++)
				{
					switch (mShape)
					{
					case psCross:
						if (cx == 0 || cy == 0)
							SetPixel(x + cx, y + cy); break;
					case psCircle:
						if (cx*cx + cy*cy <= mSize*mSize)
							SetPixel(x + cx, y + cy); break;
					case psSquare:
					default:
						SetPixel(x + cx, y + cy); break;						
					}
				}
			}
		}
		RotatePattern(1);
	}


	void FillSquare(const IRect& inRect)
	{
		mCanvas.SetRegion(inRect, mColor);
	}

	void DrawSquare(const IRect& inRect)
	{
		IRect clamped = IRect(
			gClamp<int>(inRect.mLeft, 0, mCanvas.GetWidth()),
			gClamp<int>(inRect.mTop, 0, mCanvas.GetHeight()),
			gClamp<int>(inRect.mRight, 0, mCanvas.GetWidth()),
			gClamp<int>(inRect.mBottom, 0, mCanvas.GetHeight()));

		if (clamped.GetWidth() != 0)
		{
			if ((uint) inRect.mTop >= 0 && (uint) inRect.mTop < mCanvas.GetHeight())
				DrawLine(clamped.mLeft,    clamped.mTop,      clamped.mRight-1, clamped.mTop);
			if ((uint) inRect.mBottom >= 0 && (uint) inRect.mBottom < mCanvas.GetHeight())
				DrawLine(clamped.mRight-1, clamped.mBottom-1, clamped.mLeft,    clamped.mBottom-1);
		}

		if (clamped.GetHeight() != 0)
		{
			if ((uint) inRect.mRight >= 0 && (uint) inRect.mRight < mCanvas.GetWidth())
				DrawLine(clamped.mRight-1, clamped.mTop,      clamped.mRight-1, clamped.mBottom-1);
			if ((uint) inRect.mLeft >= 0 && (uint) inRect.mLeft < mCanvas.GetWidth())
				DrawLine(clamped.mLeft,    clamped.mBottom-1, clamped.mLeft,    clamped.mTop);
		}
	}
	
	void DrawLine(int x0, int y0, int x1, int y1)
	{
		bool steep = abs(y1 - y0) > abs(x1 - x0);
		if (steep)
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
		}
		int deltax = abs(x1 - x0);
		int deltay = abs(y1 - y0);
		int error = deltax / 2;
		int ystep = (y0 < y1) ? 1 : -1;
		int xstep = (x0 < x1) ? 1 : -1;
		int y = y0;
		if (xstep > 0)
			for (int x = x0; x <= x1; x+= xstep)
			{		
				if (steep)
					DrawDot(y, x);// mCanvas.Set(y, x, mColor);
				else
					DrawDot(x, y);// mCanvas.Set(x, y, mColor);
				error -= deltay;
				if (error < 0)
				{
					y += ystep;
					error += deltax;
				}
			}
		else
			for (int x = x0; x >= x1; x+= xstep)
			{		
				if (steep) 
					DrawDot(y, x);// mCanvas.Set(y, x, mColor);
				else
					DrawDot(x, y);// mCanvas.Set(x, y, mColor);

				error -= deltay;
				if (error < 0)
				{
					y += ystep;
					error += deltax;
				}
			}

	}
	
	inline void DrawLine(ivec2 inStart, ivec2 inEnd)
	{
		// clip against edge half-spaces of canvas
		LineSegment2 ls(inStart, inEnd);
		if (!ls.Clip(HalfSpace2(fvec2(1.0f, 0.0f), 5.0f)))
			return;
		if (!ls.Clip(HalfSpace2(fvec2(0.0f, 1.0f), 5.0f)))
			return;
		if (!ls.Clip(HalfSpace2(fvec2(-1.0f, 0.0f), -float(mCanvas.GetWidth()-5))))
			return;
		if (!ls.Clip(HalfSpace2(fvec2(0.0f, -1.0f), -float(mCanvas.GetHeight()-5))))
			return;

		DrawLine(gRoundToInt(ls.mFrom.x), gRoundToInt(ls.mFrom.y), gRoundToInt(ls.mTo.x), gRoundToInt(ls.mTo.y));
	}


	private:
	BaseFrame<Color_T>	mCanvas;
	Color_T				mColor;
	uint32				mPattern;
	int					mSize;
	PenShape			mShape;
	ivec2				mOffset;
};



