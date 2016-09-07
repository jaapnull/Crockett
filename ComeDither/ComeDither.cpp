#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>


#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif

static void sPaintPattern(BaseFrame<DIBColor>& inCanvas, const IRect& inArea, const DIBColor& inColor)
{
	DIBColor* pointer = &(inCanvas.Get(inArea.mLeft, inArea.mTop));
	DIBColor c_bg = inColor.GetScaled(gFloatRand());
	DIBColor c_stripe = inColor.GetScaled(.8f);

	for (int y = inArea.mTop; y < inArea.mBottom; y++)
	{
		for (int x = inArea.mLeft; x < inArea.mRight; x++)
		{
			DIBColor v = ((x + y) & 7) < 2 ? c_stripe : c_bg;
			pointer[x - inArea.mLeft] = v;
		}
		pointer = gOffsetPointer(pointer, inCanvas.GetPitch());
	}
}



class DControl: public RefObject<DControl>, public IMouseHandler
{
public:
	DControl(const IRect& inClientRect, DControl* inParent) : mRect(inClientRect), mParent(inParent), mDirtyRect(IRect::sEmpty()) { }
	virtual void			Draw(DIB& inCanvas)
	{
		for (RefPtr<DControl> child : mChildren)
		{
			child->Draw(inCanvas);
		}
		MarkClean();
	}
	const IRect				GetRect()							{ return mRect; }
	const IRect				GetScreenRect()						{ return mParent == nullptr ? mRect : mRect.GetTranslated(mParent->GetRect().GetMin()); }
	virtual void			MarkDirtyRect(const IRect& inRect)	
	{
		if (mDirtyRect.FullyContains(inRect))
			return;
		
		IRect overlap = inRect.GetIntersect(IRect(0,0,mRect.GetWidth(), mRect.GetHeight()));

		mDirtyRect.Enclose(overlap); 
		for (RefPtr<DControl> child : mChildren)
		{
			if (mDirtyRect.OverlapsWith(child->GetRect()))
			{
				child->MarkDirtyRect(overlap.GetTranslated(-child->GetRect().GetMin()));
			}
		}
	}
	void					MarkClean()							{ mDirtyRect.SetEmpty(); }
	void					ResizeWindow(const IRect& inRect)
	{
		if (mParent != nullptr && mRect.HasArea()) 
			mParent->MarkDirtyRect(mRect);

		mRect = inRect;
		MarkDirtyRect(IRect(0,0, mRect.GetWidth(), mRect.GetHeight()));
	}

protected:

	IRect					mRect;
	IRect					mDirtyRect;

	Array<RefPtr<DControl>>	mChildren;
	RefPtr<DControl>		mParent;
};


class DocWindow : public DControl
{
public:
	DocWindow(const IRect& inClientRect, DControl* inParent) : DControl(inClientRect, inParent)
	{
		mFont = Font::sCreateFont("Segoe ui", 12);
	}
	
	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons)
	{
		ResizeWindow(mRect.Moved(ivec2(10,5)));
	}

	virtual void Draw(DIB& inCanvas)
	{
		if (!mDirtyRect.HasArea())
			return;

		ColorPen<DIBColor> p(inCanvas);
		p.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcWhite));
		p.FillSquare(mRect);
		p.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcBlack));
		p.DrawSquare(mRect);

		IRect title_bar = mRect.Widened(-2);
		IRect child_rect = mRect.Widened(-2);
		title_bar.mBottom = title_bar.mTop + 20;
		child_rect.mTop = title_bar.mBottom + 1; 
		
		sPaintPattern(inCanvas, title_bar, DIBColor(64, 128, 200));
		sPaintPattern(inCanvas, child_rect, DIBColor(128, 128, 128));
		FontDrawer fd;
		fd.SetFont(mFont);
		fd.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcWhite));
		fd.Draw(inCanvas, "test", mRect.Widened(-4));
		MarkClean();
	}

private:

	bool						mMoving			= false;
	ivec2						mMoveStart;
	RefPtr<Font>				mFont;
};



class MyWindow : public Window, public IPaintHandler, public DControl
{

public:
	MyWindow() : Window(), mCanvas(*this), mMouseHandler(*this), DControl(IRect::sEmpty(), nullptr)
	{
		AddHandler(&mCanvas);
		AddHandler(&mMouseHandler);
		mChildren.Append(new DocWindow(IRect(10,10,100,100), this));
	}


	virtual void OnSize(const ivec2& inNewSize)
	{
		ResizeWindow(IRect(0,0,inNewSize.x, inNewSize.y));
	}
	

	virtual void Draw(DIB& inDib)
	{
		if (mDirtyRect.HasArea())
		{
			DIBColor c = DIBColor::sCreateDefaultPaletteColor(EDefaultPaletteColor(gRandRange(0, 15)));
			sPaintPattern(inDib, mDirtyRect, DIBColor(80,80,80));
		}
		MarkClean();
		DControl::Draw(inDib);
	}

	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons)
	{
		for (const RefPtr<DControl>& w : mChildren)
		{
			if (w->GetScreenRect().ContainsPoint(inPosition))
			{
				w->OnMouseLeftDown(inPosition - w->GetScreenRect().GetMin(), inButtons);
			}
		}
	}

	void DoTick()
	{
		Draw(mCanvas.GetDib());
		Invalidate();
	}

private:

	MouseHandler				mMouseHandler;
	Canvas						mCanvas;
} mainwindow;

int main()
{

//	Document doc;
	DIB* bg = new DIB();
//	bg->LoadFromFile(L"./test.bmp");
//	doc.mLayers.Append(bg);

	mainwindow.Create(L"ComeDither", 640, 480);
	mainwindow.Show(true);


	while (gDoMessageLoop(true)) 
	{
		
		mainwindow.DoTick();
	}

	std::cout << "test";
	return 0;
}

