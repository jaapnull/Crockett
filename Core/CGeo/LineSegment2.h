#pragma once
#include <CGeo/HalfSpace2.h>

struct LineSegment2
{

	LineSegment2() {}
	LineSegment2(const fvec2& inFrom, const fvec2& inTo) : mFrom(inFrom), mTo(inTo) {}

	inline fvec2 GetDiff() const { return mTo - mFrom; }
	inline fvec2 GetInterpolated(float inI) const { return mFrom + GetDiff() * inI; }

	inline bool Intersect(const HalfSpace2& inHalfSpace, fvec2& outIntersectPoint) const
	{
		fvec2 s = GetDiff();
		fvec2 r = inHalfSpace.mNormal.GetPerp();
		fvec2 d = (inHalfSpace.mNormal * inHalfSpace.mOffset) - mFrom;
		float c = s.GetCross(r);
		if (c == 0) return false;
		float t = d.GetCross(s) / c;
 
		outIntersectPoint = GetInterpolated(t);

		return (t >= 0 && t <= 1.0f);
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

