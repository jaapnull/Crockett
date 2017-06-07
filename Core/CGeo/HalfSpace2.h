#pragma once
#include <CMath/Math.h>
#include <CMath/Vector.h>

struct HalfSpace2
{

	enum ESide
	{
		esInside,
		esOutside,
		esEdge
	};

	bool operator==(const HalfSpace2& inOther) const { return mOffset == inOther.mOffset && mNormal == inOther.mNormal; }

	HalfSpace2() {}
	HalfSpace2(float inNormX, float inNormY, float inOffset) : mNormal(inNormX, inNormY), mOffset(inOffset) {}
	HalfSpace2(const fvec2& inNormal, float inOffset) : mNormal(inNormal), mOffset(inOffset) {}

	const HalfSpace2 GetInverse() const { return HalfSpace2(-mNormal, -mOffset); }

	inline float SignedDistance(const fvec2& inPoint) const
	{
		return inPoint.GetDot(mNormal) - mOffset;
	}

	inline ESide GetSide(const fvec2& inPoint, float inEpsilon = cEpsilon) const
	{
		float dist = SignedDistance(inPoint);
		return (dist > inEpsilon) ? esOutside : (dist < -inEpsilon) ? esInside : esEdge;
	}

	static const HalfSpace2 sCreateBetweenPoints(const fvec2& inPointA, const fvec2& inPointB)
	{
			fvec2 s = inPointA-inPointB;
			fvec2 n = s.GetPerp().GetNormalized();
			return HalfSpace2(n, n.GetDot(inPointA));
	}


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

	fvec2		mNormal;			///< normal of the plane
	float		mOffset;			///< offset of the plane from origin
};

