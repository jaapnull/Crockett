#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>
#include <WCommon/Keyboard.h>


#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif


struct LayeredPixel
{
	LayeredPixel()
	{
		for (int p = 0; p < 16; p++) mWeight[p] = 0.0f;
	}


	float GetColorAmount() const
	{
		float color = 0.0f;
		for (int p = 0; p < 16; p++) color += mWeight[p];
		gAssert(color >= 0 && color <= 1.0f);
		return color;
	}

	float GetColorAmount(uint inExceptP) const
	{
		float color = 0.0f;
		for (int p = 0; p < 16; p++) if (p != inExceptP) color += mWeight[p];
		gAssert(color >= 0 && color <= 1.0f);
		return color;
	}

	float GetAlphaAmount() const
	{
		float alpha = 1.0f;
		for (int p = 0; p < 16; p++) alpha -= mWeight[p];
		gAssert(alpha >= 0);
		return alpha;
	}


	void AddWeight(uint inPalette, float inAdd)
	{
		float curr_weight = mWeight[inPalette];
		float new_weight = gClamp(inAdd + curr_weight, 0.0f, 1.0f);
		float room_to_make = new_weight - curr_weight;

		float weight_of_everyting_but = 1.0f - mWeight[inPalette];

		if (weight_of_everyting_but == 0.0f)
		{
			gAssert(room_to_make == 0.0f);
			return;
		}

		float multipliers = 1.0f - (room_to_make / weight_of_everyting_but);
		for (int p = 0; p < 16; p++) mWeight[p] *= multipliers;
		mWeight[inPalette] = new_weight;
	}

	void GenerateDither(uint inX, uint inY)
	{
		int bayer_idx[4][4] = 
		{
			{1,9,3,11},
			{13,5,15,7},
			{4,12,2,10},
			{16,8,14,6}
		};
		float value = float(bayer_idx[inX&3][inY&3]) / 17.0f;
		float accum = 0;
		for (int c = 0; c < 16; c++)
		{
					
			float _old = accum;
			accum += mWeight[c];
			float _new = accum;
					
			if (_old < value && _new >= value)
			{
				mCachedResult = DIBColor::sCreateDefaultPaletteColor(EDefaultPaletteColor(c));
				return;
			}
		}
		mCachedResult = DIBColor(255,255,255);
	}


	float		mWeight[16];
	DIBColor	mCachedResult;
};



DataFrame<LayeredPixel> gDocument;

class MyWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{
public:

	MyWindow() : Window(), mCanvas(*this), mMouseHandler(*this), mKeyboard(*this)
	{
		AddHandler(&mCanvas);
		AddHandler(&mMouseHandler);
		AddHandler(&mKeyboard);
	}


	virtual void OnSize(const ivec2& inNewSize)
	{
		Invalidate();
	}
	

	virtual void OnUpdate(DIB& inDib, const IRect& inRegion)
	{

		ColorPen<DIBColor> pen(inDib);

		inDib.SetAll(DIBColor::sCreateDefaultPaletteColor(dpcWhite));

		if (!mShowDither)
		{

			for (int y = inRegion.mTop; y < inRegion.mBottom; y++)
			{
				int dy = y / mPixelSize;
				if (dy < 0 || dy >= (int) gDocument.GetHeight()) break;
				for (int x = inRegion.mLeft; x < inRegion.mRight; x++)
				{
					int dx = x / mPixelSize;
					if (dx < 0 || dx >= (int) gDocument.GetWidth()) break;
					
					inDib.Set(x, y, gDocument.Get(dx,dy).mCachedResult);
				}
			}
		}
		else
		{
			for (uint x = 0; x < gDocument.GetWidth(); x++)
			for (uint y = 0; y < gDocument.GetHeight(); y++)
			{
				LayeredPixel lp = gDocument.Get(x,y);
				float accum = 0;
				for (int c = 0; c < 16; c++)
				{
					pen.SetColor(DIBColor::sCreateDefaultPaletteColor(EDefaultPaletteColor(c)));
					int _old = int(accum * float(mPixelSize-1));
					accum += lp.mWeight[c];
					int _new = int(accum * float(mPixelSize-1));
					pen.FillSquare(IRect(x*mPixelSize+1, y*mPixelSize+_old+1, x*mPixelSize+mPixelSize, y*mPixelSize+_new+1));
				}
			}
		}
	}

	void PaintAt(const ivec2& inPos, uint inSize)
	{
		int brush_size = inSize;

		ivec2 min = inPos - ivec2(brush_size,brush_size);
		min.x = gClamp<int>(min.x, 0, gDocument.GetWidth()-1);
		min.y = gClamp<int>(min.y, 0, gDocument.GetHeight()-1);
		ivec2 max = inPos + ivec2(brush_size,brush_size);
		max.x = gClamp<int>(max.x, 0, gDocument.GetWidth()-1);
		max.y = gClamp<int>(max.y, 0, gDocument.GetHeight()-1);

		for (int x = min.x; x <= max.x; x++)
		for (int y = min.y; y <= max.y; y++)
		{
			float dist_sq = 0.0f;
			dist_sq += gSquared(float(x - inPos.x));
			dist_sq += gSquared(float(y - inPos.y));

			float dist = sqrt(dist_sq);

			float weight = gClamp((float(brush_size) - dist) * gRecp(float(brush_size)), 0.0f, 1.0f);
			gDocument.Get(x,y).AddWeight(mColor, 0.05f * weight);
			gDocument.Get(x,y).GenerateDither(x,y);
		}

			Invalidate(
				IRect(	(inPos.x-mBrushSize) * mPixelSize,				(inPos.y-mBrushSize) * mPixelSize,
						(inPos.x+mBrushSize) * mPixelSize + mPixelSize,	(inPos.y+mBrushSize) * mPixelSize + mPixelSize));


	}


	virtual void OnKeyDown(const uint inKeyCode) 
	{
		if (inKeyCode >= '0' && inKeyCode <= '9') mColor = inKeyCode - '0';
		if (inKeyCode == ' ')
		{
			mShowDither = !mShowDither;
			Invalidate();
		}

		if (inKeyCode == 'A')
		{
			mPixelSize--;
			if (mPixelSize <= 0) mPixelSize = 1;
			Invalidate();
		}

		if (inKeyCode == 'Z')
		{
			mPixelSize++;
			Invalidate();
		}

		if (inKeyCode == 'S')
		{
			mBrushSize--;
		}

		if (inKeyCode == 'X')
		{
			mBrushSize++;
		}

	}



	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons)
	{
		if (inButtons.Contains(mbLeft))
		{
			ivec2 pos = inPosition / mPixelSize;
			PaintAt(pos, mBrushSize);
		}
	}


	virtual void OnMouseLeftUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons)
	{

	}


	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons)
	{
		ivec2 pos = inPosition / mPixelSize;
		PaintAt(pos, mBrushSize);
		Invalidate();
	}

	void DoTick()
	{
	
	}

private:
	bool						mShowDither			= false;
	uint						mColor				= 4;
	uint						mPixelSize			= 1;
	uint						mBrushSize			= 10;
	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;
} mainwindow;





int main()
{
	gDocument.Resize(640,640);
//	Document doc;
//	DIB* bg = new DIB();
//	bg->LoadFromFile(L"./test.bmp");
//	doc.mLayers.Append(bg);

	for (uint y = 0; y < gDocument.GetHeight(); y++)
	for (uint x = 0; x < gDocument.GetWidth(); x++)
	{
		gDocument.Get(x,y).AddWeight(1, float(x) / float(gDocument.GetWidth()));
		gDocument.Get(x,y).GenerateDither(x,y);
	}

	mainwindow.Create(L"ComeDither", 640, 480);
	mainwindow.Show(true);


	while (gDoMessageLoop(true)) 
	{
		
		mainwindow.DoTick();
	}

	std::cout << "test";
	return 0;
}

