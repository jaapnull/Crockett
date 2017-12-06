#pragma once

#include <CCore/Types.h>
#include <CCore/String.h>
#include <CCore/Frame.h>



bool sEvalChar(uint16 inChar, uint x, uint y)
{
	return (uint32(inChar) & (1 << ((y - (inChar >> 15)) * 3 + x))) != 0;
}

template<typename T_Pixel, uint T_Scale = 1>
void gDrawDebugFontChar(uchar inChar, BaseFrame<T_Pixel>& inFrame, const ivec2& inOffset, const T_Pixel& inColor)
{

	const uchar f_start = ' ';
	const uchar f_end = '~';
	const uint f_count = f_end - f_start + 1;
	if (inChar < f_start || inChar > f_end) inChar = f_start;

	static uint16 debug_chars[f_count] = 
	{
	0x0000, /* */ 0x2092, /*!*/ 0x002d, /*"*/ 0x5f7d, /*#*/ 0x7cfa, /*$*/ 0x52a5, /*%*/ 0x777f, /*&*/ 0x0012, /*'*/
	0x224a, /*(*/ 0x3493, /*)*/ 0x2eba, /***/ 0x05d0, /*+*/ 0x3400, /*,*/ 0x01c0, /*-*/ 0x2000, /*.*/ 0x12a4, /*/*/
	0x7b78, /*0*/ 0x2498, /*1*/ 0x7338, /*2*/ 0x79b8, /*3*/ 0x4f68, /*4*/ 0x7878, /*5*/ 0x7e78, /*6*/ 0x2538, /*7*/
	0x7bf0, /*8*/ 0x79f8, /*9*/ 0x2010, /*:*/ 0x3410, /*;*/ 0x4454, /*<*/ 0x0e38, /*=*/ 0x1511, /*>*/ 0x21a7, /*?*/
	0x62fe, /*@*/ 0x5f6a, /*A*/ 0x3beb, /*B*/ 0x624e, /*C*/ 0x3b6b, /*D*/ 0x72cf, /*E*/ 0x12cf, /*F*/ 0x7b4f, /*G*/
	0x5bed, /*H*/ 0x7497, /*I*/ 0x3b27, /*J*/ 0x5aed, /*K*/ 0x7249, /*L*/ 0x5bfd, /*M*/ 0x5b6f, /*N*/ 0x7b6f, /*O*/
	0x13ef, /*P*/ 0x6f6f, /*Q*/ 0x5aeb, /*R*/ 0x388e, /*S*/ 0x2497, /*T*/ 0x6b6d, /*U*/ 0x2b6d, /*V*/ 0x5fed, /*W*/
	0x5aad, /*X*/ 0x39ad, /*Y*/ 0x72a7, /*Z*/ 0x324b, /*[*/ 0x4889, /*\*/ 0x3493, /*]*/ 0x002a, /*^*/ 0x7000, /*_*/
	0x0011, /*`*/ 0x7b80, /*a*/ 0x3ac8, /*b*/ 0x6380, /*c*/ 0x7ba0, /*d*/ 0x6750, /*e*/ 0x2ca0, /*f*/ 0xe9aa, /*g*/
	0x5ac8, /*h*/ 0x2410, /*i*/ 0x9482, /*j*/ 0x5740, /*k*/ 0x3240, /*l*/ 0x5fc0, /*m*/ 0x5ac0, /*n*/ 0x7bc0, /*o*/
	0x9f58, /*p*/ 0xcf70, /*q*/ 0x12c0, /*r*/ 0x3580, /*s*/ 0x22c8, /*t*/ 0x6b40, /*u*/ 0x2b40, /*v*/ 0x7f40, /*w*/
	0x5540, /*x*/ 0x9568, /*y*/ 0x64c0, /*z*/ 0x44d4, /*{*/ 0x2492, /*|*/ 0x1591, /*}*/ 0x00A8  /*~*/
	};

	for (uint y = 0; y < 6; y++)
	for (uint x = 0; x < 3; x++)
	{
		if (sEvalChar(debug_chars[inChar - f_start], x, y))
		{
			for (int sy = 0; sy < T_Scale; sy++)
			for (int sx = 0; sx < T_Scale; sx++)
				inFrame.SetSafe(inOffset.x + (T_Scale * x) + sx, inOffset.y + T_Scale * (y) + sy, inColor);
		}
	}
}


template<typename T_Pixel, uint inSize = 1>
void gDrawDebugFontText(const String& inText, BaseFrame<T_Pixel>& inFrame, const ivec2& inOffset, const T_Pixel& inColor)
{
	const uint font_spacing_h = 4 * inSize;
	const uint font_spacing_v = 9 * inSize;
	ivec2 cursor = inOffset;
	for (char c : inText)
	{
		if (c == '\n') 
		{
			cursor.x = inOffset.x; 
			cursor.y += font_spacing_v;
		}
		else
		{
			gDrawDebugFontChar<T_Pixel, inSize>(c, inFrame, cursor, inColor);
			cursor.x += font_spacing_h;
		}
	}
}



/*
0000 2092 002d 5f7d 7cfa 52a5 777f 0012
224a 3493 2eba 05d0 3400 01c0 2000 12a4
7b78 2498 7338 79b8 4f68 7878 7e78 2538
7bf0 79f8 2010 3410 4454 0e38 1511 21a7
62fe 5f6a 3beb 624e 3b6b 72cf 12cf 7b4f
5bed 7497 3b27 5aed 7249 5bfd 5b6f 7b6f
13ef 6f6f 5aeb 388e 2497 6b6d 2b6d 5fed
5aad 39ad 72a7 324b 4889 3493 002a 7000
0011 7b80 3ac8 6380 7ba0 6750 2ca0 e9aa
5ac8 2410 9482 5740 3240 5fc0 5ac0 7bc0
9f58 cf70 12c0 3580 22c8 6b40 2b40 7f40
5540 9568 64c0 44d4 2492 1591 00A8

*/
