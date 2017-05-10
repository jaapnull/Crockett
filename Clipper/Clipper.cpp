#include <CCore/String.h>
#include <CMath/Math.h>
#include <CMath/Vector.h>
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



struct HalfSpaceMask
{
	HalfSpaceMask() {}
	HalfSpaceMask(uint32 inMask, uint32 inOrientation) : mMask(inMask), mOrientation(inOrientation) {}
	uint32 mMask;
	uint32 mOrientation;

	const String ToString()
	{
		String s;
		for (int i = 0; i < 32; i++)
		{
			if ((i&7)==0) s.Append('.');
			if (mMask&(1 << (31 - i)))
			{
				if(mOrientation&(1 << (31-i)))
					s.Append('1');
				else
					s.Append('0');
			}
			else
				s.Append('u');
		}
		return s;
	}

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
		SetupHalfSpaces();
	}

	virtual void OnSize(const ivec2& inNewSize) override
	{
		mHalfSpaceData.Resize(inNewSize.x, inNewSize.y);
		FillHalfSpaceData();
		Redraw();
	}

	virtual void OnMouseLeftUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
	}

	virtual void OnMouseLeave() override
	{
		mMouseOverHalfSpace = -1;
		DrawLines();
		Invalidate();
	}

	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		mLastMouse = pos;
		const float clip_space_selection_range = 0.1f;
		
		int idx = 0;
		int previous_selected = mMouseOverHalfSpace;
		mMouseOverHalfSpace = -1;
		for (const HalfSpace2& hs : mHalfSpaces)
		{
			if (gAbs(hs.SignedDistance(pos)) < clip_space_selection_range)
			{
				if (mMouseOverHalfSpace != idx)
				{
					mMouseOverHalfSpace = idx;
					DrawLines();
					Invalidate();
				}
				break;
			}
			idx++;
		}
		if (previous_selected != mMouseOverHalfSpace)
		{
			if (mMouseOverHalfSpace >= 0)
			{
				HalfSpace2& hs = mHalfSpaces[mMouseOverHalfSpace];
				std::cout << std::hex << hs.mOffset << ":" << hs.mNormal.x << ',' << hs.mNormal.y << std::endl;
			}
			DrawLines();
			Invalidate();
		}
	}

	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		if (mPointsSet > 0)
		{
			mPointsSet--;
			SetupHalfSpaces();
		}
		else
		{
			if (mPolygons.IsEmpty()) mPolygons.AppendEmpty();
			mPolygons[0].AppendVertex(pos);
		}
		FillHalfSpaceData();
		Redraw();
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		if (mMouseOverHalfSpace == -1)
			AddPoint(pos);
	}

	void AddPoint(fvec2 inPoint)
	{
		if (mPointsSet == 0)		{ mP[mPointsSet] = inPoint; mM[mPointsSet] = HalfSpaceMask(0xF, CreateFieldForCoord(inPoint));	mPointsSet++;  }
		else if (mPointsSet == 1)	{ mP[mPointsSet] = inPoint; mM[mPointsSet] = HalfSpaceMask(0xFF, CreateFieldForCoord(inPoint));	mPointsSet++;  }
		else if (mPointsSet == 2)	{ mP[mPointsSet] = inPoint; mM[mPointsSet] = HalfSpaceMask(0xFFF, CreateFieldForCoord(inPoint));mPointsSet++;  }
		SetupHalfSpaces();
		FillHalfSpaceData();
		Redraw();
	}

	uint32 CreateFieldForCoord(const fvec2& inPos)
	{
		int idx = 0;
		uint32 mask = 0;
		for (const HalfSpace2& hs : mHalfSpaces)
		{
			float d = hs.SignedDistance(inPos);
			if (d > 0) mask |= (0x1 << idx);
			idx++;
		}
		return mask;
	}

	void SetupHalfSpaces()
	{
		mMarkers.Clear();
		mLines.Clear();


		mHalfSpaces.Clear();
		mHalfSpaces.Append(HalfSpace2(0,1,1));
		mHalfSpaces.Append(HalfSpace2(0,-1,1));
		mHalfSpaces.Append(HalfSpace2(1,0,1));
		mHalfSpaces.Append(HalfSpace2(-1,0,1));

		if (mPointsSet >= 1)
		{
			mMarkers.Append(mP[0]);
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[0], fvec2(-1,-1)));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[0], fvec2( 1,-1)));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[0], fvec2(-1, 1)));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[0], fvec2( 1, 1)));
			std::cout << mM[0].ToString() << std::endl;
		}

		if (mPointsSet >= 2)
		{
			mMarkers.Append(mP[1]);
			//mLines.Append(LineSegment2(mP[0], mP[1]));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[1], fvec2(-1,-1)));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[1], fvec2( 1,-1)));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[1], fvec2(-1, 1)));
			mHalfSpaces.Append(HalfSpace2::sCreateBetweenPoints(mP[1], fvec2( 1, 1)));
			std::cout << mM[1].ToString() << std::endl;
		}

		if (mPointsSet >= 3)
		{
			mMarkers.Append(mP[2]);
			//mLines.Append(LineSegment2(mP[1], mP[2]));
			//mLines.Append(LineSegment2(mP[2], mP[0]));
			std::cout << mM[2].ToString() << std::endl;
		}



		Polygon2 p;
		p.AppendVertex(fvec2(-mVisibleRange,  mVisibleRange) * 0.9f);
		p.AppendVertex(fvec2( mVisibleRange,  mVisibleRange) * 0.9f);
		p.AppendVertex(fvec2( mVisibleRange, -mVisibleRange) * 0.9f);
		p.AppendVertex(fvec2(-mVisibleRange, -mVisibleRange) * 0.9f);
		

		mPolygons.Clear();
		mPolygons.Append(p);

		int m = 0;
		for (const HalfSpace2& hs : mHalfSpaces)
		{
			Array<Polygon2> output;
			for (const Polygon2& p : mPolygons)
			{
				Polygon2 i,o;
				p.SplitConvex(hs, i, o);
				if (o.GetVertexCount() >= 3) output.Append(o);
				if (i.GetVertexCount() >= 3) output.Append(i);
			}
			mPolygons = output;
			//if (++m == 1) break;
		}
	}

	void FillHalfSpaceData()
	{
		return;
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
				fvec2 pos = corner + fvec2(float(x) * stepx, float(y) * stepy);
				mHalfSpaceData.Set(x,y, CreateFieldForCoord(pos));
			}
		}
		else
		{
			mHalfSpaceData.SetAll(0);
		}
	}


	void DrawField()
	{
		mCanvas.GetDib().SetAll(DIBColor(128,64,128));
		return;
		DIB& dib = mCanvas.GetDib();
		fvec2 corner(-mVisibleRange, -mVisibleRange);
		float stepx = float(mVisibleRange*2.0f)/float(dib.GetWidth());
		float stepy = float(mVisibleRange*2.0f)/float(dib.GetHeight());

		for (uint x = 0; x < dib.GetWidth(); x++)
		for (uint y = 0; y < dib.GetHeight(); y++)
		{
			uint32 mask = mHalfSpaceData.Get(x,y);
			uint32 h = ((uint64(mask) * 5317ll) % uint64(0x600));
			dib.Set(x, y,  DIBColor::sFromHSVInt(h, 128, 255));
		}
	}

	void DrawLines()
	{
		DIB& dib = mCanvas.GetDib();
		ColorPen<DIBColor> pen(dib);

		int idx = 0;
		if (false)
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

			p0.x = gClamp<float>(p0.x + .5f, 0, (float) GetWidth()-1);
			p0.y = gClamp<float>(p0.y + .5f, 0, (float) GetHeight()-1);
			p1.x = gClamp<float>(p1.x + .5f, 0, (float) GetWidth()-1);
			p1.y = gClamp<float>(p1.y + .5f, 0, (float) GetHeight()-1);

			pen.SetColor((idx == mMouseOverHalfSpace) ? DIBColor(255,255,255) : DIBColor(0,0,0));
			pen.DrawLine(p0, p1);
			pen.DrawLine(a0, a1);
			idx++;

		}
		pen.SetColor(DIBColor(255,255,0));


		Array<LineSegment2> lines = mLines;
		Array<fvec2> points = mMarkers;

		struct CCW_Sort
		{
			fvec2 mMid;
			bool operator()(const fvec2& inA, const fvec2& inB) { return fvec2(inA-mMid).GetCross(inB-mMid) < 0.0f; }
		} ccws;

		//for (const LineSegment2& s : lines)
		//{
		//	fvec2 p0 = TransformClipToScreen(s.mTo);
		//	fvec2 p1 = TransformClipToScreen(s.mFrom);
		//	pen.DrawLine(p0, p1);
		//}
		//
		//for (const fvec2& f : points)
		//{
		//	ivec2 p(TransformClipToScreen(f));
		//	pen.DrawLine(ivec2(p.x-5, p.y-5), ivec2(p.x-5, p.y+5));
		//	pen.DrawLine(ivec2(p.x-5, p.y+5), ivec2(p.x+5, p.y+5));
		//	pen.DrawLine(ivec2(p.x+5, p.y+5), ivec2(p.x+5, p.y-5));
		//	pen.DrawLine(ivec2(p.x+5, p.y-5), ivec2(p.x-5, p.y-5));
		//}

		for (Polygon2& plgon : mPolygons)
		{
			if (plgon.IsEmpty()) 
				continue;

			if (plgon.CheckSide(mLastMouse) != HalfSpace2::esOutside)
			{
				pen.SetColor(DIBColor(255,255,255));
			}
			else
			{
				pen.SetColor(DIBColor(128,128,128));
			}

			

			fvec2 avg = fvec2::sZero();
			for (const fvec2& p : plgon) avg += p;
			avg /= float(plgon.GetVertexCount());
			points.Append(avg);

			ccws.mMid = avg + fvec2(0.1f, 0.0);

			fvec2 prev = plgon.Back();
			for (const fvec2& p : plgon)
			{
				points.Append(p);
				fvec2 shifted_p = p - ((p-avg).GetNormalised() * .1f);
				fvec2 shifted_prev = prev - ((prev-avg).GetNormalised() * .1f);
				if (prev != p) 
				{
					pen.DrawLine(TransformClipToScreen(prev), TransformClipToScreen(p));
				}
				prev = p;
			}
		
		}
	}

	void Redraw()
	{
		DrawField();
		DrawLines();
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

	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;

	int							mPointsSet = 0;
	int							mMouseOverHalfSpace = -1;
	
	Array<fvec2>				mMarkers;
	Array<LineSegment2>			mLines;
	Array<Polygon2>				mPolygons;

	fvec2						mLastMouse = fvec2(0,0);
	fvec2						mP[3];
	HalfSpaceMask				mM[3];

} gMainWindow;







int main()
{

	Test_Sort();

	gMainWindow.Create(L"ComeDither", 1024, 1025);
	gMainWindow.Show(true);
	gMainWindow.Redraw();

	while (gDoMessageLoop(true)) 
	{
	}

	std::cout << "test";
	return 0;
}

