#pragma once
#include <algorithm>
#include "../Core/Vector.h"

template <class T, unsigned int iR, unsigned int iC>
class Matrix
{
private:



	typedef Matrix<T, 1, iC> Row;
	typedef Matrix<T, iR, 1> Column;	

public:

	T mCells[iR][iC];

	const T& Get(unsigned int r, unsigned int c) const
	{
		return mCells[r][c];
	}

	T& Get(unsigned int r, unsigned int c)
	{
		return mCells[r][c];
	}
	
	Matrix<T, iR, iC>& SetZero()
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				mCells[r][c] = 0;
		return *this;
	}
	
	Matrix<T, iR, iC>& SetIdentity()
	{
		SetZero();
		for (unsigned int x = 0; x < std::min(iC, iR); x++)
			mCells[x][x] = 1;
		return *this;
	}
	
	Matrix<T, iC, iR> GetTranspose() const
	{
		Matrix<T, iC, iR> result;
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				result.Get(c,r) = Get(r,c);
		return result;
	}

	Matrix<T, iR-1, iC-1> GetMinor(int x, int y) const
	{
		// before the minor, upper left
		Matrix<T, iR-1, iC-1> temp;
		for (int r = 0; r < x; r++)
		for (int c = 0; c < y; c++)
			temp.mCells[r][c] = mCells[r][c];

		// upper-right corner
		for (int r = x + 1; r < iR; r++)
			for (int c = 0; c < y; c++)
				temp.mCells[r-1][c] = mCells[r][c];

		// lower left corner
		for (int r = 0; r < x; r++)
			for (int c = y + 1; c < iC; c++)
				temp.mCells[r][c-1] = mCells[r][c];

		// right-lower corner
		for (int r = x + 1; r < iR; r++)
			for (int c = y + 1; c < iC; c++)
				temp.mCells[r-1][c-1] = mCells[r][c];
		return temp;
	}

	
	template <unsigned int oC>
	Matrix<T, iR, oC> Multiply(const Matrix<T, iC, oC>& other) const
	{
		Matrix<T, iR, oC> result;
		result.SetZero();
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
			{				
				for (unsigned int x = 0;	x < iC; x++)
					result.Get(r,c) += Get(r,x) * other.Get(x,c);
			}
		return result;
	}
	
	
	Matrix<T, iR, iC> operator-(const Matrix<T, iR, iC>& other) const
	{
		Matrix<T, iR, iC> result;
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
				result.Get(r,c) = Get(r,c) - other.Get(r,c);
		return result;
	}

	Matrix<T, iR, iC>& operator-=(const Matrix<T, iR, iC>& other)
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
				Get(r,c) -= other.Get(r,c);
		return this;
	}
	

	Matrix<T, iR, iC> operator+(const Matrix<T, iR, iC>& other) const
	{
		Matrix<T, iR, iC> result;
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
				result.Get(r,c) = Get(r,c) + other.Get(r,c);
		return result;
	}

	Matrix<T, iR, iC>& operator+=(const Matrix<T, iR, iC>& other)
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
				Get(r,c) += other.Get(r,c);
		return this;
	}
	
	Matrix<T, iR, iC> operator/(const T& other) const
	{
		Matrix<T, iR, iC> result;
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
				result.Get(r,c) = Get(r,c) / other;
		return result;
	}

	Matrix<T, iR, iC>& operator/=(const T& other)
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < oC; c++)
				Get(r,c) /= other;
		return *this;
	}
	
	Matrix<T, iR, iC> operator*(const T& other) const
	{
		Matrix<T, iR, iC> result;
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				result.Get(r,c) = Get(r,c) * other;
		return result;
	}

	Matrix<T, iR, iC>& operator*=(const T& other)
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				Get(r,c) *= other;
		return *this;
	}	


	void SetRow(unsigned int row, const T (&data)[iC])
	{
		for (unsigned int c = 0; c < iC; c++)
			Get(row,c) = data[c]; 
	}

	Column GetColumn(unsigned int column) const
	{
		Row result;
		for (unsigned int r = 0; r < iR; r++)
			result.Get(r,0) = Get(r, column);
	}

	Row GetRow(unsigned int row) const
	{
		Row result;
		for (unsigned int c = 0; c < iC; c++)
			result.Get(0,c) = Get(row, c);
	}

	void SetColumn(unsigned int column, const T (&data)[iR])
	{
		for (unsigned int r = 0; r < iR; r++)
				Get(r,column) = data[r]; 
	}

	void Set(const T (&data)[iR*iC])
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				Get(r,c) = data[r*iR+c]; 
	}

	bool operator==(const Matrix<T, iR, iC>& other)
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				if (Get(r,c) != other.Get(r,c)) return false;
		return true;
	}


	Matrix<T, iR, iC>& operator=(const Matrix<T, iR, iC>& other)
	{
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				Get(r,c) = other.Get(r,c);		
		return *this;		
	}


	template <unsigned int oC>
	Matrix<T, iR, oC>& operator*(const Matrix<T, iC, oC>& other) const
	{
		return Multiply(other);
	}

	// only works with square matrices.
	template <unsigned int oC>
	Matrix<T, iR, oC> operator*=(const Matrix<T, iC, oC>& other)
	{
		*this=Multiply(other);
		return *this;
	}
	
	Matrix(void){}
	
	~Matrix(void){}
	
	// vectors Multiplication
	void Transform(Vector<T, iC>& v) const
	{
		Vector<T, iC> ret;
		ret.SetZero();
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
			{
				ret[r] += mCells[r][c] * v[c];
			}
		v = ret;
	}

	// vectors Multiplication
	void Transform(Matrix<T, iR, iC>& v) const
	{
		assert(iR == iC); // this only works with square matrix
		Matrix<T, iR, iC> ret;
		ret.SetZero();
		for (unsigned int r = 0; r < iR; r++)
			for (unsigned int c = 0; c < iC; c++)
				for (unsigned int s = 0; s < iR; s++)
					ret.mCells[r][c] += v.mCells[r][s] * mCells[s][c];				
		v = ret;
	}

};


template <typename T, int iR, int iC>
std::ostream& operator<<(std::ostream& ioStream, const Matrix<T, iR, iC> inMat)
{
	ioStream << "{";
	for (int x = 0; x < iR; x++)
	{
		ioStream << "{";
		for (int y = 0; y < iC; y++)
		{
			ioStream << inMat.mCells[x][y];
			if (y != iC-1) ioStream << ',';
		}
		ioStream << "}\n";		
	}
	ioStream << "}\n";
	return ioStream;
}

template <typename T, int iD>
T sGetDeterminant(const Matrix<T, iD, iD>& inMatrix)
{
	T det = 0;
	assert(false); // ACCURATE BUT VERY SLOW SHOULD NOT USE
	int x = 0; // TODO: smart column/row selection? now always taking column 0
		for (int y = 0; y < iD; y++)
		{
			T accum = sGetDeterminant(inMatrix.GetMinor(x,y)) * inMatrix.mCells[x][y];
			std::cout << "piece of det: " << x << ","  << y << " " << accum << " * "  <<  inMatrix.mCells[x][y] << '\n';
			std::cout << inMatrix.GetMinor(x,y);
			if ((x+y) %2) 
				det-= accum;
			else
				det+= accum;
		}
	return det;
}

template <>
inline float sGetDeterminant<float, 1> (const Matrix<float, 1, 1>& inMatrix) 
{
	return inMatrix.mCells[0][0]; 
}

template <>
inline float sGetDeterminant<float, 2> (const Matrix<float, 2, 2>& inMatrix)
{
	return (inMatrix.mCells[0][0] * inMatrix.mCells[1][1]) - (inMatrix.mCells[1][0] * inMatrix.mCells[0][1]);
}

template <>
inline float sGetDeterminant<float, 3> (const Matrix<float, 3, 3>& inMatrix)
{
	return	(inMatrix.mCells[0][0] * inMatrix.mCells[1][1] * inMatrix.mCells[2][2]) + 
		(inMatrix.mCells[1][0] * inMatrix.mCells[2][1] * inMatrix.mCells[0][2]) + 
		(inMatrix.mCells[2][0] * inMatrix.mCells[0][1] * inMatrix.mCells[1][2]) -
		(inMatrix.mCells[0][2] * inMatrix.mCells[1][1] * inMatrix.mCells[2][0]) -
		(inMatrix.mCells[0][0] * inMatrix.mCells[1][2] * inMatrix.mCells[2][1]) -
		(inMatrix.mCells[0][1] * inMatrix.mCells[1][0] * inMatrix.mCells[2][2]);	
}

template <typename T, int iD>
Matrix<T, iD, iD> sGetInverse(const Matrix<T, iD, iD>& inMatrix)
{
	Matrix<T, iD, iD> dets;
	for (int x = 0; x < iD; x++)
	for (int y = 0; y < iD; y++)
	{
		dets.mCells[y][x] = sGetDeterminant(inMatrix.GetMinor(x,y));
	}

	// fast determinant by just resolving to first row
	T det = 0;
	for (int y = 0; y < iD; y++)
	{
		det += dets.mCells[y][0] * inMatrix.mCells[0][y]  * ((y % 2) ? -1.0f : 1.0f);
	}
	assert(det != 0); // cannot invert a 0-det matrix
	
	// making the determinant matrix into adjunct by creating signs
	for (int x = 0; x < iD; x++)
	for (int y = 0; y < iD; y++)
	{
		dets.mCells[y][x] *= (((x+y) % 2) ? -1.0f : 1.0f);
	}

	dets *= (1.0f/det);
	return dets;
}


template <typename T, int iD>
Matrix<T, iD, iD> sGetInverseOrthogonal(const Matrix<T, iD, iD>& inMatrix)
{
	return inMatrix.GetTranspose();
}

template <typename T, int iD>
Matrix<T, iD, iD> sGetInverseAffine(const Matrix<T, iD, iD>& inMatrix)
{
	
	// Get and invert the inner rotation matrix (using GetMinor())
	Matrix<T, iD-1, iD-1> rotation_inv = sGetInverse(inMatrix.GetMinor(iD-1,iD-1));
	// Fill the translation vector
	Vector<T, iD-1> translation;
	for (int x = 0; x < iD-1; x++) translation.Get(x) = inMatrix.Get(x, iD-1);
	// transform and negate translation vector
	rotation_inv.Transform(translation);
	translation*= (T) -1;
	
	// fill everything into the final matrix
	Matrix<T, iD, iD> dets;
	for (int x = 0; x < iD-1; x++)
	{
		dets.Get(x, iD-1) = translation.Get(x);
		dets.Get(iD-1, x) = (T) 0;
		for (int y = 0; y < iD-1; y++)
			dets.Get(x,y) = rotation_inv.Get(x,y);
	}
	dets.Get(iD-1, iD-1) = (T) 1;
	return dets;
}


class Matrix44 : public Matrix<float, 4, 4>
{
public:
	Matrix44(const Matrix<float, 4, 4>& inMatrix) : Matrix(inMatrix) {}
	Matrix44() {}

	static inline const Matrix44 sTranslationMatrix(const fvec3& inTranslation)
	{
		Matrix44 translate;
		translate.SetIdentity();
		translate.mCells[0][3] = inTranslation.x;
		translate.mCells[1][3] = inTranslation.y;
		translate.mCells[2][3] = inTranslation.z;
		return translate;
	}

	static inline const Matrix44 sRotationMatrixY(float inRadians)
	{
		Matrix44 rotate;
		rotate.SetIdentity();
		rotate.mCells[0][0] = cos(inRadians);
		rotate.mCells[0][2] = -sin(inRadians);
		rotate.mCells[2][0] = -rotate.mCells[0][2];
		rotate.mCells[2][2] = rotate.mCells[0][0];
		return rotate;
	}

	static inline const Matrix44 sRotationMatrixX(float inRadians)
	{
		Matrix44 rotate;
		rotate.SetIdentity();
		rotate.mCells[1][1] = cos(inRadians);
		rotate.mCells[1][2] = -sin(inRadians);
		rotate.mCells[2][1] = -rotate.mCells[1][2];
		rotate.mCells[2][2] = rotate.mCells[1][1];
		return rotate;
	}

	static inline const Matrix44 sScaleMatrix(const fvec3& inScales)
	{
		Matrix44 scale;
		scale.SetIdentity();
		scale.mCells[0][0] = inScales.x;
		scale.mCells[1][1] = inScales.y;
		scale.mCells[2][2] = inScales.z;
		return scale;
	}

	static inline const Matrix44 sRotationMatrixZ(float inRadians)
	{
		Matrix44 rotate;
		rotate.SetIdentity();
		rotate.mCells[0][0] = cos(inRadians);
		rotate.mCells[0][1] = -sin(inRadians);
		rotate.mCells[1][0] = -rotate.mCells[0][1];
		rotate.mCells[1][1] = rotate.mCells[0][0];
		return rotate;
	}

	static inline const Matrix44 sDivideByZ()
	{
		Matrix44 rotate;
		rotate.SetIdentity();
		rotate.mCells[3][3] = 0.0f; // do not use the standard w = 1 value
		rotate.mCells[3][2] = 1.0f; // copy over z to w
		rotate.mCells[2][2] = 0.0f; // z is not copied over to z
		rotate.mCells[2][3] = -1.0f; // z = -1, so afterward division by w, z will effectively be 1/-z (order will still be the same)

		return rotate;
	}


};

