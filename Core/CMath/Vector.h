#pragma once
#include <stdarg.h>
#include <CCore/Assert.h>

template <class T, int I> class BaseVector
{
	T mField[I];

	inline void Fill(const T* data)
	{
		for (unsigned int x = 0; x < I; x++)
		mField[x] = data[x];
	}

};

template <class T> class BaseVector<T, 1>
{
public:
	union
	{
		T x;
		T mField[1];
	};

	inline void Fill(const T* data)
	{
		x = data[0];
	}

	inline void Fill(const T& px)
	{
		x = px;
	}
};

template <class T> class BaseVector<T, 2>
{
public:
	union
	{
		struct{T x; T y;};
		T mField[2];
	};

	inline void Fill(const T* data)
	{
		x = data[0];
		y = data[1];
	}

	inline void Fill(const T& px, const T& py)
	{
		x = px;
		y = py;
	}


	T GetPerpDot(const BaseVector<T, 2>& other) const
	{
		return y*other.x - x*other.y;
	}
};

template <class T> class BaseVector<T, 3>
{
public:
	union
	{
		struct{T x; T y; T z;};
		BaseVector<T, 2> xy;
		T mField[3];
	};
	inline void Fill(const T& px, const T& py, const T& pz)
	{
		x = px;
		y = py;
		z = pz;
	}
	inline void Fill(const T* data)
	{
		x = data[0];
		y = data[1];
		z = data[2];
	}

};

template <class T> class BaseVector<T, 4>
{
public:
	union
	{
		struct{T x; T y; T z; T w;};
		BaseVector<T, 2> xy;
		BaseVector<T, 3> xyz;

		T mField[4];
	};
	inline void Fill(const T& px, const T& py, const T& pz, const T& pw)
	{
		x = px;
		y = py;
		z = pz;
		w = pw;
	}
	inline void Fill(const T* data)
	{
		x = data[0];
		y = data[1];
		z = data[2];
		w = data[3];
	}
};


template <class T, unsigned int i>
class Vector : public BaseVector<T,i>
{
public:

	const T& Get(unsigned int n) const
	{
		gAssert(n < i);
		return mField[n];
	}

	unsigned int GetSize() const
	{
		return i;
	}

	T& Get(unsigned int n)
	{
		gAssert(n < i);
		return mField[n];
	}

	template <class S>
	explicit Vector(const Vector<S, i>& other)
	{		
		for (int x = 0; x < i; x++)
			mField[x] = (T) other.mField[x];
	}

	Vector(const T& x, const T& y, const T& z, const T& w)
	{
		gAssert(i >= 4);
		mField[0] = x; mField[1] = y; mField[2] = z; mField[3] = w;
	}

	explicit Vector(const BaseVector<T, 3>& inXyz, const T& w)
	{
		gAssert(i >= 4);
		mField[0] = inXyz.x; mField[1] = inXyz.y; mField[2] = inXyz.z; mField[3] = w;
	}

	explicit Vector(const T& x, const T& y, const T& z)
	{
		gAssert(i >= 3);
		mField[0] = x; mField[1] = y; mField[2] = z;
	}

	explicit Vector(const BaseVector<T, 2>& inXy, const T& z)
	{
		gAssert(i >= 3);
		mField[0] = inXy.x; mField[1] = inXy.y; mField[2] = z;
	}

	explicit Vector(const T& x, const T& y)
	{
		gAssert(i >= 2);
		mField[0] = x; mField[1] = y;
	}

	explicit Vector(const T& x)
	{
		gAssert(i >= 1);
		mField[0] = x;
	}

	BaseVector<T, i>& AddScalar(const T& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] += other.mField[x];
		}
		return *this;
	}

	bool IsNear(const BaseVector<T, i>& other) const
	{
		for (unsigned int x = 0; x < i; x++)
			if (!gIsNear(other.mField[x], mField[x])) return false;
		return true;
	}

	T GetDot(const BaseVector<T, i>& other) const
	{
		T accum = 0;
		for (unsigned int x = 0; x < i; x++)
		{
			accum += other.mField[x] * mField[x];
		}
		return accum;
	}

	T GetLengthSquared() const
	{
		T accum = 0;
		for (unsigned int x = 0; x < i; x++)
		{
			accum += mField[x]*mField[x];
		}
		return accum;
	}

	float GetLength() const
	{
		T accum = 0;
		for (unsigned int x = 0; x < i; x++)
		{
			accum += mField[x]*mField[x];
		}
		return sqrtf((float)accum);
	}

	void Normalize()
	{
		float f = GetLength();
		(*this)/=f;
	}

	Vector<T,i> GetNormalised() const
	{
		float f = GetLength();
		return (*this) / f;
	}

	Vector<T, i> GetSafeNormalised() const
	{
		float f = GetLength();
		return f == 0 ? Vector::sZero() : (*this) / f;
	}

	bool operator==(const BaseVector<T,i>& other) const
	{
		for (unsigned int x = 0; x < i; x++)
		{
			if (mField[x] != other.mField[x]) return false;
		}
		return true;
	}

	bool operator!=(const BaseVector<T,i>& other) const
	{
		for (unsigned int x = 0; x < i; x++)
		{
			if (mField[x] != other.mField[x]) return true;
		}
		return false;
	}

	Vector<T,i>& operator+=(const BaseVector<T,i>& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] += other.mField[x];
		}
		return *this;
	}

	Vector<T,i>& operator-=(const BaseVector<T,i>& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] -= other.mField[x];
		}
		return *this;
	}

	Vector<T,i>& operator/=(const T& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] /= other;
		}
		return *this;
	}

	Vector<T,i>& operator/=(Vector<T,i>& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] /= other.mField[x];
		}
		return *this;
	}

	Vector<T,i> operator-() const
	{
		Vector<T,i> temp;
		for (unsigned int x = 0; x < i; x++)
			temp.mField[x] = -mField[x];
		return temp;
	}


	T& operator[](unsigned int n)
	{
		return Get(n);
	}

	const T& operator[](unsigned int n) const
	{
		return Get(n);
	}

	Vector<T,i>& operator*=(const T& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] *= other;
		}
		return *this;
	}

		void SetZero()
	{
		for (unsigned int x = 0; x < i; x++) mField[x] = 0;
	}

	Vector<T,i> GetMultiply(const Vector<T,i>& other) const
	{
		Vector<T, i> v = *this;
		for (unsigned int x = 0; x < i; x++)
		{
			v[x] *= other[x];
		}
		return v;
	}

	Vector<T,i>& Multiply(const Vector<T,i>& other)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] *= other[x];
		}
		return *this;
	}


	Vector<T,i+1> ExtendedWith(const T& inExtension) const
	{
		Vector<T,i+1> temp;
		for (int c = 0; c < i; c++)
		{
			temp[c] = mField[c];
		}
		temp[i] = inExtension;
		return temp;
	}


	template <class S>
	Vector<T,i> operator=(const Vector<S, i>& other)
	{		
		for (int x = 0; x < i; x++)
			mField[x] = (T) other.mField[x];
		return *this;
	}


	void Lerp(const Vector<T,i>& other, float inValue)
	{
		for (unsigned int x = 0; x < i; x++)
		{
			mField[x] *= (1.0f - inValue);
			mField[x] += other.mField[x] * inValue;
		}
	}

	Vector<T,i> operator*(const T& other) const
	{
		Vector<T,i> v = *this;
		v*= other;
		return v;
	}
	Vector<T,i> operator/(const T& other) const
	{
		Vector<T,i> v = *this;
		v/= other;
		return v;
	}
	Vector<T,i> operator+(const BaseVector<T,i>& other) const
	{
		Vector<T,i> v = *this;
		v+= other;
		return v;
	}
	Vector<T,i> operator-(const BaseVector<T,i>& other) const
	{
		Vector<T,i> v = *this;
		v-= other;
		return v;
	}

	Vector<T,i>& operator=(const BaseVector<T,i>& other)
	{
		for (unsigned int x = 0; x < i; x++)
			mField[x] = other.mField[x];
	}


	Vector()
	{
	}

	template <class T2>
	Vector(const BaseVector<T2,i>& other)
	{
		for (unsigned int x = 0; x < i; x++)
			mField[x] = (T) other.mField[x];
	}


	template <class T2>
	void Set(const BaseVector<T2,i>& other)
	{
		mField[0] = 1;
		for (unsigned int x = 0; x < i; x++)
			mField[x] = (T) other.mField[x];
	}


	static const Vector<T, i> sZero()
	{
		Vector<T, i> v;
		v.SetZero();
		return v;
	}

};


class ivec2 : public Vector<int, 2>
{
public:
	ivec2() {}
	ivec2(const Vector<int, 2>& inBase) : Vector<int, 2>(inBase) {}
	ivec2(const Vector<float, 2>& inBase) : Vector<int, 2>((int)inBase.x, (int) inBase.y) {}
	ivec2(int inX, int inY) : Vector<int,2>(inX, inY) {}

	const ivec2 GetPerp() const { return ivec2(y, -x); }
};

typedef Vector<int, 3> ivec3;
typedef Vector<int, 4> ivec4;


class fvec2 : public Vector<float, 2>
{
public:
	fvec2() {}
	fvec2(const Vector<float, 2>& inBase) : Vector<float, 2>(inBase)							{}
	fvec2(const Vector<int, 2>& inBase) : Vector<float, 2>((float)inBase.x, (float) inBase.y)	{}	
	fvec2(float inX, float inY) : Vector<float,2>(inX, inY)										{}

	const fvec2 GetPerp() const																	{ return fvec2(y, -x); }
	float GetCross(const fvec2 inOther)															{ return x*inOther.y - inOther.x*y; }
};


class fvec3 : public Vector<float, 3>
{

	fvec3() {}
	fvec3(const Vector<float, 3>& inBase) : Vector<float, 3>(inBase) {}
	fvec3(const Vector<int, 3>& inBase) : Vector<float, 3>((float)inBase.x, (float) inBase.y) {}	
	fvec3(float inX, float inY, float inZ) : Vector<float,3>(inX, inY, inZ) {}

	fvec3 GetCross(const fvec3& inOther)														{ return fvec3(y*inOther.z - inOther.y*z, z*inOther.x - inOther.z*x, x*inOther.y - inOther.x*y); }
};

typedef Vector<float, 4> fvec4;

