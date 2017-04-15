// ------------------------------------------------------------------------------------------------------------
// Utility functions and structures describing colors in different packing formats
// ------------------------------------------------------------------------------------------------------------
#pragma once

#include <CCore/types.h>
#include <CMath/Math.h>

// ------------------------------------------------------------------------------------------------------------
// Default palette colors as defined by the 16-color EGA palette
// ------------------------------------------------------------------------------------------------------------
enum EDefaultPaletteColor
{
	dpcBlack = 0,
	dpcBlue = 1,
	dpcGreen = 2,
	dpcCyan = 3,
	dpcRed = 4,
	dpcPurple = 5,
	dpcBrown = 6,	// odd one out, should be "dark" yellow
	dpcGrey = 7,
	dpcDarkGrey = 8,	// actually light black
	dpcLightBlue = 9,
	dpcLightGreen = 10,
	dpcLightCyan = 11,
	dpcLightRed = 12,
	dpcLightPurple = 13,
	dpcYellow = 14,
	dpcWhite = 15
};

// ------------------------------------------------------------------------------------------------------------
// Gamma functions
// ------------------------------------------------------------------------------------------------------------
inline void gGammaToLinearComponent(float& ioComponent)
{
	ioComponent = (ioComponent <= 0.04045f)
		? ioComponent / 12.92f
		: gPowF((ioComponent + 0.055f) / 1.055f, 2.4f);
}

inline void gLinearToGammaComponent(float& ioComponent)
{
	ioComponent = (ioComponent <= 0.0031308f)
		? ioComponent * 12.92f
		: 1.055f * gPowF(ioComponent, 1.0f / 2.4f) - 0.055f;
}


// ------------------------------------------------------------------------------------------------------------
// default 24-bit RGB structure
// ------------------------------------------------------------------------------------------------------------
struct ColorPacking_RGB
{
public:
	uint8 mRed;
	uint8 mGreen;
	uint8 mBlue;
};

// ------------------------------------------------------------------------------------------------------------
// default 32-bit RGBA structure
// ------------------------------------------------------------------------------------------------------------
struct ColorPacking_RGBA
{
public:
	union
	{
		ColorPacking_RGB mColors;
		struct
		{
			uint8 mRed;
			uint8 mGreen;
			uint8 mBlue;
			uint8 mAlpha;
		};
		uint32 mIntValue;
	};
};


// ------------------------------------------------------------------------------------------------------------
// default 24-bit BGR structure
// ------------------------------------------------------------------------------------------------------------
struct ColorPacking_BGR
{
public:

	union
	{
		struct
		{
			uint8 mBlue;
			uint8 mGreen;
			uint8 mRed;
		};
		uint32 mIntValue;
	};

};

// ------------------------------------------------------------------------------------------------------------
// default 32-bit BGRA structure
// ------------------------------------------------------------------------------------------------------------
struct ColorPacking_BGRA
{
public:

	union
	{
		ColorPacking_BGR mColors;

		struct
		{
			uint8 mBlue;
			uint8 mGreen;
			uint8 mRed;
			uint8 mAlpha;
		};
		uint32 mIntValue;
	};
};


// ------------------------------------------------------------------------------------------------------------
// floating point RGBA structure; standard range is 0.0 - 1.0
// ------------------------------------------------------------------------------------------------------------
struct ColorPacking_RGBAf
{
public:
	struct
	{
		float mRed;
		float mGreen;
		float mBlue;
		float mAlpha;
	};
};


// ------------------------------------------------------------------------------------------------------------
// Functional wrapper for integer (8-bits) colors
// It encapsulates the T_Colorpacking struct with char
// ------------------------------------------------------------------------------------------------------------
template<typename T_ColorPacking>
class IntColor : public T_ColorPacking
{
public:
	IntColor(const uint8& inRed, const uint8& inGreen, const uint8& inBlue, const uint8& inAlpha)		{ mBlue = inBlue; mGreen = inGreen; mRed = inRed; mAlpha = inAlpha; }
	IntColor(const uint8& inRed, const uint8& inGreen, const uint8& inBlue)								{ mBlue = inBlue; mGreen = inGreen; mRed = inRed; }
	explicit IntColor(const uint32 inIntValue)															{ mIntValue = inIntValue; }
	IntColor() {}
	IntColor(const T_ColorPacking& inBase) { *(T_ColorPacking*)this = inBase; }

	template <class T_ColorType>	bool	Equals(const T_ColorType& inOther)					const	{ return (mBlue == inOther.mBlue && mGreen == inOther.mGreen && mRed == inOther.mRed && mAlpha == inOther.mAlpha); }
	template <class T_ColorType>	bool	EqualsIgnoreAlpha(const T_ColorType& inOther)		const	{ return (mBlue == inOther.mBlue && mGreen == inOther.mGreen && mRed == inOther.mRed); }
	template <class T_ColorType>	void	Add(const T_ColorType& other)								{ mRed += other.mRed; mBlue += other.mBlue; mGreen += other.mGreen; }
	template <class T_ColorType>	bool	operator==(const T_ColorType& inOther)				const	{ return Equals(inOther); }

	template <class T_ColorType>
	void AddSaturate(const T_ColorType& other)
	{
		uint32 temp = mRed + other.mRed;
		mRed = (temp > 0xFF ? 0xFF : temp);
		temp = mBlue + other.mBlue;
		mBlue = (temp > 0xFF ? 0xFF : temp);
		temp = mGreen + other.mGreen;
		mGreen = (temp > 0xFF ? 0xFF : temp);
	}

	static IntColor<T_ColorPacking> sGrayScale(const uchar& inGrayscale)								{ return IntColor<T_ColorPacking>(inGrayscale, inGrayscale, inGrayscale); }
	
	const IntColor GetScaled(float f) const
	{
		IntColor temp;
		temp.mRed = uint8(float(mRed) * f + 0.5f);
		temp.mGreen = uint8(float(mGreen) * f + 0.5f);
		temp.mBlue = uint8(float(mBlue) * f + 0.5f);
		return temp;
	}

	void Scale(float f) // fast
	{
		mRed = uint8(float(mRed) * f + 0.5f);
		mGreen = uint8(float(mGreen) * f + 0.5f);
		mBlue = uint8(float(mBlue) * f + 0.5f);
	}

	void ScaleSaturate(float f) // fast
	{
		float t;
		t = float(mRed) * f + 0.5f;
		mRed = uint8(t > 255.0 ? 255.0 : t);
		t = float(mGreen) * f + 0.5f;
		mGreen = uint8(t > 255.0 ? 255.0 : t);
		t = float(mBlue) * f + 0.5f;
		mBlue = uint8(t > 255.0 ? 255.0 : t);
	}

	void Invert()
	{
		mRed = 0xFF - mRed;
		mBlue = 0xFF - mBlue;
		mGreen = 0xFF - mGreen;
	}

	template <class T_ColorType>
	static IntColor sFromFloatColorIgnoreAlpha(const T_ColorType& inFloatColor)
	{
		IntColor f;
		f.mRed = uint8(gClamp<int>(int(inFloatColor.mRed * 255.0f), 0, 255));
		f.mGreen = uint8(gClamp<int>(int(inFloatColor.mGreen * 255.0f), 0, 255));
		f.mBlue = uint8(gClamp<int>(int(inFloatColor.mBlue * 255.0f), 0, 255));
		return f;
	}

	template <class T_ColorType>
	static IntColor sFromFloatColor(const T_ColorType& inFloatColor)
	{
		IntColor f;
		f.mRed = uint8(gClamp<int>(int(inFloatColor.mRed * 255.0f), 0, 255));
		f.mGreen = uint8(gClamp<int>(int(inFloatColor.mGreen * 255.0f), 0, 255));
		f.mBlue = uint8(gClamp<int>(int(inFloatColor.mBlue * 255.0f), 0, 255));
		f.mAlpha = uint8(gClamp<int>(int(inFloatColor.mAlpha * 255.0f), 0, 255));
		return f;
	}


	static IntColor sInterpolate(const IntColor& inA, const IntColor& inB, float i)
	{
		IntColor temp = inA;
		temp.Scale(1.0f - i);
		IntColor tempb = inB;
		tempb.Scale(i);
		temp.Add(tempb);
		return temp;
	}

	static IntColor sCreateDefaultPaletteColor(EDefaultPaletteColor inEntry)
	{
		switch (inEntry)
		{
		case  0: return IntColor(0x00, 0x00, 0x00);
		case  1: return IntColor(0x00, 0x00, 0xAA);
		case  2: return IntColor(0x00, 0xAA, 0x00);
		case  3: return IntColor(0x00, 0xAA, 0xAA);
		case  4: return IntColor(0xAA, 0x00, 0x00);
		case  5: return IntColor(0xAA, 0x00, 0xAA);
		case  6: return IntColor(0xAA, 0x55, 0x00); // odd one out
		case  7: return IntColor(0xAA, 0xAA, 0xAA);
		case  8: return IntColor(0x55, 0x55, 0x55);
		case  9: return IntColor(0x55, 0x55, 0xFF);
		case 10: return IntColor(0x55, 0xFF, 0x55);
		case 11: return IntColor(0x55, 0xFF, 0xFF);
		case 12: return IntColor(0xFF, 0x55, 0x55);
		case 13: return IntColor(0xFF, 0x55, 0xFF);
		case 14: return IntColor(0xFF, 0xFF, 0x55);
		default:
		case 15: return IntColor(0xFF, 0xFF, 0xFF);
		}
	}


	/// hue=0..360, value=0..1, saturation=0..1
	static IntColor sFromHSV(float inHue, float inValue, float inSaturation)
	{
		float h60 = inHue / 60.0f;
		float r,g,b;
		if (h60 < 3.0f)
		{
			g = h60;
			r = 2.0f - h60;
			b = h60 - 2.0f;
		}
		else
		{
			g = 4.0f - h60;
			r = h60 - 4.0f;
			b = 6.0f - h60;
		}
		g = gClamp(g,0.0f,1.0f) * inValue * inSaturation + (1.0f - inSaturation) * inSaturation;
		r = gClamp(r,0.0f,1.0f) * inValue * inSaturation + (1.0f - inSaturation) * inSaturation;
		b = gClamp(b,0.0f,1.0f) * inValue * inSaturation + (1.0f - inSaturation) * inSaturation;
		return IntColor(uint8(r * 255.0f), uint8(g * 255.0f), uint8(b * 255.0f));
	}
};


// ------------------------------------------------------------------------------------------------------------
// Functional wrapper for float colors
// It encapsulates the T_Colorpacking struct with float values
// ------------------------------------------------------------------------------------------------------------
template<typename T_ColorPacking>
class FloatColor : public T_ColorPacking
{
public:
	FloatColor()																						{}
	FloatColor(const float& inRed, const float& inGreen, const float& inBlue, const float& inAlpha)		{ mBlue = inBlue; mGreen = inGreen; mRed = inRed; mAlpha = inAlpha; }
	FloatColor(const float& inRed, const float& inGreen, const float& inBlue)							{ mBlue = inBlue; mGreen = inGreen; mRed = inRed; mAlpha = 1.0f; }

	template <class T_ColorType>	bool	Equals(const T_ColorType& inOther)					const	{ return (mBlue == inOther.mBlue && mGreen == inOther.mGreen && mRed == inOther.mRed && mAlpha = inOther.mAlpha); }
	template <class T_ColorType>	bool	EqualsIgnoreAlpha(const T_ColorType& inOther)		const	{ return (mBlue == inOther.mBlue && mGreen == inOther.mGreen && mRed == inOther.mRed); }
	template <class T_ColorType>	void	Add(const T_ColorType& other)								{ red += other.red; blue += other.blue; green += other.green; }
	template <class T_ColorType>	bool	operator==(const T_ColorType& inOther)				const	{ return Compare(inOther); }


	template <class T_ColorType>
	void AddSaturate(const T_ColorType& other)
	{
		float temp = mRed + other.mRed;
		mRed = (temp > 1.0f ? 1.0f : temp);
		temp = mBlue + other.mBlue;
		mBlue = (temp > 1.0f ? 1.0f : temp);
		temp = mGreen + other.mGreen;
		mGreen = (temp > 1.0f ? 1.0f : temp);
	}


	template <class T_ColorType>
	float DistanceSquared(const T_ColorType& inOtherColor)
	{

		float f = inOtherColor.mRed - mRed;
		float a = f*f;
		f = inOtherColor.mBlue - mBlue;
		a += (f*f);
		f = inOtherColor.mGreen - mGreen;
		a += (f*f);
		return a;
	}


	template <class T_ColorType>
	void AddSaturateOverflow(const T_ColorType& other)
	{
		float overflow = 0;
		float temp = mRed + other.mRed;
		if (temp > 1.0f) overflow += temp - 1.0f;
		mRed = (temp > 1.0f ? 1.0f : temp);
		temp = mBlue + other.mBlue;
		if (temp > 1.0f) overflow += temp - 1.0f;
		mBlue = (temp > 1.0f ? 1.0f : temp);
		temp = mGreen + other.mGreen;
		if (temp > 1.0f) overflow += temp - 1.0f;
		mGreen = (temp > 1.0f ? 1.0f : temp);
		if (overflow > 0)
		{
			overflow *= 0.3333f;
			AddSaturate(T_ColorType(overflow, overflow, overflow));
		}
	}


	template <class T_ColorType>
	static const FloatColor sFromIntColorIgnoreAlpha(const T_ColorType& inIntColor)
	{
		FloatColor f;
		f.mRed = float(inIntColor.mRed) / 255.0f;
		f.mGreen = float(inIntColor.mGreen) / 255.0f;
		f.mBlue = float(inIntColor.mBlue) / 255.0f;
		return f;
	}


	template <class T_ColorType>
	static const FloatColor sFromIntColor(const T_ColorType& inIntColor)
	{
		FloatColor f;
		f.mRed = float(inIntColor.mRed) / 255.0f;
		f.mGreen = float(inIntColor.mGreen) / 255.0f;
		f.mBlue = float(inIntColor.mBlue) / 255.0f;
		f.mAlpha = float(inIntColor.mAlpha) / 255.0f;
		return f;
	}


	void GammaToLinear()
	{
		gGammaToLinearComponent(mRed);
		gGammaToLinearComponent(mBlue);
		gGammaToLinearComponent(mGreen);
	}


	void LinearToGamma()
	{
		gLinearToGammaComponent(mRed);
		gLinearToGammaComponent(mBlue);
		gLinearToGammaComponent(mGreen);
	}


	void Scale(float f) // fffffast
	{
		mRed *= f;
		mBlue *= f;
		mGreen *= f;
	}


	void Invert()
	{
		mRed = 1.0f - mRed;
		mBlue = 1.0f - mBlue;
		mGreen = 1.0f - mGreen;
	}


	static FloatColor sCreateDefaultPaletteColor(EDefaultPaletteColor inEntry)
	{
		switch (inEntry)
		{
		case  0: return FloatColor(0.00f, 0.00f, 0.00f);
		case  1: return FloatColor(0.00f, 0.00f, 0.67f);
		case  2: return FloatColor(0.00f, 0.67f, 0.00f);
		case  3: return FloatColor(0.00f, 0.67f, 0.67f);
		case  4: return FloatColor(0.67f, 0.00f, 0.00f);
		case  5: return FloatColor(0.67f, 0.00f, 0.67f);
		case  6: return FloatColor(0.67f, 0.33f, 0.00f); // odd one out
		case  7: return FloatColor(0.67f, 0.67f, 0.67f);
		case  8: return FloatColor(0.33f, 0.33f, 0.33f);
		case  9: return FloatColor(0.33f, 0.33f, 1.00f);
		case 10: return FloatColor(0.33f, 1.00f, 0.33f);
		case 11: return FloatColor(0.33f, 1.00f, 1.00f);
		case 12: return FloatColor(1.00f, 0.33f, 0.33f);
		case 13: return FloatColor(1.00f, 0.33f, 1.00f);
		case 14: return FloatColor(1.00f, 1.00f, 0.33f);
		default:
		case 15: return FloatColor(1.00f, 1.00f, 1.00f);
		}
	}
};


