#include <ClipGeo.h>

Array<Edge>			gEdges;
Array<Node>			gNodes;
Array<Ring>			gRings;

const Edge* EdgeIndex::operator->() const	{ return gEdges.GetData() + mIndex; }
Edge* EdgeIndex::operator->()				{ return gEdges.GetData() + mIndex; }

const Node* NodeIndex::operator->() const	{ return gNodes.GetData() + mIndex; }
Node* NodeIndex::operator->()				{ return gNodes.GetData() + mIndex; }

NodeIndex gCreateNodeFromPoint(const fvec2& inPoint)
{
	gNodes.Append(Node(inPoint));
	return NodeIndex((uint)gNodes.GetLength() - 1);
}


NodeIndex gFindOrCreateNodeFromPoint(const fvec2& inPoint)
{
	for (uint n = 0; n < gNodes.GetLength(); n++)
	{
		if (gNodes[n].GetPosition() == inPoint)
			return NodeIndex(n);
	}
	return gCreateNodeFromPoint(inPoint);
}

NodeIndex gFindOrCreateEdgeIntersection(EdgeIndex inEdgeA, EdgeIndex inEdgeB)
{
	uint inHitA = 0xFFFFFFFF;
	uint inHitB = 0xFFFFFFFF;
	for (NodeIndex n0 : inEdgeA->GetNodes())
		for (NodeIndex n1 : inEdgeB->GetNodes())
		{
			if (n0 == n1)
				return n0;
		}

	fvec2 intersection;
	if (inEdgeA->GetHalfSpace().GetIntersect(inEdgeB->GetHalfSpace(), intersection))
	{
		NodeIndex new_intersection = gFindOrCreateNodeFromPoint(intersection);
		new_intersection->AddEdge(inEdgeA);
		new_intersection->AddEdge(inEdgeB);
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
	EdgeIndex ei((uint)gEdges.GetLength());
	inNodeA->AddEdge(ei);
	inNodeB->AddEdge(ei);
	gEdges.Append(Edge(inNodeA, inNodeB));

	for (uint i = 0; i < gEdges.GetLength() - 1; i++)
	{
		gFindOrCreateEdgeIntersection(EdgeIndex(i), EdgeIndex(ei));
	}
	return ei;
}


EdgeIndex gFindEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB)
{
	for (EdgeIndex e : inNodeA->GetEdges())
	{
		if (e->HasNode(inNodeB))
		{
			return e;
		}
	}
	return EdgeIndex();
}


EdgeIndex gFindOrCreateEdgeFromNodes(NodeIndex inNodeA, NodeIndex inNodeB)
{
	for (EdgeIndex e : inNodeA->GetEdges())
	{
		if (e->HasNode(inNodeB))
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
	for (EdgeIndex ei : inNodeA->GetEdges())
	{
		if (ei->HasNode(inNodeB))
		{
			return ei;
		}
	}
	return 0xFFFFFFFF;
}