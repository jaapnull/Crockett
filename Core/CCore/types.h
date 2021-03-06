#pragma once
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <float.h>


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

constexpr size64			cMaxSize64 = size64(-1);
constexpr uint				cMaxUInt = uint(-1);
constexpr int				cMaxInt = 0x7FFFFFFF;
constexpr int				cMinInt = 0x80000000;


template<typename T> T		 gMaxValue();
template<typename T> T		 gMinValue();

template <>					inline float gMaxValue<float>()	{ return FLT_MAX; }
template <>					inline float gMinValue<float>()	{ return -FLT_MAX; }
template <>					inline int gMaxValue<int>()		{ return INT_MAX; }
template <>					inline int gMinValue<int>()		{ return INT_MIN; }


template<typename TFirst, typename TSecond>
struct Tuple
{
	Tuple(const TFirst& inFirst, const TSecond& inSecond) : mFirst(inFirst), mSecond(inSecond) {}
	Tuple() {}

	TFirst		mFirst;
	TSecond		mSecond;
};


