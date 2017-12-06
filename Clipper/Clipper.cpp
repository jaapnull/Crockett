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

#include <ClipGeo.h>

#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif

Array<fvec2> gUserPoints;

void gSetupScene()
{
	gNodes.Clear();
	gEdges.Clear();
	fvec2 corners[4] = { fvec2(-1,-1), fvec2(1,-1), fvec2(1, 1), fvec2(-1, 1) };
	float se = 4.0f;
	fvec2 screen_edges[4] = { fvec2(-se,-se), fvec2(se,-se), fvec2(se, se), fvec2(-se, se) };

	////

	Array<fvec2> points;
	for (fvec2 c : screen_edges)
	{
		gFindOrCreateNodeFromPoint(c);
		points.Append(c);
	}
	
	NodeIndex prev = gFindOrCreateNodeFromPoint(corners[3]);
	for (fvec2& c : corners)
	{
		NodeIndex p = gFindOrCreateNodeFromPoint(c);
		gFindOrCreateEdgeFromNodes(p, prev);
		prev = p;
	}

	gRings.Clear();




	//gRings.Append(Ring(points));




	
	for (int p = 1; p < gUserPoints.GetLength(); p++)
	{
		NodeIndex n_p0 = gFindOrCreateNodeFromPoint(gUserPoints[p-1]);
		NodeIndex n_p1 = gFindOrCreateNodeFromPoint(gUserPoints[p]);

		gFindOrCreateEdgeFromNodes(n_p0, n_p1);
		
		//for (fvec2& c : corners)
		//{
		//	NodeIndex n_c = gFindOrCreateNodeFromPoint(c);
		//	EdgeIndex e = gFindOrCreateEdgeFromNodes(n_p, n_c);
		//}
	}
//			for (uint ie = 0; ie < gEdges.GetLength(); ie++)
//				gFindOrCreateEdgeIntersection(e, EdgeIndex(ie));
		//
		//	Array<Ring> new_rings;
		//	for (Ring& r : gRings)
		//	{
		//		
		//		Ring inner, outer;
		//		r.Split(e, inner, outer);
		//		if (inner.mNodes.GetLength() > 2) new_rings.Append(inner);
		//		if (outer.mNodes.GetLength() > 2) new_rings.Append(outer);
		//		gAssert(!new_rings.IsEmpty());
		//	}
		//	gRings = new_rings;
		// }

	

}


class MyWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{

public:

	MyWindow() : Window(), mCanvas(*this), mMouseHandler(*this), mKeyboard(*this)
	{
		AddHandler(&mCanvas);
		mCanvas.SetDefaultColor(DIBColor(255,255,255));
		AddHandler(&mMouseHandler);
		AddHandler(&mKeyboard);
	}

	virtual void OnSized(const ivec2& inNewSize) override
	{
		Redraw();
	}

	virtual void OnMouseLeftUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
	}

	virtual void OnKeyDown(const uint inKeyCode)
	{
		Redraw();
	}

	virtual void OnMouseLeave() override
	{
		DrawLines();
		Invalidate();
	}

	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		mLastMouse = pos;
		//Redraw();
	}

	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		if (!gUserPoints.IsEmpty())
		{
			gUserPoints.Pop();
			gSetupScene();
		}
		Redraw();
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);

		gUserPoints.Append(mLastMouse);
		gSetupScene();
		Redraw();

	}



	void DrawLines()
	{
		DIB& dib = mCanvas.GetDib();
		dib.SetAll(DIBColor(32, 32, 32));
		ColorPen<DIBColor> pen(dib);

		struct Text
		{
			Text() {}
			Text(const fvec2& inPos, const String& inText) : mPos(inPos), mText(inText) {}
			fvec2 mPos;
			String mText;
		};

		Array<Text> texts;
		Array<Text> texts_large;
		Array<LineSegment2> lines				= mLines;
		Array<fvec2> points_selected			= mMarkers;
		Array<fvec2> points_selected_large;
		Array<fvec2> points_unselected;
		Array<LineSegment2> lines_selected;
		Array<LineSegment2> lines_unselected;
		Array<LineSegment2> lines_dotted;

		////

		int n_id = 0;
		for (Node& n : gNodes)
		{
			points_unselected.Append(n.GetPosition());
			fvec2 avg_normal(0, 0);
			for (EdgeIndex e : n.GetEdges())
			{
				lines_unselected.Append(LineSegment2(n.GetPosition() - e->GetHalfSpace().mNormal.GetPerp() * 0.1f, n.GetPosition() + e->GetHalfSpace().mNormal.GetPerp() * 0.1f));
				avg_normal += e->GetHalfSpace().mNormal;
			}
			//texts.Append(Text(avg_normal.GetSafeNormalized() * 0.1f + n.mPosition, gToString(n_id++)));
		}

		mVisibleRange = 8.0f;

		int r_id = 0;
		for (Ring& r : gRings)
		{
			//if (r.mNodes.IsEmpty())
			//	continue;
			//fvec2 avg = fvec2::sZero();
			//fvec2 prev = r.mNodes.Back()->GetPosition();
			//for (NodeIndex n : r.mNodes)
			//{
			//	fvec2 p = n->GetPosition();
			//	points_selected.Append(p);
			//	lines_selected.Append(LineSegment2(prev, p));
			//	prev = p;
			//	mVisibleRange = gMax(mVisibleRange, gMax(gAbs(p.x), gAbs(p.y)));
			//	avg += p;
			//}
			//avg /= float(r.mNodes.GetLength());
			//
			//points_selected_large.Append(avg);
			//texts_large.Append(Text(avg + fvec2(0.05f,0), gToString(r_id++)));
		}
		mVisibleRange *= 1.1f;
		mVisibleRangeRcp = 1.0f / mVisibleRange;


		for (Edge& e : gEdges)
		{
			for (NodeIndex n : e.GetNodes())
			{
				points_unselected.Append(n->GetPosition());
				texts.Append(Text(n->GetPosition() + e.GetHalfSpace().mNormal.GetPerp()*0.2f, gToString(e.GetInterpolant(n))));
			}

			fvec2 m = e.GetHalfSpace().mNormal * e.GetHalfSpace().mOffset;
			fvec2 p0 = m + e.GetHalfSpace().mNormal.GetPerp() * mVisibleRange * 2.0f;
			fvec2 p1 = m - e.GetHalfSpace().mNormal.GetPerp() * mVisibleRange * 2.0f;
			lines_dotted.Append(LineSegment2(p0, p1));
		}

		////

		pen.SetColor(DIBColor(64,64,64));
		pen.SetPattern(0xF0F0F0F0);
		for (const LineSegment2& ls : lines_dotted)
			pen.DrawLine(TransformClipToScreen(ls.mFrom), TransformClipToScreen(ls.mTo));
		pen.SetPattern(-1);

		for (const LineSegment2& ls : lines_unselected)
			pen.DrawLine(TransformClipToScreen(ls.mFrom), TransformClipToScreen(ls.mTo));

		for (const fvec2& p : points_unselected)
		{
			fvec2 ps = TransformClipToScreen(p);
			pen.DrawLine(ps + fvec2(-1,-1), ps + fvec2(-1, 1));
			pen.DrawLine(ps + fvec2(-1, 1), ps + fvec2( 1, 1));
			pen.DrawLine(ps + fvec2( 1, 1), ps + fvec2( 1,-1));
			pen.DrawLine(ps + fvec2( 1,-1), ps + fvec2(-1,-1));
		}

		
		pen.SetColor(DIBColor(255,255,255));
		for (const LineSegment2& ls : lines_selected)
			pen.DrawLine(TransformClipToScreen(ls.mFrom), TransformClipToScreen(ls.mTo));

		
		for (const fvec2& p : points_selected)
		{
			fvec2 ps = TransformClipToScreen(p);
			pen.DrawLine(ps + fvec2(-1,-1), ps + fvec2(-1, 1));
			pen.DrawLine(ps + fvec2(-1, 1), ps + fvec2( 1, 1));
			pen.DrawLine(ps + fvec2( 1, 1), ps + fvec2( 1,-1));
			pen.DrawLine(ps + fvec2( 1,-1), ps + fvec2(-1,-1));
		}

		for (const fvec2& p : points_selected_large)
		{
			fvec2 ps = TransformClipToScreen(p);
			pen.DrawLine(ps + fvec2(-2,-2), ps + fvec2(-2, 2));
			pen.DrawLine(ps + fvec2(-2, 2), ps + fvec2( 2, 2));
			pen.DrawLine(ps + fvec2( 2, 2), ps + fvec2( 2,-2));
			pen.DrawLine(ps + fvec2( 2,-2), ps + fvec2(-2,-2));
		}

		for (const Text& t : texts)
		{
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(-1, -1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(-1, 0), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(-1, 1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(1, -1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(1, 0), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(1, 1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(0, 1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(0, -1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 1>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(0, 0), DIBColor(255, 255, 255));
		}
		for (const Text& t : texts_large)
		{
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(-1, -1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(-1, 0), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(-1, 1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(1, -1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(1, 0), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(1, 1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(0, 1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(0, -1), DIBColor(0, 0, 0));
			gDrawDebugFontText<DIBColor, 2>(t.mText, dib, ivec2(TransformClipToScreen(t.mPos)) + ivec2(0, 0), DIBColor(255, 255, 255));
		}

	}

	void Redraw()
	{
		DrawLines();
		Invalidate();
	}

private:

	// scale parameters
	float mVisibleRange				= 6.0f;
	float mVisibleRangeRcp			= 1.0f / mVisibleRange;

	fvec2 TransformClipToScreen(const fvec2& inClipCoord)
	{
		return fvec2((((inClipCoord.x * mVisibleRangeRcp) + 1.0f) * 0.5f) * float(GetWidth()), (((inClipCoord.y * mVisibleRangeRcp) + 1.0f) * 0.5f) * float(GetHeight()));
	}
	fvec2 TransformScreenToClip(const fvec2& inClipCoord)
	{
		return fvec2(((inClipCoord.x / float(GetWidth() / 2) - 1.0f) * mVisibleRange), ((inClipCoord.y / float(GetHeight() / 2) - 1.0f) * mVisibleRange));
	}


	// Handlers

	bool						mStep2 = false;

	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;

	Array<fvec2>				mMarkers;
	Array<LineSegment2>			mLines;

	fvec2						mLastMouse = fvec2(0,0);

} gMainWindow;







int main()
{


	gSetupScene();
	gMainWindow.Create(L"ComeDither", 1024, 1024);
	gMainWindow.Show(true);
	gMainWindow.Redraw();

	while (gDoMessageLoop(true)) 
	{
	}

	std::cout << "test";
	return 0;
}

