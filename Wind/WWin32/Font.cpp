#pragma once

#include <WCommon/Font.h>
#include <WWin32/Win32.h>


Font::Font() 
{}

Font::~Font()
{
	if (mHandle != 0)
	{
		DeleteObject(gHandleToHANDLE(mHandle));
		mHandle = 0;
	}

}


Font* Font::sCreateFont(const String& inFontName, float inFontsize)
{
	LOGFONT f;

	int point_size = (gRoundToInt(inFontsize) * GetDeviceCaps(GetDC(0), LOGPIXELSY)) / 72;
	f.lfHeight				= point_size;
	f.lfWidth				= 0;							// match height
	f.lfEscapement			= 0;							// straight
	f.lfOrientation			= 0;							// straight
	f.lfWeight				= 400;							// normal, non-bold
	f.lfItalic				= false;						// no italic
	f.lfUnderline			= false;						// no underline
	f.lfStrikeOut			= false;						// no strike-out
	f.lfCharSet				= DEFAULT_CHARSET;				// default character set
	f.lfOutPrecision		= OUT_DEFAULT_PRECIS;			// default precision
	f.lfClipPrecision		= CLIP_DEFAULT_PRECIS;			// default clipping
	f.lfQuality				= DEFAULT_QUALITY;				// anti-aliased (no clear-font)
	f.lfPitchAndFamily		= VARIABLE_PITCH | FF_MODERN;	// when the font is not available, grab variable-width, modern font
	memcpy_s(f.lfFaceName, 32, inFontName.GetData(), inFontName.GetLength());

	HFONT font = CreateFontIndirect(&f);
	Font* new_font = new Font();
	new_font->mName			= inFontName;
	new_font->mSize			= inFontsize;
	new_font->mHandle		= _Handle(font);
	return new_font;
}


void FontDrawer::Draw(DIB& inDib, const String& inString, const IRect& inLocation)
{
	RECT r;
	r.bottom	= inLocation.mBottom;
	r.top		= inLocation.mTop;
	r.right		= inLocation.mRight;
	r.left		= inLocation.mLeft;

	BaseString<wchar_t> wide_text(L"The quick brown fox jumped over the lazy dog");
	SelectObject(gHandleToHDC(inDib.GetHandle()), gHandleToHANDLE(mFont->GetHandle()));
	SetBkMode(gHandleToHDC(inDib.GetHandle()), TRANSPARENT);
	SetTextColor(gHandleToHDC(inDib.GetHandle()), mColor.mIntValue);
	DrawTextEx(gHandleToHDC(inDib.GetHandle()), (LPWSTR) wide_text.GetCString(), (int) wide_text.GetLength(), &r, DT_LEFT, nullptr);
}

ivec2 FontDrawer::GetTextSize(const String& inString)
{
	return ivec2(0, 0);
}


