#pragma once
#include "../Core/Vector.h"

template <class T>
class Quad
{
public:
	T mLeft;
	T mTop;
	T mRight;
	T mBottom;
	T GetWidth() const {return mRight - mLeft;}
	T GetHeight() const {return mBottom - mTop;}

	Quad() {}
	Quad(const T& inLeft, const T& inTop, const T& inRight, const T& inBottom) : mLeft(inLeft), mTop(inTop), mRight(inRight), mBottom(inBottom) {}
	Quad(const T& inWidth, const T& inHeight) : mLeft(0), mTop(0), mRight(inWidth), mBottom(inHeight) {}

	Quad<T> Scale(const T& inScale)
	{
		mLeft *= inScale;
		mTop *= inScale;
		mBottom *= inScale;
		mRight *= inScale;
		return *this;
	}
	
	Quad<T> Scale(const T& inScaleV, const T& inScaleH)
	{
		mLeft *= inScaleH;
		mRight *= inScaleH;
		mTop *= inScaleV;
		mBottom *= inScaleV;
		return *this;
	}

	Quad<T> GetTranslated(const Vector<T, 2>& inOffset) const
	{
		Quad<T> t = *this;
		t.Translate(inOffset.x, inOffset.y);
		return t;
	}

	Quad<T> Translate(const T& inOffsetV, const T& inOffsetH)
	{
		mLeft += inOffsetV;
		mTop += inOffsetH;
		mBottom += inOffsetH;
		mRight += inOffsetV;
		return *this;
	}

	bool IsValid() const
	{
		return GetWidth() > 0 && GetHeight() > 0;
	}

	T GetSurfaceArea() const
	{
		return GetWidth() * GetHeight();
	}

	bool IsZero() const
	{
		return mLeft == 0 && mTop == 0 && mRight == 0 && mBottom == 0;
	}

	void SetInvalid()
	{
		mLeft = FLT_MAX;
		mRight = -FLT_MAX;
		mTop = FLT_MAX;
		mBottom = -FLT_MAX;

	}

	void SetZero()
	{
		mLeft = 0; mTop = 0; mRight = 0; mBottom = 0;
	}

	Quad<T> GetIntersect(const Quad<T>& other) const
	{
		Quad<T> temp = *this;
		temp.Intersect(other);
		return temp;
	}

	void Intersect(const Quad<T>& other)
	{
		mLeft = std::max<T>(mLeft, other.mLeft);
		mRight = std::min<T>(mRight, other.mRight);
		mTop = std::max<T>(mTop, other.mTop);
		mBottom = std::min<T>(mBottom, other.mBottom);		
	}

	Quad<T> Bound(const Quad<T>& other)
	{
		// todo implement
		assert(false);
	}

	Quad<T> GetBound(const Quad<T>& other) const
	{
		Quad<T> temp = *this;
		temp.Bound(other);
		return temp;
	}
	
	Vector<T, 2> GetOffset() const
	{
		return Vector<T, 2>(mLeft, mTop);
	}

	Vector<T, 2> GetCenter() const
	{
		return Vector<T, 2>((mLeft+mRight)/T(2), (mTop+mBottom)/T(2));
	}


	Vector<T, 2> GetSpan() const
	{
		return Vector<T, 2>(mRight-mLeft, mBottom-mTop);
	}

	void Set(const Quad<T>& other)
	{
		mLeft = other.mLeft;
		mTop = other.mTop;
		mBottom = other.mBottom;
		mRight = other.mRight;
	}

	Quad<T> Enclose(const Vector<T, 2>& other)
	{
		mLeft = std::min<T>(mLeft, other.x);
		mRight = std::max<T>(mRight, other.x);
		mTop = std::min<T>(mTop, other.y);
		mBottom = std::max<T>(mBottom, other.y);
		return *this;
	}

	bool Contains(const Vector<T, 2>& other)
	{
		return other.x >= mLeft && other.y >= mTop && other.x < mRight && other.y < mBottom;
	}

	bool Contains(const Quad<T>& other)
	{
		return other.mLeft >= mLeft && other.mTop >= mTop && other.mRight <= mRight && other.mBottom <= mBottom;
	}

	Quad<T> Enclose(const Quad<T>& other)
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
	
	Quad<T> Expand(T inValue)
	{
		mLeft -= inValue;
		mTop -= inValue;
		mRight += inValue;
		mBottom += inValue;
		return *this;
	}

	Quad<T> Expand(const fvec2& inValues)
	{
		mLeft -= inValues.x;
		mTop -= inValues.y;
		mRight += inValues.x;
		mBottom += inValues.y;
		return *this;
	}


	Quad<T> FlipValid()
	{
		if (mLeft > mRight) std::swap(mLeft, mRight);
		if (mTop > mBottom) std::swap(mTop, mBottom);
		return *this;
	}

	bool operator==(const Quad<T>& inOther) const
	{
		return	(mLeft	== inOther.mLeft) &&
				(mRight	== inOther.mRight) &&
				(mTop	== inOther.mTop) &&
				(mBottom== inOther.mBottom);
	}


	Quad<T> operator*(const T& inOther) const
	{
		return Quad<T>(	mLeft * inOther, mTop * inOther, mRight * inOther, mBottom * inOther);
	}

	Quad<T> operator+(const Vector<T, 2>& inOffset) const
	{
		return GetTranslated(inOffset);
	}

	Quad<T> operator-(const Vector<T, 2>& inOffset) const
	{
		return GetTranslated(-inOffset);
	}


};


template <class T>
std::ostream& operator<<(std::ostream& inStream, const Quad<T>& inQuad)
{
	inStream << '{';
	inStream << inQuad.mLeft << ',';
	inStream << inQuad.mTop << ',';
	inStream << inQuad.mRight << ',';
	inStream << inQuad.mBottom;

	inStream << '}';
	return inStream;
}

typedef Quad<int> iquad;

class fquad : public Quad<float>
{
	public:
	static fquad sEmpty() { return fquad(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX); }
	static fquad sUnit()  { return fquad(-1.0f, -1.0f, 1.0f, 1.0f); };

	// copy of constructors of Quad<float>
	fquad() {}
	fquad(const float inLeft, const float inTop, const float inRight, const float inBottom) : Quad<float>(inLeft, inTop, inRight, inBottom) {}
	fquad(const float inWidth, const float inHeight) : Quad<float>(inWidth, inHeight) {}



};


