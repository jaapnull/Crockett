#pragma once
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <CCore/Types.h>
#include <CCore/Assert.h>

// Define PI and some other stuff
static const float	cPi_f = 3.14159265358979323846f;
static const double cPi_d = 3.14159265358979323846;
static const float cSqrt2_f = 1.41421356237309504880f;
static const double cSqrt2_d = 1.41421356237309504880;

static const float cEpsilon = 1.192092896e-07f;	// FLT_EPSILON


// TODO move these to utils.h or something
inline float gFloatRand()
{
	return (float(rand())) / float(RAND_MAX);
}

inline float gFloatRandSymmetric()
{
	gAssert(RAND_MAX < INT_MAX);

	return (float(rand())-float(RAND_MAX/2)) / float((RAND_MAX+1)/2); // the +1 creates a shift-division
}

inline uint gCountBits(uint32 inValue)
{
	uint32 r = inValue;
	r			= (r&0x55555555) + ((r&0xAAAAAAAA) >> 1);
	r			= (r&0x33333333) + ((r&0xCCCCCCCC) >> 2);
	r			= (r&0x0F0F0F0F) + ((r&0xF0F0F0F0) >> 4);
	r			= (r&0x00FF00FF) + ((r&0xFF00FF00) >> 8);
	r			= (r&0x0000FFFF) + ((r&0xFFFF0000) >> 16);
	return r;
}

inline uint gCountBits(uint64 inValue)
{
	uint64 r = inValue;
	r			= (r&0x5555555555555555) + ((r&0xAAAAAAAAAAAAAAAA) >> 1);
	r			= (r&0x3333333333333333) + ((r&0xCCCCCCCCCCCCCCCC) >> 2);
	r			= (r&0x0F0F0F0F0F0F0F0F) + ((r&0xF0F0F0F0F0F0F0F0) >> 4);
	r			= (r&0x00FF00FF00FF00FF) + ((r&0xFF00FF00FF00FF00) >> 8);
	r			= (r&0x0000FFFF0000FFFF) + ((r&0xFFFF0000FFFF0000) >> 16);
	r			= (r&0x00000000FFFFFFFF) + ((r&0xFFFFFFFF00000000) >> 32);
	return (uint) r;
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

inline uint32 gRand()
{
	return (uint32) rand();
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

template<typename T>
inline T gSquared(const T& inValue)
{
	return inValue * inValue;
}

template<typename T>
inline T gRecp(const T& inValue)
{
	return T(1.0) / inValue;
}

inline int gRoundToNearestInt(float r)
{
	return (r > 0.0) ? int(floor(r + 0.5f)) : int(ceil(r - 0.5f)); 
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

inline bool gIsNear(float inA, float inB)
{
	float e = gMax(inA, inB) * cEpsilon;
	return gAbs(inA-inB) <= e;
}
