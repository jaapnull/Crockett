#include <PCHWind.h>
#include <WCommon/DIB.h>
#include <WWin32/Win32.h>
// helper functions
static void sFillHeader(BITMAPINFOHEADER& header, unsigned int pwidth, unsigned int pheight)
{
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = pwidth;
	header.biHeight = pheight;
	header.biPlanes = 1;
	header.biBitCount = sizeof(DIBColor)* 8;
	header.biCompression = BI_RGB; 
	header.biSizeImage = 0; 
	header.biXPelsPerMeter = 200; 
	header.biYPelsPerMeter = 200; 
	header.biClrUsed = 0;
	header.biClrImportant = 0; 
}

DIB::DIB(uint inWidth, uint inHeight)
{
	Resize(inWidth, inHeight);
}

DIB::~DIB(void)
{
	ClearResources();
}

void DIB::ClearResources()
{
	if (mBmp) DeleteObject(gHandleToHBITMAP(mBmp));
	mBmp = 0;
	if (mHdc) DeleteDC(gHandleToHDC(mHdc));
	mHdc = 0;
	BaseFrame::Resize(0, 0); // makes sure that a resize to former size retriggers resource generation
}


void DIB::CopyFrom(const DIB& inOther)
{	
	Resize(inOther.GetWidth(), inOther.GetHeight());
	DrawImage(0,0,inOther, 0,0,GetWidth(), GetHeight());
	
}


bool DIB::LoadFromFile(const WString& filename)
{
	ClearResources();
	HDC tdc = GetDC(0);
	HBITMAP phbmp = (HBITMAP)LoadImage(0, filename.GetCString(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);//CreateDIBSection(tdc, &bi, DIB_RGB_COLORS, (void**) &bitmap, 0, 0);
	if (!phbmp) { assert(false);  ClearResources(); return false; } // loadimage can fail so easily and should be captured, we give proper exit		
	BITMAP bm;
	BOOL ret = GetObject(phbmp, sizeof(BITMAP), &bm);
	assert(ret);
	Resize(bm.bmWidth, bm.bmHeight);
	assert(sizeof(DIBColor) == 3 || sizeof(DIBColor) == 4);

	HDC tempdc = CreateCompatibleDC(gHandleToHDC(mHdc));
	assert(tempdc);
	HGDIOBJ c = SelectObject(tempdc, phbmp);
	assert(c);

	BOOL b = BitBlt(gHandleToHDC(mHdc), 0, 0, GetWidth(), GetHeight(), tempdc, 0, 0, SRCCOPY);
	assert(b);
	b = DeleteObject(phbmp);
	assert(b);
	return true;
}

DIB::DIB(const WString& filename)
{
	LoadFromFile(filename);
}

bool DIB::IsValid() const
{
	return mBmp != 0 && mHdc!= 0 && GetBaseData();
}

void DIB::Resize(uint inWidth, uint inHeight)
{
	if (inWidth == GetWidth() && inHeight == GetHeight()) return;
	//if (hbmp) DeleteObject(hbmp);
	ClearResources();
	if (inWidth == 0 || inHeight == 0) return; // invalid bmp
	int pitch = ((int)inWidth)*sizeof(DIBColor);

	// make sure it is 32-bits "aligned"
	if (pitch % 4 != 0) pitch += (4 - pitch % 4);

	BaseFrame::Resize(inWidth, inHeight, -pitch);

	BITMAPINFO bi;
	sFillHeader(bi.bmiHeader, GetWidth(), GetHeight());

	DIBColor* bitmap;
	HDC tdc = GetDC(0);
	HBITMAP hbmp = CreateDIBSection(tdc, &bi, DIB_RGB_COLORS, (void**)&bitmap, 0, 0);
	assert(hbmp);
	HDC hdc = CreateCompatibleDC(tdc);
	assert(hdc);
	HGDIOBJ b = SelectObject(hdc, hbmp);
	SetStretchBltMode(hdc, COLORONCOLOR);	// creates proper stretchblt
	assert(b);
	ReleaseDC(0, tdc);

	mBmp = _Handle(hbmp);
	mHdc = _Handle(hdc);

	BaseFrame::SetBaseData(bitmap);
}

void DIB::DrawImage(int inDestX, int inDestY, const DIB& image, int inSrcX, int inSrcY, int inWidth, int inHeight)
{
	assert(image.IsValid());
	HRESULT b;
	b = BitBlt(gHandleToHDC(GetHandle()), inDestX, inDestY, inWidth, inHeight, gHandleToHDC(image.GetHandle()), inSrcX, inSrcY, SRCCOPY);
	assert(b);
}

void DIB::DrawImageColoredKey(int inDestX, int inDestY, const DIB& image, int inSrcX, int inSrcY, int inWidth, int inHeight, const DIBColor &inKey)
{
	int x = inDestX;
	int y = inDestY;
	int width = inWidth;
	int height = inHeight;
	// clamp when dest < 0; reduce width and offset src
	if (x < 0)							{ width += x; inSrcX -= x; x = 0; }
	if (y < 0)							{ height += y; inSrcY -= y; y = 0; }

	if (x + width > int(GetWidth()))	{ width -= (x + width - GetWidth()); }
	if (y + height > int(GetHeight()))	{ height -= (y + height - GetHeight()); }
	assert(y + height <= int(GetHeight()));

	if (height < 0 || width < 0) return;
	
	// left upper corner
	DIBColor* src = (DIBColor*)(&(image.Get(inSrcX, inSrcY)));
	DIBColor* dst = (DIBColor*)(&(Get(x, y)));
	
	for (int sl = 0; sl < height; sl++)
	{
		for (int c = 0; c < width; c++)
		{
			if ((src[c].mIntValue & 0xFFFFFF) != (inKey.mIntValue & 0xFFFFFF))
			{
				dst[c].mIntValue = src[c].mIntValue; //+= 0x001F1F1F;// = src[c];
				//dst[c].mIntValue += 0x000008;// (src[c].mIntValue & 0xFCFCFCFC) >> 2;//  0x001F1F1F;// = src[c];
			}
		}
		
		//memcpy(dst, src, (width) * sizeof(DIBColor));
		src = (DIBColor*) ((byte*)src + image.GetPitch());
		dst = (DIBColor*)((byte*)dst + GetPitch());
	}
}


void DIB::DrawImageStretched(int inDestX, int inDestY, int inDestWidth, int inDestHeight, const DIB& image, int inSrcX, int inSrcY, int inSrcWidth, int inSrcHeight)
{
	assert(image.IsValid());
	HRESULT b;
	b = StretchBlt(gHandleToHDC(GetHandle()), inDestX, inDestY, inDestWidth, inDestHeight, gHandleToHDC(image.GetHandle()), inSrcX,inSrcY, inSrcWidth, inSrcHeight, SRCCOPY);
	assert(b);
}
