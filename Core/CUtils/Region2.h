#pragma once
#include <CGeo/Polygon2.h>
#include <CGeo/HalfSpace2.h>

class Region2 : public Polygon2
{

public:

	void SplitConvexEx(const HalfSpace2& inSplitPlane, Region2& outInner, Region2& outOuter) const
	{
		if (mVertices.IsEmpty())
			return;

		for (int v_idx = 0; v_idx < mVertices.GetLength(); v_idx++)
		{
			fvec2 v0 = mVertices[v_idx];
			fvec2 v1 = mVertices[(v_idx + 1) % GetVertexCount()];
			const HalfSpace2& hs = mHalfSpaces[v_idx];

			HalfSpace2::ESide s0 = inSplitPlane.GetSide(v0, cEpsilon * 2.0f * (gAbs(v0.x) + gAbs(v0.y)));
			float f0 = inSplitPlane.SignedDistance(v0);
			HalfSpace2::ESide s1 = inSplitPlane.GetSide(v1, cEpsilon * 2.0f * (gAbs(v1.x) + gAbs(v1.y)));
			float f1 = inSplitPlane.SignedDistance(v1);

			if (s0 != HalfSpace2::esOutside)
			{
				//a
				outInner.AddSide(v0, s0 == HalfSpace2::esEdge && s1 != HalfSpace2::esInside ? inSplitPlane : hs);

				if (s1 == HalfSpace2::esOutside)
				{
					fvec2 intersect_point = v0;
					if (s0 == HalfSpace2::esInside)
					{
						//b
						bool intersected = LineSegment2(v0, v1).Intersect(inSplitPlane, intersect_point);
						bool inverted = LineSegment2(v0, v1).GetDiff().GetCross(inSplitPlane.mNormal) < 0.0f;
						outInner.AddSide(intersect_point, inSplitPlane);

						// c
						gAssert(hs.SignedDistance(intersect_point) < cEpsilon * 100.0f);
						outOuter.AddSide(intersect_point, hs);
					}
				}
			}


			if (s0 != HalfSpace2::esInside)
			{
				//a
				outOuter.AddSide(v0, s0 == HalfSpace2::esEdge && s1 != HalfSpace2::esOutside ? inSplitPlane.GetInverse() : hs);

				if (s1 == HalfSpace2::esInside)
				{
					fvec2 intersect_point = v0;
					if (s0 == HalfSpace2::esOutside)
					{
						//b
						bool intersected = LineSegment2(v0, v1).Intersect(inSplitPlane, intersect_point);
						bool inverted = LineSegment2(v0, v1).GetDiff().GetCross(inSplitPlane.mNormal) > 0.0f;
						outOuter.AddSide(intersect_point, inSplitPlane.GetInverse());

						// c
						outInner.AddSide(intersect_point, hs);
					}
				}
			}

		}
	}

	bool CutsWithHalfSpace(const HalfSpace2& inHS) const
	{
		return mHalfSpaces.Find(inHS) != cMaxSize64;
	}

	bool HasCorner(const fvec2& inCorner) const
	{
		return mVertices.Find(inCorner) != cMaxSize64;
	}


	void AddSide(const fvec2& inBegin, const HalfSpace2& inEdge)
	{
		//gAssert(mVertices.IsEmpty() || inBegin != mVertices.Back());
		if (!mVertices.IsEmpty() && mVertices.Back() == inBegin) return;
		gAssert(inBegin.IsRational());
		float e = cEpsilon * 2.0f * (gAbs(inBegin.x) + gAbs(inBegin.y)) * 1000.0f;
		float d = inEdge.SignedDistance(inBegin);
		gAssert(inEdge.GetSide(inBegin, e) == HalfSpace2::esEdge);
		gAssert(mVertices.GetLength() == mHalfSpaces.GetLength());
		mVertices.Append(inBegin);
		mHalfSpaces.Append(inEdge);
	}


	uint16				mHalfSpaceOrders;
	Array<HalfSpace2>	mHalfSpaces;
};


