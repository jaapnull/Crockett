#pragma once
#include <CGeo/HalfSpace2.h>

struct LineSegment2
{

	LineSegment2() {}
	LineSegment2(const fvec2& inFrom, const fvec2& inTo) : mFrom(inFrom), mTo(inTo) {}

	inline fvec2 GetDiff() const { return mTo - mFrom; }
	inline fvec2 GetInterpolated(float inI) const { return mFrom + GetDiff() * inI; }

	inline bool PointOnSegment(const fvec2& inPoint)
	{
		// first check for co-linearity....
		fvec2 d = GetDiff();
		fvec2 p0 = (inPoint - mFrom);
		fvec2 p1 = (inPoint - mTo);
		bool co_linear = p0.GetPerpDot(d) < cEpsilon;

		// then check if the point lies between the two half-spaces defined by the two end points
		return co_linear && p0.GetDot(d) >= 0 && p1.GetDot(d) <= 0;
	}

	inline bool Intersect(const HalfSpace2& inHalfSpace, fvec2& outIntersectPoint) const
	{
		fvec2 s = GetDiff();
		fvec2 r = inHalfSpace.mNormal.GetPerp();
		fvec2 d = mFrom - (inHalfSpace.mNormal * inHalfSpace.mOffset);
		float c = r.GetCross(s);
		if (c == 0) 
			return false;
		float t = d.GetCross(r) / c;
		outIntersectPoint = GetInterpolated(t);

		return (t >= 0 && t <= 1.0f);
	}


	inline bool Clip(const HalfSpace2 inHalfSpace)
	{
		bool from_inside = inHalfSpace.SignedDistance(mFrom) >= 0;
		bool to_inside = inHalfSpace.SignedDistance(mTo) >= 0;
		if (from_inside && !to_inside)
		{
			Intersect(inHalfSpace, mTo);
		}
		else if (!from_inside && to_inside)
		{
			Intersect(inHalfSpace, mFrom);
		}
		
		return (from_inside || to_inside);
	}


	inline bool Intersect(const LineSegment2& inSegment, fvec2& outIntersectPoint) const
	{
		fvec2 s = GetDiff();
		fvec2 r = inSegment.GetDiff();
		fvec2 d = mFrom - inSegment.mFrom;
		float c = r.GetCross(s);
		if (c == 0) 
			return false;

		float u = d.GetCross(s) / c;
		float t = d.GetCross(r) / c;
		outIntersectPoint = GetInterpolated(t);

		return (t >= 0 && t <= 1.0f && u >= 0 && u <= 1.0f);
	}

	fvec2 mTo;
	fvec2 mFrom;
};

