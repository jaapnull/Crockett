#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>

//HWND windows_included;

struct AnchorRestriction
{
			AnchorRestriction(int inPos) :
				mP0(cMaxUint),
				mP1(cMaxUint),
				mMin(0),
				mMax(0),
				mPercentage(0.0f),
				mResolvedPosition(inPos),
				mRefCount(0) {}

			AnchorRestriction(uint inP0, uint inP1, int inMin, int inMax, float inPercentage) :
				mP0(inP0),
				mP1(inP1),
				mMin(inMin),
				mMax(inMax),
				mPercentage(inPercentage),
				mResolvedPosition(0),
				mRefCount(0) {}
	uint	mP0;
	uint	mP1;
	int		mMin;
	int		mMax;
	float	mPercentage;
	int		mResolvedPosition;
	uint	mRefCount;
};


struct LayoutData
{
	AnchorRestriction	mLeft;
	AnchorRestriction	mTop;
	AnchorRestriction	mRight;
	AnchorRestriction	mBottom;
};

class JuiceWindow
{
public:
	virtual void											OnDraw(DIB& inCanvas, IRect& inClientRect) = 0;
	virtual void											AddChildWindow(JuiceWindow* inChild) = 0;
private:
	//typedef std::pair<JuiceWindow*, LayoutData>				LayoutEntry;
	//typedef std::unordered_map<JuiceWindow, LayoutData>		ChildMap;			// 
	//ChildMap													mChilderen;			// a map from id to instance function		
	//JuiceLayout												mLayout;
};


class JuicePanel : public JuiceWindow
{
public:
	JuicePanel(const DIBColor& inColor) : mColor(inColor) {}

	virtual void OnDraw(DIB& inCanvas, IRect& inClientRect)
	{
		inCanvas.SetRegion(inClientRect, mColor);
	}

private:
	DIBColor mColor;
};

class JuiceLayout
{
public:

	JuiceLayout()
	{
		// setup the four base restrictions (left, top, right, bottom)
		mRestrictionsHorz.Append(AnchorRestriction(0));
		mRestrictionsVert.Append(AnchorRestriction(0));
		mRestrictionsHorz.Append(AnchorRestriction(0));
		mRestrictionsVert.Append(AnchorRestriction(0));
	}

	void UpdateLayout(IRect inArea)
	{
		gAssert(mRestrictionsVert.GetLength() >= 2);
		gAssert(mRestrictionsHorz.GetLength() >= 2);
		// first four always resolve to area bounds
		mRestrictionsVert[0].mResolvedPosition = inArea.mTop;
		mRestrictionsVert[1].mResolvedPosition = inArea.mBottom;
		mRestrictionsHorz[0].mResolvedPosition = inArea.mLeft;
		mRestrictionsHorz[1].mResolvedPosition = inArea.mRight;

		for (AnchorRestriction& p : mRestrictionsHorz)
		{
			if (p.mP0 == cMaxUint) continue;
			int p0 = mRestrictionsHorz[p.mP0].mResolvedPosition;
			int p1 = mRestrictionsHorz[p.mP1].mResolvedPosition;
			if (p0 < p1)
			{
				p.mResolvedPosition = gMin(p1, (int)gMax(float(p0 + p.mMin), (float(p0) + float(p1 - p0) * p.mPercentage)));
			}
			else
			{
				p.mResolvedPosition = gMax(p1, (int)gMin(float(p0 - p.mMin), (float(p0) + float(p1 - p0) * p.mPercentage)));
			}
		}
		for (AnchorRestriction& p : mRestrictionsVert)
		{
			if (p.mP0 == cMaxUint) continue;
			int p0 = mRestrictionsVert[p.mP0].mResolvedPosition;
			int p1 = mRestrictionsVert[p.mP1].mResolvedPosition;
			if (p0 < p1)
			{
				p.mResolvedPosition = gMin(p1, (int)gMax(float(p0 + p.mMin), (float(p0) + float(p1 - p0) * p.mPercentage)));
			}
			else
			{
				p.mResolvedPosition = gMax(p1, (int)gMin(float(p0 - p.mMin), (float(p0) + float(p1 - p0) * p.mPercentage)));
			}
		}
	}

	size64 AddRestrictionVert(const AnchorRestriction& inAnchor)
	{
		mRestrictionsVert.Append(inAnchor);
		return mRestrictionsVert.GetLength();
	}
	
	size64 AddRestrictionHorz(const AnchorRestriction& inAnchor)
	{
		mRestrictionsHorz.Append(inAnchor);
		return mRestrictionsHorz.GetLength();
	}

	const Array<AnchorRestriction> GetRestrictionsHorz()
	{
		return mRestrictionsHorz;
	}

	const Array<AnchorRestriction> GetRestrictionsVert()
	{
		return mRestrictionsVert;
	}

	private:
	Array<AnchorRestriction>		mRestrictionsHorz;
	Array<AnchorRestriction>		mRestrictionsVert;

};


class MyWindow: public Window, IPaintHandler
{

public:
	MyWindow() : Window(), mCanvas(*this)
	{
		AddHandler(&mCanvas);
		mLayout.AddRestrictionHorz(AnchorRestriction(1, 0, 10, 0, 0.0f));
		mLayout.AddRestrictionHorz(AnchorRestriction(2, 0, 60, 0, 0.0f));
		
		mLayout.AddRestrictionVert(AnchorRestriction(0, 1, 30, 0, 0.0f));
		mLayout.AddRestrictionVert(AnchorRestriction(2, 1, 50, 0, 0.0f));
		mLayout.AddRestrictionVert(AnchorRestriction(3, 1, 10, 0, 0.0f));
		mLayout.AddRestrictionVert(AnchorRestriction(4, 1, 50, 0, 0.0f));
		mLayout.AddRestrictionVert(AnchorRestriction(5, 1, 10, 0, 0.0f));
		mLayout.AddRestrictionVert(AnchorRestriction(6, 1, 50, 0, 0.0f));
	}
	
	void OnUpdate(DIB& inDib, const IRect& inRegion) 
	{ 
		mLayout.UpdateLayout(IRect(0,0,GetWidth()-1, GetHeight()-1));

		if (GetWidth() < 1 || GetHeight() < 1) return;

		inDib.SetAll(DIBColor::sCreateDefaultPaletteColor(dpcLightCyan));
		ColorPen<DIBColor> pen(inDib);
		

		int vert_tabs = 15;
		int horz_tabs = 15;

		for (const AnchorRestriction& a : mLayout.GetRestrictionsVert())
		{
			if (a.mP0 != cMaxUint)
			{
				pen.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcBlack));
				pen.DrawLine(ivec2(0, a.mResolvedPosition), ivec2(GetWidth() - 1, a.mResolvedPosition));
				pen.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcCyan));
				ivec2 begin(vert_tabs, mLayout.GetRestrictionsVert()[a.mP0].mResolvedPosition);
				ivec2 end(vert_tabs, a.mResolvedPosition);
				pen.DrawLine(begin, end);
				vert_tabs += 7;
			}
		}

		for (const AnchorRestriction& a : mLayout.GetRestrictionsHorz())
		{
			if (a.mP0 != cMaxUint)
			{
				pen.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcBlack));
				pen.DrawLine(ivec2(a.mResolvedPosition, 0), ivec2(a.mResolvedPosition, GetHeight()-1));
				pen.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcCyan));
				ivec2 begin(mLayout.GetRestrictionsHorz()[a.mP0].mResolvedPosition, horz_tabs);
				ivec2 end(a.mResolvedPosition, horz_tabs);
				pen.DrawLine(begin, end);
				horz_tabs += 7;
			}
		}

	}

private:
	JuiceLayout			mLayout;
	Canvas				mCanvas;

} mainwindow;

int main()
{
	mainwindow.Create(L"Crockett", 640, 480);
	mainwindow.Show(true);

	while (gDoMessageLoop(true)) {}

	std::cout << "test";
	return 0;
}

