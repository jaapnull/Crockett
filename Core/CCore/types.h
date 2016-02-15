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

const size64				cMaxSize64 = 0xFFFFFFFFFFFFFFFFll;