#pragma once
#include <CCore/Array.h>
#include <CMath/Vector.h>
#include <CGeo/HalfSpace2.h>
#include <CGeo/LineSegment2.h>
#include <CGeo/Quad.h>

class Polygon2
{
public:

	void InsertVertex(const fvec2& inNewVertex, size64 inBeforeIndex)	{ mVertices.Insert(inNewVertex, inBeforeIndex); }
	void AppendVertex(const fvec2& inNewVertex)							{ mVertices.Append(inNewVertex); }

	const BoundingBox2 GetBoundingBox() const
	{
		if (mVertices.IsEmpty())
			return BoundingBox2();
		fvec2 v_min = mVertices[0];
		fvec2 v_max = mVertices[0];
		for (fvec2 v : mVertices)
		{
			v_min = gMin(v_min, v);
			v_max = gMax(v_max, v);
		}
		return BoundingBox2(v_min, v_max);
	}
	

	HalfSpace2::ESide CheckSide(const fvec2& inPoint)
	{
		if (GetVertexCount() == 1) return inPoint.IsNear(mVertices[0]) ? HalfSpace2::esEdge : HalfSpace2::esOutside;
		if (GetVertexCount() == 2) return LineSegment2(mVertices[0], mVertices[1]).PointOnSegment(inPoint) ? HalfSpace2::esEdge : HalfSpace2::esOutside;

		HalfSpace2::ESide side = HalfSpace2::esInside;

		fvec2 prev = mVertices.Back();
		for (const fvec2& v : mVertices)
		{
			fvec2 p = inPoint - v;
			float d = (prev-v).GetPerpDot(p);
			if (gIsNear(d, 0)) side = HalfSpace2::esEdge;
			else if (d > 0) return HalfSpace2::esOutside;
			prev = v;
		}
		return side;
	}


	void SplitConvex(const HalfSpace2& inSplitPlane, Polygon2& outInner, Polygon2& outOuter) const
	{
		if (mVertices.IsEmpty())
			return;


		fvec2 prev_vert = mVertices.Back();
		HalfSpace2::ESide prev_side = inSplitPlane.GetSide(prev_vert);
		for (const fvec2& v : mVertices)
		{
			HalfSpace2::ESide new_side = inSplitPlane.GetSide(v);
			if ((prev_side == HalfSpace2::esInside && new_side == HalfSpace2::esOutside) || (prev_side == HalfSpace2::esOutside && new_side == HalfSpace2::esInside))
			{
				fvec2 intersect_point;
				bool intersected = LineSegment2(prev_vert, v).Intersect(inSplitPlane, intersect_point);
				if (!intersected)
				{
					float f0 = inSplitPlane.SignedDistance(prev_vert);
					float f1 = inSplitPlane.SignedDistance(v);
					std::cout << f0 << ',' << f1 << std::endl;
				}
				outOuter.AppendVertex(intersect_point);
				outInner.AppendVertex(intersect_point);
			}
			if (new_side != HalfSpace2::esInside) outOuter.AppendVertex(v);
			if (new_side != HalfSpace2::esOutside) outInner.AppendVertex(v);
			prev_vert = v;
			prev_side = new_side;
		}
		return;
	}

	const fvec2&		Back() const									{ return mVertices.Back(); }
	fvec2&				Back()											{ return mVertices.Back(); }
	bool				IsEmpty() const									{ return mVertices.IsEmpty(); }
	size64				GetVertexCount() const							{ return mVertices.GetLength(); }

	fvec2&				operator[](size64 inIndex)						{ return mVertices[inIndex]; }
	const fvec2&		operator[](size64 inIndex) const				{ return mVertices[inIndex]; }

	// Range based for accessors
	const fvec2*		begin() const									{ return mVertices.begin(); }
	const fvec2*		end() const										{ return mVertices.end(); }
	fvec2*				begin()											{ return mVertices.begin(); }
	fvec2*				end()											{ return mVertices.end(); }
private:
	Array<fvec2> mVertices;

};
#pragma once
