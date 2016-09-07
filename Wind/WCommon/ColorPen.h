// ------------------------------------------------------------------------------------------------------------
// Simple 2D pixel drawing utilities wrapped in a pen class
// ------------------------------------------------------------------------------------------------------------
#pragma once
#include <PCHWind.h>
#include <CCore/Types.h>
#include <CCore/Array.h>
#include <CCore/String.h>
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
		DrawLine(inRect.mLeft, inRect.mTop, inRect.mRight-1, inRect.mTop);
		DrawLine(inRect.mRight-1, inRect.mTop, inRect.mRight-1, inRect.mBottom-1);
		DrawLine(inRect.mRight-1, inRect.mBottom-1, inRect.mLeft, inRect.mBottom-1);
		DrawLine(inRect.mLeft, inRect.mBottom-1, inRect.mLeft, inRect.mTop);
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
				if (steep) mCanvas.Set(y,x, mColor); else mCanvas.Set(x,y, mColor);
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
				if (steep) mCanvas.Set(y,x, mColor); else mCanvas.Set(x,y, mColor);
				error -= deltay;
				if (error < 0)
				{
					y += ystep;
					error += deltax;
				}
			}

	}
	
	inline void DrawLine(ivec2 inStart, ivec2 inEnd) { DrawLine(inStart.x, inStart.y, inEnd.x, inEnd.y); }


	private:
	BaseFrame<Color_T>	mCanvas;
	Color_T				mColor;
	uint32				mPattern;
	int					mSize;
	PenShape			mShape;
	ivec2				mOffset;
};



