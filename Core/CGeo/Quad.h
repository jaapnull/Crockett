#pragma once
#include <CMath/Vector.h>

template <class T>
class Rect
{
public:
	T mLeft;
	T mTop;
	T mRight;
	T mBottom;
	T GetWidth() const {return mRight - mLeft;}
	T GetHeight() const {return mBottom - mTop;}

	Rect() {}
	Rect(const T& inLeft, const T& inTop, const T& inRight, const T& inBottom) : mLeft(inLeft), mTop(inTop), mRight(inRight), mBottom(inBottom) {}
	Rect(const T& inWidth, const T& inHeight) : mLeft(0), mTop(0), mRight(inWidth), mBottom(inHeight) {}

	Rect<T> Scale(const T& inScale)
	{
		mLeft *= inScale;
		mTop *= inScale;
		mBottom *= inScale;
		mRight *= inScale;
		return *this;
	}
	
	Rect<T> Scale(const T& inScaleV, const T& inScaleH)
	{
		mLeft *= inScaleH;
		mRight *= inScaleH;
		mTop *= inScaleV;
		mBottom *= inScaleV;
		return *this;
	}

	Rect<T> GetTranslated(const Vector<T, 2>& inOffset) const
	{
		Rect<T> t = *this;
		t.Translate(inOffset);
		return t;
	}

	Rect<T> Translate(const Vector<T, 2>& inOffset)
	{
		mLeft += inOffset.x;
		mTop += inOffset.y;
		mBottom += inOffset.y;
		mRight += inOffset.x;
		return *this;
	}

	bool HasPositiveArea() const
	{
		return mLeft < mRight && mTop < mBottom;
	}

	bool HasInversions() const
	{
		return mLeft > mRight || mTop > mBottom;
	}

	bool HasZeroArea() const
	{
		return mLeft == mRight || mTop == mBottom;
	}


	T GetSurfaceArea() const
	{
		return GetWidth() * GetHeight();
	}

	bool IsPointRect() const
	{
		return mLeft == mRight && mTop == mBottom;
	}

	Rect<T> Moved(const Vector<T, 2> inMovement) const
	{
		Rect<T> r = *this;
		r.mLeft		+= inMovement.x;
		r.mRight	+= inMovement.x;
		r.mTop		+= inMovement.y;
		r.mBottom	+= inMovement.y;
		return r;
	}

	void SetEmpty()
	{
		mLeft = gMaxValue<T>();
		mTop = gMaxValue<T>();
		mBottom = gMinValue<T>();
		mRight = gMinValue<T>();
	}

	void SetZero()
	{
		mLeft = 0; mTop = 0; mRight = 0; mBottom = 0;
	}

	Rect<T> GetIntersect(const Rect<T>& other) const
	{
		Rect<T> temp = *this;
		temp.Intersect(other);
		return temp;
	}


	uint SubtractAndFragment(const Rect<T> inOther, Rect<T>* outFragments)
	{
		Rect<T> intersect = GetIntersect(inOther);
		if (!intersect.HasPositiveArea())
		{
			outFragments[0] = *this; return 1;
		}
		if (intersect == *this)
		{
			return 0;
		}
		// .-----------.
		// |   | U |   |
		// |   |---|   |
		// | L | i | R |
		// |   |---|   |
		// |   | D |   |
		// '-----------'
		uint fragments = 0;
		if (intersect.mLeft > mLeft) //L 
			outFragments[fragments++] = Rect<T>(mLeft, mTop, intersect.mLeft, mBottom);
		if (intersect.mRight < mRight)  //R
			outFragments[fragments++] = Rect<T>(intersect.mRight, mTop, mRight, mBottom);
		if (intersect.mTop > mTop) //U
			outFragments[fragments++] = Rect<T>(intersect.mLeft, mTop, intersect.mRight, intersect.mTop);
		if (intersect.mBottom < mBottom) //D
			outFragments[fragments++] = Rect<T>(intersect.mLeft, intersect.mBottom, intersect.mRight, mBottom);
		return fragments;
	}


	void Intersect(const Rect<T>& other)
	{
		mLeft = std::max<T>(mLeft, other.mLeft);
		mRight = std::min<T>(mRight, other.mRight);
		mTop = std::max<T>(mTop, other.mTop);
		mBottom = std::min<T>(mBottom, other.mBottom);
	}

	Rect<T> Bound(const Rect<T>& other)
	{
		// todo implement
		assert(false);
	}

	Rect<T> GetBound(const Rect<T>& other) const
	{
		Rect<T> temp = *this;
		temp.Bound(other);
		return temp;
	}
	
	Vector<T, 2> GetMin() const
	{
		return Vector<T, 2>(mLeft, mTop);
	}

	Rect<T> Widened(T inOffset)
	{
		Rect<T> t = *this;
		t.mLeft		-= inOffset;
		t.mRight	+= inOffset;
		t.mTop		-= inOffset;
		t.mBottom	+= inOffset;
		return t;
	}

	Vector<T, 2> GetMax() const
	{
		return Vector<T, 2>(mRight, mBottom);
	}


	Vector<T, 2> GetCenter() const
	{
		return Vector<T, 2>((mLeft+mRight)/T(2), (mTop+mBottom)/T(2));
	}


	Vector<T, 2> GetSpan() const
	{
		return Vector<T, 2>(mRight-mLeft, mBottom-mTop);
	}

	void Set(const Rect<T>& other)
	{
		mLeft = other.mLeft;
		mTop = other.mTop;
		mBottom = other.mBottom;
		mRight = other.mRight;
	}

	Rect<T> Enclose(const Vector<T, 2>& other)
	{
		mLeft = std::min<T>(mLeft, other.x);
		mRight = std::max<T>(mRight, other.x);
		mTop = std::min<T>(mTop, other.y);
		mBottom = std::max<T>(mBottom, other.y);
		return *this;
	}

	bool ContainsPoint(const Vector<T, 2>& other) const
	{
		return other.x >= mLeft && other.y >= mTop && other.x < mRight && other.y < mBottom;
	}

	bool OverlapsWith(const Rect<T>& other) const
	{
		bool horz = other.mLeft < mRight && mLeft < other.mRight;
		bool vert = other.mTop < mBottom && mTop < other.mBottom;
		return horz && vert;
	}

	bool FullyContains(const Rect<T>& other) const
	{
		if (HasInversions() || !other.HasInversions()) return false;
		return other.mLeft >= mLeft && other.mTop >= mTop && other.mRight <= mRight && other.mBottom <= mBottom;
	}

	Rect<T> Enclose(const Rect<T>& other)
	{
		if (GetSurfaceArea() == 0)
		{
			Set(other);
		}
		else
		{
			mLeft = std::min<T>(mLeft, other.mLeft);
			mRight = std::max<T>(mRight, other.mRight);
			mTop = std::min<T>(mTop, other.mTop);
			mBottom = std::max<T>(mBottom, other.mBottom);
		}
		return *this;
	}
	
	Vector<T, 2> GetDimensions() const
	{
		return Vector<T, 2>(GetWidth(), GetHeight());
	}

	Rect<T> Expand(T inValue)
	{
		mLeft -= inValue;
		mTop -= inValue;
		mRight += inValue;
		mBottom += inValue;
		return *this;
	}

	Rect<T> Expand(const fvec2& inValues)
	{
		mLeft -= inValues.x;
		mTop -= inValues.y;
		mRight += inValues.x;
		mBottom += inValues.y;
		return *this;
	}

	Rect<T> FlipValid()
	{
		if (mLeft > mRight) std::swap(mLeft, mRight);
		if (mTop > mBottom) std::swap(mTop, mBottom);
		return *this;
	}

	bool operator==(const Rect<T>& inOther) const
	{
		return	(mLeft	== inOther.mLeft) &&
				(mRight	== inOther.mRight) &&
				(mTop	== inOther.mTop) &&
				(mBottom== inOther.mBottom);
	}

	Rect<T> operator*(const T& inOther) const
	{
		return Rect<T>(	mLeft * inOther, mTop * inOther, mRight * inOther, mBottom * inOther);
	}

	Rect<T> operator+(const Vector<T, 2>& inOffset) const
	{
		return GetTranslated(inOffset);
	}

	Rect<T> operator-(const Vector<T, 2>& inOffset) const
	{
		return GetTranslated(-inOffset);
	}

	static Rect<T> sEmpty() { Rect<T> r; r.SetEmpty(); return r; }


};


template <class T>
std::ostream& operator<<(std::ostream& inStream, const Rect<T>& inRect)
{
	inStream << '{';
	inStream << inRect.mLeft << ',';
	inStream << inRect.mTop << ',';
	inStream << inRect.mRight << ',';
	inStream << inRect.mBottom;

	inStream << '}';
	return inStream;
}

typedef Rect<int> IRect;

class FRect : public Rect<float>
{
	public:
	static FRect sEmpty() { return FRect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX); }
	static FRect sUnit()  { return FRect(-1.0f, -1.0f, 1.0f, 1.0f); };

	// copy of constructors of Rect<float>
	FRect() {}
	FRect(const float inLeft, const float inTop, const float inRight, const float inBottom) : Rect<float>(inLeft, inTop, inRight, inBottom) {}
	FRect(const float inWidth, const float inHeight) : Rect<float>(inWidth, inHeight) {}
};


