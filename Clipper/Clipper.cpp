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


struct Node;
struct NodeIndex
{
	NodeIndex() {}
	NodeIndex(uint inIndex) : mIndex(inIndex) {}
	bool operator==(const NodeIndex& inIndex) const		{ return mIndex == inIndex.mIndex; }
	bool operator<(const NodeIndex& inIndex) const		{ return mIndex < inIndex.mIndex; }
	bool operator!=(const NodeIndex& inIndex) const		{ return mIndex != inIndex.mIndex; }
	bool IsValid() const								{ return mIndex != 0xFFFFFFFF; }
	const Node* operator->() const;
	Node* operator->();

	uint32 mIndex = 0xFFFFFFFF;
};

struct Edge;
struct EdgeIndex
{
	EdgeIndex() {}
	EdgeIndex(uint inIndex) : mIndex(inIndex) {}
	bool operator==(const EdgeIndex& inIndex) const		{ return mIndex == inIndex.mIndex; }
	bool operator<(const EdgeIndex& inIndex) const		{ return mIndex < inIndex.mIndex; }
	bool operator!=(const EdgeIndex& inIndex) const		{ return mIndex != inIndex.mIndex; }
	bool IsValid() const								{ return mIndex != 0xFFFFFFFF; }
	const Edge* operator->() const;
	Edge* operator->();

	uint32 mIndex;
};

struct Node
{
	Node() {}
	Node(const fvec2& inPos) : mPosition(inPos) {}
	fvec2				mPosition;
	Array<EdgeIndex>	mEdges;
};

Array<Node>			gNodes;
const Node* NodeIndex::operator->() const	{ return gNodes.GetData() + mIndex; }
Node* NodeIndex::operator->()				{ return gNodes.GetData() + mIndex; }


struct Edge
{
	Edge() {}
	Edge(NodeIndex inNodeA, NodeIndex inNodeB)
	{
		mNodes.Append(inNodeA);
		mNodes.Append(inNodeB);
		mHalfSpace = HalfSpace2::sCreateBetweenPoints(inNodeA->mPosition, inNodeB->mPosition);
	}

	void AddNode(NodeIndex inNode)
	{
		gAssert(mNodes.Find(inNode) == cMaxSize64);
		mNodes.Append(inNode);
	}

	HalfSpace2			mHalfSpace;
	Array<NodeIndex>	mNodes;
};

Array<Edge>			gEdges;
const Edge* EdgeIndex::operator->() const	{ return gEdges.GetData() + mIndex; }
Edge* EdgeIndex::operator->()				{ return gEdges.GetData() + mIndex; }


NodeIndex gCreateNodeFromPoint(const fvec2& inPoint)
{
	gNodes.Append(Node(inPoint)); 
	return NodeIndex((uint) gNodes.GetLength()-1); 
}


NodeIndex gFindOrCreateNodeFromPoint(const fvec2& inPoint)
{
	for (uint n = 0; n < gNodes.GetLength(); n++)
	{
		if (gNodes[n].mPosition == inPoint) 
			return NodeIndex(n);
	}	
	return gCreateNodeFromPoint(inPoint);
}


NodeIndex gFindOrCreateEdgeIntersection(EdgeIndex inEdgeA, EdgeIndex inEdgeB)
{

	uint inHitA = 0xFFFFFFFF;
	uint inHitB = 0xFFFFFFFF;
	for (NodeIndex n0 : inEdgeA->mNodes)
	for (NodeIndex n1 : inEdgeB->mNodes)
	{
		if (n0 == n1)
			return n0;
	}

	fvec2 intersection;
	if (inEdgeA->mHalfSpace.GetIntersect(inEdgeB->mHalfSpace, intersection))
	{
		NodeIndex new_intersection = gFindOrCreateNodeFromPoint(intersection);
		new_intersection->mEdges.Append(inEdgeA);
		new_intersection->mEdges.Append(inEdgeB);
		inEdgeA->AddNode(new_intersection);
		inEdgeB->AddNode(new_intersection);
		return new_intersection;
	}
	else
	{
		return 0xFFFFFFFF;
	}
}


EdgeIndex gCreateEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB)
{
	EdgeIndex ei((uint) gEdges.GetLength());
	inNodeA->mEdges.Append(ei);
	inNodeB->mEdges.Append(ei);
	gEdges.Append(Edge(inNodeA, inNodeB));

	for (uint i = 0; i < gEdges.GetLength()-1; i++)
	{
		gFindOrCreateEdgeIntersection(EdgeIndex(i), EdgeIndex(ei));
	}

	return ei;
}

EdgeIndex gFindOrCreateEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB)
{
	for (EdgeIndex e : inNodeA->mEdges)
	{
		if (e->mNodes.Find(inNodeB) != cMaxSize64)
		{
			return e;
		}
	}
	return gCreateEdgeFromNodes(inNodeA, inNodeB);
}


EdgeIndex gCreateEdgeFromPoints(const fvec2& inPointA, const fvec2& inPointB)
{
	NodeIndex n0 = gFindOrCreateNodeFromPoint(inPointA);
	NodeIndex n1 = gFindOrCreateNodeFromPoint(inPointB);
	return gFindOrCreateEdgeFromNodes(n0, n1);
}

EdgeIndex gGetSharedEdge(NodeIndex inNodeA, NodeIndex inNodeB)
{
	for (EdgeIndex ei : inNodeA->mEdges)
	{
		if (ei->mNodes.Find(inNodeB))
		{
			return ei;
		}
	}
	return 0xFFFFFFFF;
}


struct Ring
{

	Ring(Array<fvec2>& inPoints)
	{
		if (inPoints.IsEmpty()) return;

		NodeIndex prev_n = gFindOrCreateNodeFromPoint(inPoints.Back());
		for (fvec2& f : inPoints)
		{
			NodeIndex n = gFindOrCreateNodeFromPoint(f);
			mNodes.Append(n);
			mEdges.Append(gFindOrCreateEdgeFromNodes(prev_n, n));
			prev_n = n;
		}
	}


	void AddNode(NodeIndex inNode, EdgeIndex inEdge)
	{
		gAssert(mNodes.IsEmpty() || gGetSharedEdge(mNodes.Back(), inNode).IsValid());
		mNodes.Append(inNode);
		mEdges.Append(inEdge);
	}

	Array<NodeIndex> mNodes;
	Array<EdgeIndex> mEdges;
};
Array<Ring> gRings;


Array<fvec2> gPoints;

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

	
	gRings.Append(Ring(points));


	for (fvec2& p : gPoints)
	{
		NodeIndex n_p = gFindOrCreateNodeFromPoint(p);

		for (fvec2& c : corners)
		{
			NodeIndex n_c = gFindOrCreateNodeFromPoint(c);
			EdgeIndex e = gFindOrCreateEdgeFromNodes(n_p, n_c);

//			for (uint ie = 0; ie < gEdges.GetLength(); ie++)
//				gFindOrCreateEdgeIntersection(e, EdgeIndex(ie));
		}

	}

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

	virtual void OnSize(const ivec2& inNewSize) override
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
		Redraw();
	}

	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);
		if (!gPoints.IsEmpty())
		{
			gPoints.Pop();
			gSetupScene();
		}
		Redraw();
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		fvec2 pos = TransformScreenToClip(inPosition);

		gPoints.Append(mLastMouse);
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
		Array<LineSegment2> lines				= mLines;
		Array<fvec2> points_selected			= mMarkers;
		Array<fvec2> points_unselected;
		Array<LineSegment2> lines_selected;
		Array<LineSegment2> lines_unselected;
		Array<LineSegment2> lines_dotted;

		////

		int n_id = 0;
		for (Node& n : gNodes)
		{
			points_unselected.Append(n.mPosition);
			fvec2 avg_normal(0, 0);
			for (EdgeIndex e : n.mEdges)
			{
				lines_unselected.Append(LineSegment2(n.mPosition - e->mHalfSpace.mNormal.GetPerp() * 0.1f, n.mPosition + e->mHalfSpace.mNormal.GetPerp() * 0.1f));
				avg_normal += e->mHalfSpace.mNormal;
			}
			//texts.Append(Text(avg_normal.GetSafeNormalized() * 0.1f + n.mPosition, gToString(n_id++)));
		}

		mVisibleRange = 1.0f;

		for (Ring& r : gRings)
		{
			if (r.mNodes.IsEmpty())
				continue;

			fvec2 prev = r.mNodes.Back()->mPosition;
			for (NodeIndex n : r.mNodes)
			{
				fvec2 p = n->mPosition;
				points_selected.Append(p);
				lines_selected.Append(LineSegment2(prev, p));
				prev = p;
				mVisibleRange = gMax(mVisibleRange, gMax(gAbs(p.x), gAbs(p.y)));
			}
		}
		mVisibleRange *= 1.1f;
		mVisibleRangeRcp = 1.0f / mVisibleRange;


		for (Edge& e : gEdges)
		{
			for (NodeIndex n : e.mNodes)
				points_unselected.Append(n->mPosition);

			fvec2 m = e.mHalfSpace.mNormal * e.mHalfSpace.mOffset;
			fvec2 p0 = m + e.mHalfSpace.mNormal.GetPerp() * mVisibleRange * 2.0f;
			fvec2 p1 = m - e.mHalfSpace.mNormal.GetPerp() * mVisibleRange * 2.0f;
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

		for (const Text& t : texts)
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

