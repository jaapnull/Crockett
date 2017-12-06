#pragma once
#include <CMath/Math.h>
#include <CMath/Vector.h>
#include <CGeo/HalfSpace2.h>
#include <CGeo/LineSegment2.h>
#include <CGeo/Polygon2.h>


class Node;
struct NodeIndex
{
	NodeIndex() {}
	NodeIndex(uint inIndex) : mIndex(inIndex)				{}
	bool operator==(const NodeIndex& inIndex) const			{ return mIndex == inIndex.mIndex; }
	bool operator<(const NodeIndex& inIndex) const			{ return mIndex < inIndex.mIndex; }
	bool operator!=(const NodeIndex& inIndex) const			{ return mIndex != inIndex.mIndex; }
	bool IsValid() const									{ return mIndex != 0xFFFFFFFF; }
	const Node* operator->() const;
	Node* operator->();

	uint32 mIndex = 0xFFFFFFFF;
};

class Edge;
struct EdgeIndex
{
	EdgeIndex() {}
	EdgeIndex(uint inIndex) : mIndex(inIndex)				{}
	bool operator==(const EdgeIndex& inIndex) const			{ return mIndex == inIndex.mIndex; }
	bool operator<(const EdgeIndex& inIndex) const			{ return mIndex < inIndex.mIndex; }
	bool operator!=(const EdgeIndex& inIndex) const			{ return mIndex != inIndex.mIndex; }
	bool IsValid() const									{ return mIndex != 0xFFFFFFFF; }
	const Edge* operator->() const;
	Edge* operator->();

	uint32 mIndex = 0xFFFFFFFF;
};

class Node
{
public:
	Node() {}
	Node(const fvec2& inPos) : mPosition(inPos)				{}
	const fvec2&				GetPosition() const			{ return mPosition; }
	const Array<EdgeIndex>&		GetEdges() const			{ return mEdges; }
	void AddEdge(EdgeIndex inEdge)							{ if(mEdges.Find(inEdge) == cMaxSize64) mEdges.Append(inEdge); }
	bool HasEdge(EdgeIndex inEdge) const					{ return mEdges.Find(inEdge) != cMaxSize64; }
private:
	fvec2													mPosition;
	Array<EdgeIndex>										mEdges;
};
extern Array<Node> gNodes;

class Edge
{
public:
	Edge() {}
	Edge(NodeIndex inNodeA, NodeIndex inNodeB)				{ mNodes.Append(inNodeA); mNodes.Append(inNodeB); mHalfSpace = HalfSpace2::sCreateBetweenPoints(inNodeA->GetPosition(), inNodeB->GetPosition()); }
	void AddNode(NodeIndex inNode)							{ if(mNodes.Find(inNode) == cMaxSize64) mNodes.Append(inNode); }
	bool HasNode(NodeIndex inNode) const					{ return mNodes.Find(inNode) != cMaxSize64; }
	const Array<NodeIndex>& GetNodes() const				{ return mNodes; }
	const HalfSpace2& GetHalfSpace() const					{ return mHalfSpace; }
	float GetInterpolant(NodeIndex inNode)					{ gAssert(HasNode(inNode)); return fvec2(inNode->GetPosition() - (mHalfSpace.mNormal * mHalfSpace.mOffset)).GetCross(inNode->GetPosition()); }
private:
	HalfSpace2												mHalfSpace;
	Array<NodeIndex>										mNodes;
};

extern Array<Edge>			gEdges;


NodeIndex gCreateNodeFromPoint(const fvec2& inPoint);
NodeIndex gFindOrCreateNodeFromPoint(const fvec2& inPoint);
NodeIndex gFindOrCreateEdgeIntersection(EdgeIndex inEdgeA, EdgeIndex inEdgeB);

EdgeIndex gCreateEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB);
EdgeIndex gFindEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB);
EdgeIndex gFindOrCreateEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB);
EdgeIndex gCreateEdgeFromPoints(const fvec2& inPointA, const fvec2& inPointB);
EdgeIndex gGetSharedEdge(NodeIndex inNodeA, NodeIndex inNodeB);

class Ring
{
public:
	void AddEdge(EdgeIndex inNode, bool inFaceOrigin)
	{
		mEdges.Append(mEdges);
	}

private:

	struct OrientedEdge
	{
		OrientedEdge(EdgeIndex inIndex, bool inFacingOrigin) : mIndex(inIndex), mFacingOrigin(inFacingOrigin) {}
		EdgeIndex	mIndex;				// Edge that represents half-space 
		bool		mFacingOrigin;		// if true, the half space faces origin
	};
	Array<OrientedEdge> mEdges;
};

extern Array<Ring> gRings;

/*


void Split(const EdgeIndex& inSplitPlane, Ring& outInner, Ring& outOuter) const
{
if (mNodes.IsEmpty())
return;

for (int v_idx = 0; v_idx < mNodes.GetLength(); v_idx++)
{
NodeIndex v0 = mNodes[v_idx];
NodeIndex v1 = mNodes[(v_idx + 1) % mNodes.GetLength()];
EdgeIndex hs = mEdges[v_idx];
EdgeIndex v0v1 = gFindEdgeFromNodes(mNodes[v_idx], mNodes[(v_idx + 1) % mNodes.GetLength()]);
gAssert(v0v1 == hs);

HalfSpace2::ESide s0 = inSplitPlane->GetHalfSpace().GetSide(v0->GetPosition(), cEpsilon * 2.0f * (gAbs(v0->GetPosition().x) + gAbs(v0->GetPosition().y)));
float f0 = inSplitPlane->GetHalfSpace().SignedDistance(v0->GetPosition());
HalfSpace2::ESide s1 = inSplitPlane->GetHalfSpace().GetSide(v1->GetPosition(), cEpsilon * 2.0f * (gAbs(v1->GetPosition().x) + gAbs(v1->GetPosition().y)));
float f1 = inSplitPlane->GetHalfSpace().SignedDistance(v1->GetPosition());

if (s0 != HalfSpace2::esOutside)
{
//a
outInner.AddNode(mNodes[v_idx], s0 == HalfSpace2::esEdge && s1 != HalfSpace2::esInside ? inSplitPlane : mEdges[v_idx]);

if (s1 == HalfSpace2::esOutside)
{
if (s0 == HalfSpace2::esInside)
{
//b
//bool intersected = LineSegment2(v0, v1).Intersect(inSplitPlane, intersect_point);
NodeIndex intersection = gFindOrCreateEdgeIntersection(hs, inSplitPlane);
outInner.AddNode(intersection, inSplitPlane);

// c
//gAssert(hs.SignedDistance(intersect_point) < cEpsilon * 100.0f);
outOuter.AddNode(intersection, mEdges[v_idx]);
}
}
}


if (s0 != HalfSpace2::esInside)
{
//a
outOuter.AddNode(v0, s0 == HalfSpace2::esEdge && s1 != HalfSpace2::esOutside ? inSplitPlane : hs); //inverse splitplane

if (s1 == HalfSpace2::esInside)
{
//fvec2 intersect_point = v0;
if (s0 == HalfSpace2::esOutside)
{
//b
NodeIndex intersected = gFindOrCreateEdgeIntersection(hs, inSplitPlane);
//LineSegment2(v0, v1).Intersect(inSplitPlane, intersect_point);
//bool inverted = LineSegment2(v0, v1).GetDiff().GetCross(inSplitPlane.mNormal) > 0.0f;
outOuter.AddNode(intersected, inSplitPlane); //inverse splitpane

// c
outInner.AddNode(intersected, hs);
}
}
}

}
}

*/