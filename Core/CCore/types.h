#pragma once

#include <cstdint>
#include <cstdlib>
#include <math.h>

// using abbreviated unsigned names to keep word count down and code clean
typedef unsigned int		uint;
typedef unsigned char		uchar;
typedef unsigned short		ushort;

typedef signed int			sint;
typedef signed char			schar;
typedef signed short		sshort;

// using sizes to indicate important fixed-size constructs; platform dependent (!)
typedef unsigned long long	uint64;
typedef unsigned int		uint32;
typedef unsigned short		uint16;
typedef unsigned char		uint8;

typedef signed long long	int64;
typedef signed int			int32;
typedef signed short		int16;
typedef signed char			int8;

typedef unsigned short		word;
typedef unsigned char		byte;

// large types	
typedef uintptr_t			size64;		// Size of arrays/buffers/etc
typedef uintptr_t			offset64;	// Offset within array; maximum offset between pointers


// Define PI and some other stuff
static const float	cPi_f = 3.14159265358979323846f;
static const double cPi_d = 3.14159265358979323846;
static const float cSqrt2_f = 1.41421356237309504880f;
static const double cSqrt2_d = 1.41421356237309504880;

#define gStaticAssert(e) typedef char __C_ASSERT__[(e)?1:-1]

// TODO move these to utils.h or something
inline float gFloatRand()
{
	return (float(rand())) / float(RAND_MAX);
}

inline float gFloatRandSymmetric()
{
	assert(RAND_MAX < INT_MAX);

	return (float(rand())-float(RAND_MAX/2)) / float((RAND_MAX+1)/2); // the +1 creates a shift-division
}

template <typename T>
inline const T gAbs(T inValue) 
{
	return inValue > T(0) ? inValue : -inValue;
}

inline int gRoundToInt(float inFloat)
{
	return int(roundf(inFloat));
}

inline float gCeil(float inFloat) 
{
	return ceilf(inFloat);
}

inline float gFloor(float inFloat) 
{
	return floorf(inFloat);
}

inline int gRandRange(int inMin, int inMax)
{
	return (rand()%(inMax-inMin+1)) + inMin;
}

inline int gRand()
{
	return rand();
}


inline float gPowF(float inBase, float inPower)
{
	return powf(inBase, inPower);
}

template<typename T>
inline T gClamp(const T& inValue, const T& inMin, const T& inMax)
{
	return std::max<T>(std::min(inMax, inValue), inMin);
}

inline int gRoundToNearestInt(float r)
{
	return (r > 0.0) ? int(floor(r + 0.5f)) : int(ceil(r - 0.5f)); 
}



inline void gAssert(bool inAssertion)
{
	assert(inAssertion);
}

template <typename T>
inline const T& gMin(const T& A, const T& B)
{
	return A < B ? A : B;
}

template <typename T>
inline const T& gMax(const T& A, const T& B)
{
	return A > B ? A : B;
}