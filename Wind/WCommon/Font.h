#pragma once

#include <CCore/types.h>
#include <CCore/RefCount.h>
#include <WCommon/DIB.h>



class Font : public RefObject<Font>
{
public:
	Font();
	~Font();

	static Font*	sCreateFont(const String& inFontName, float inFontsize);
	String			GetName() const								{ return mName; }
	float			GetSize() const								{ return mSize; }
	_Handle			GetHandle() const							{ return mHandle; }
private:
	String			mName;
	float			mSize		= 0.0f;
	_Handle			mHandle	= nullptr;
};

class FontDrawer
{
public:
	void			SetFont(const Font* inFont)					{ mFont = inFont; }
	const Font&		GetFont() const								{ return *mFont; }
	void			SetColor(const DIBColor& inColor)			{ mColor = inColor; }

	void			Draw(DIB& inDib, const String& inString, const IRect& inLocation);
	ivec2			GetTextSize(const String& inString);

private:
	DIBColor			mColor;
	RefPtr<const Font>	mFont;
};

