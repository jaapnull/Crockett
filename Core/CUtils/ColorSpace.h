#pragma once

struct YCoCg 
{
public:
	uchar mY;
	uchar mCo;
	uchar mCg;
};


template <class T_packing>
void RGB2YCoCg(YCoCg& outYCoCg, const IntColor<T_packing>& inRGB)
{
/*
NEW AND CRAP
Co = R - B
t = B + (Co >> 1)
Cg = G - t
Y = t + (Cg >> 1)
OLD AND AWESOME
Y = 1/4  1/2  1/4
Co= 1/2   0  -1/2
Cg -1/4  1/2 -1/4


*/
	int tY =    inRGB.mRed / 4 + inRGB.mGreen / 2 + inRGB.mBlue / 4;
	int tCo =  (inRGB.mRed - inRGB.mBlue) / 2;
	int tCg = - inRGB.mRed / 4 + inRGB.mGreen / 2 - inRGB.mBlue / 4;

	outYCoCg.mY = tY;
	outYCoCg.mCo = tCo;
	outYCoCg.mCg = tCg;
};	

template <class T_packing>
YCoCg YCoCg2RGB(IntColor<T_packing>& outRGB, const YCoCg& inYCoCg)
{
	uchar t			= inYCoCg.mY - inYCoCg.mCg * 2;
	outRGB.mGreen	= inYCoCg.mCg + t;
	outRGB.mBlue	= t - (inYCoCg.mCo >> 1);
	outRGB.mRed		= inYCoCg.mCo + outRGB.mBlue;
};

