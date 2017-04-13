#pragma once

#include <CCore/Types.h>
#include <CCore/String.h>
#include <CCore/Frame.h>
#include <CUtils/Color.h>
#include <WCommon/Window.h>

typedef IntColor<ColorPacking_BGR> DIBColor;

class DIB : public BaseFrame<DIBColor>
{
private:
	_Handle		mBmp;

public:
	_Handle		mHdc;
				DIB() : mBmp(0), mHdc(0)										{}
				DIB(const DIB& inOther) : mBmp(0), mHdc(0), BaseFrame()			{CopyFrom(inOther);}
	DIB&		operator=(const DIB& inOther)									{CopyFrom(inOther); return *this; }
				DIB(const WString& filename);
				DIB(uint inWidth, uint inHeight);				
				~DIB();

	void		Resize(uint inWidth, uint inHeight);
	void		ResizeCopyData(uint inWidth, uint inHeight);
	bool		IsValid() const;
	bool		LoadFromFile(const WString& filename);
	void		ClearResources();
	
	void		CopyFrom(const DIB& inOther);
	void		DrawImage(int inDestX, int inDestY, const DIB& image, int inSrcX, int inSrcY, int inWidth, int inHeight);
	void		DrawImageStretched(int inDestX, int inDestY, int inDestWidth, int inDestHeight, const DIB& image, int inSrcX, int inSrcY, int inSrcWidth, int inSrcHeight);
	void		DrawImageColoredKey(int inDestX, int inDestY, const DIB& image, int inSrcX, int inSrcY, int inWidth, int inHeight, const DIBColor &key);


	_Handle		GetHandle() const { return mHdc; }
};