#include <CCore/String.h>
#include <CMath/Vector.h>
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




struct LineSegment2
{
	fvec2 mOrigin;
	fvec2 mDirection;
};

struct HalfSpace2
{
	HalfSpace2(float inNormX, float inNormY, float inOffset) : mNormal(inNormX, inNormY), mOffset(inOffset) {}
	HalfSpace2(const fvec2& inNormal, float inOffset) : mNormal(inNormal), mOffset(inOffset) {}

	inline float SignedDistance(const fvec2& inPoint) const
	{
		return inPoint.GetDot(mNormal) - mOffset;
	}


	//bool GetIntersectInterpolant(const fvec2 inOrigin, const fvec2 inDirection, float& outInterpolant)
	//{
	//	// p = inOrigin + t * D
	//	// p dot mNormal = -mOffset
	//	
	//	// (O + t * D) dot N = -offset
	//	 
	//}


	bool GetIntersect(const HalfSpace2& inOther, fvec2& outIntersect) const
	{
		// Line description N.x * x + N.y * y + offset = 0
		// Can be seen as homogenous coordinates of NxNyO dot XY1 = 0
		// So solution can be seen as L0 dot XY1 = 0 && L1 dot XY1 = 0
		// XYW = L0 X L1
		// XY1 = XYW / W

		// Cross product of (normal,offset)X(normal,offset)=(U,V,W)
		// Projected to W = 1 (div by W)

		float w = (mNormal.y*inOther.mNormal.x) - (mNormal.x*inOther.mNormal.y);
		if (w == 0) return false;
		
		outIntersect.x = (inOther.mOffset * mNormal.y) - (mOffset * inOther.mNormal.y);
		outIntersect.y = (mOffset * inOther.mNormal.x) - (inOther.mOffset * mNormal.x);
		outIntersect.x /= w;
		outIntersect.y /= w;
		return true;
	}

	fvec2 mNormal;
	float mOffset;
};


class MyWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{

public:

	MyWindow() : Window(), mCanvas(*this), mMouseHandler(*this), mKeyboard(*this)
	{
		AddHandler(&mCanvas);
		mCanvas.SetDefaultColor(DIBColor(255,255,255));
		AddHandler(&mMouseHandler);
		AddHandler(&mKeyboard);

		mHalfSpaces.Append(HalfSpace2(0,1,1));
		mHalfSpaces.Append(HalfSpace2(0,-1,1));
		mHalfSpaces.Append(HalfSpace2(1,0,1));
		mHalfSpaces.Append(HalfSpace2(-1,0,1));
		mHalfSpaces.Append(HalfSpace2(0.707f,-0.707f,0.0f));
		mHalfSpaces.Append(HalfSpace2(0.707f,0.707f,0.0f));
	}

	virtual void OnSize(const ivec2& inNewSize) override
	{
		mHalfSpaceData.Resize(inNewSize.x, inNewSize.y);
		FillHalfSpaceData();
		Redraw();
	}


	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		String mask_string;
		uint32 mask = mHalfSpaceData.Get(inPosition.x, inPosition.y);
		for (int b = 0; b < mHalfSpaces.GetLength(); b++)
		{
			mask_string.Append((mask&(1<<b)) ? '1' : '0');
		}
		std::cout << std::hex << mask_string << std::endl;
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		if (mPointsSet == 0)
		{
			mP0 = pos;
			fvec2 s0 = pos;
			fvec2 s1(-1,-1);
			fvec2 s = s1-s0;
			fvec2 n(-s.y, s.x);
			fvec2 nn = n.GetNormalised();
			mHalfSpaces.Append(HalfSpace2(nn, nn.GetDot(s0)));
		}

		if (mPointsSet == 1)
		{
			mP1 = pos;
		}

		if (mPointsSet == 2)
		{
			mP2 = pos;
		}


		mPointsSet++;
		//mHalfSpaces.Append(HalfSpace2(pos.GetNormalised(), pos.GetLength()));
		FillHalfSpaceData();
		Redraw();
	}


	void FillHalfSpaceData()
	{
		gAssert(mHalfSpaceData.GetWidth() == mCanvas.GetDib().GetWidth());
		gAssert(mHalfSpaceData.GetHeight() == mCanvas.GetDib().GetHeight());
		
		if (!mHalfSpaces.IsEmpty())
		{
			fvec2 corner(-mVisibleRange, -mVisibleRange);
			float stepx = float(mVisibleRange*2.0f)/float(mHalfSpaceData.GetWidth());
			float stepy = float(mVisibleRange*2.0f)/float(mHalfSpaceData.GetHeight());
			for (uint x = 0; x < mHalfSpaceData.GetWidth(); x++)
			for (uint y = 0; y < mHalfSpaceData.GetHeight(); y++)
			{
				int32 mask = 0;
				for (const HalfSpace2& hs : mHalfSpaces)
				{
					fvec2 pos = corner + fvec2(float(x) * stepx, float(y) * stepy);
					mask <<= 1;
					float d = hs.SignedDistance(pos);
					if (d > 0)
						mask |= 0x1;
				}
				mHalfSpaceData.Set(x,y, mask);
			}
		}
		else
		{
			mHalfSpaceData.SetAll(0);
		}
	}

	void Redraw()
	{
		DIB& dib = mCanvas.GetDib();
		dib.SetAll(DIBColor::sCreateDefaultPaletteColor(dpcWhite));

		
		{
			int max = (1 << mHalfSpaces.GetLength());
			fvec2 corner(-mVisibleRange, -mVisibleRange);
			float stepx = float(mVisibleRange*2.0f)/float(dib.GetWidth());
			float stepy = float(mVisibleRange*2.0f)/float(dib.GetHeight());

			for (uint x = 0; x < dib.GetWidth(); x++)
			for (uint y = 0; y < dib.GetHeight(); y++)
			{
				uint32 mask = mHalfSpaceData.Get(x,y);
				float h = float(mask) / float(max) * 360.0f;
				dib.Set(x,y, DIBColor::sFromHSV(h, 1, 0.5));
			}
		}

		for (const HalfSpace2& hs : mHalfSpaces)
		{

			fvec2 o = hs.mNormal * hs.mOffset;
			fvec2 d(hs.mNormal.y, -hs.mNormal.x);

			HalfSpace2 hs0(0, 1,  mVisibleRange);
			HalfSpace2 hs1(0, -1, mVisibleRange);
			HalfSpace2 hs2(1, 0,  mVisibleRange);
			HalfSpace2 hs3(-1, 0, mVisibleRange);



			float t_min = FLT_MAX;
			float t_max = -FLT_MAX;

			float t0 = (-hs0.mOffset - o.y*hs0.mNormal.y - o.x*hs0.mNormal.x);
			float det = d.GetDot(hs0.mNormal);
			if (det < 0) t_min = gMin<float>(t_min, t0/det);
			if (det > 0) t_max = gMax<float>(t_max, t0/det);

			float t1 = (-hs1.mOffset - o.y*hs1.mNormal.y - o.x*hs1.mNormal.x);
			det = d.GetDot(hs1.mNormal);
			if (det < 0) t_min = gMin<float>(t_min, t1/det);
			if (det > 0) t_max = gMax<float>(t_max, t1/det);

			float t2 = (-hs2.mOffset - o.y*hs2.mNormal.y - o.x*hs2.mNormal.x);
			det = d.GetDot(hs2.mNormal);
			if (det < 0) t_min = gMin<float>(t_min, t2/det);
			if (det > 0) t_max = gMax<float>(t_max, t2/det);

			float t3 = (-hs3.mOffset - o.y*hs3.mNormal.y - o.x*hs3.mNormal.x); 
			det = d.GetDot(hs3.mNormal);
			if (det < 0) t_min = gMin<float>(t_min, t3/det);
			if (det > 0) t_max = gMax<float>(t_max, t3/det);


			fvec2 c0 = o + d*t_min;
			fvec2 c1 = o + d*t_max;

			fvec2 p0 = TransformClipToScreen(c0);
			fvec2 p1 = TransformClipToScreen(c1);

	
			fvec2 a0 = TransformClipToScreen(o+fvec2(d.y*0.1f, d.x*-0.1f));
			fvec2 a1 = TransformClipToScreen(o+d*0.2f);

			p0.x = gClamp<float>(p0.x + .5f, 0, GetWidth()-1);
			p0.y = gClamp<float>(p0.y + .5f, 0, GetHeight()-1);
			p1.x = gClamp<float>(p1.x + .5f, 0, GetWidth()-1);
			p1.y = gClamp<float>(p1.y + .5f, 0, GetHeight()-1);


			ColorPen<DIBColor> pen(dib);
			pen.DrawLine(p0, p1);
			pen.DrawLine(a0, a1);
		}
		Invalidate();
	}


private:

	// scale parameters
	float mVisibleRange				= 3.0f;
	float mVisibleRangeRcp			= 1.0f / 3.0f;

	fvec2 TransformClipToScreen(const fvec2& inClipCoord)
	{
		return fvec2(((inClipCoord.x * mVisibleRangeRcp + 1.0f) * 0.5f) * float(GetWidth()), ((inClipCoord.y * mVisibleRangeRcp + 1.0f) * 0.5f) * float(GetHeight()));
	}
	fvec2 TransformScreenToClip(const fvec2& inClipCoord)
	{
		return fvec2(((inClipCoord.x / float(GetWidth() / 2) - 1.0f) * mVisibleRange), ((inClipCoord.y / float(GetHeight() / 2) - 1.0f) * mVisibleRange));
	}


	// Handlers
	Array<HalfSpace2>			mHalfSpaces;
	DataFrame<uint32>			mHalfSpaceData;
	fvec2						mPrevClick = fvec2(0,0);
	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;

	int							mPointsSet = 0;
	fvec2						mP0;
	fvec2						mP1;
	fvec2						mP2;

} gMainWindow;







int main()
{

	gMainWindow.Create(L"ComeDither", 640, 640);
	gMainWindow.Show(true);
	gMainWindow.Redraw();
	while (gDoMessageLoop(true)) 
	{
	}

	std::cout << "test";
	return 0;
}

