#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>

//HWND windows_included;

struct PanelAnchorPoint
{
	enum EAnchorType { atHorizontal, atVertical};

				PanelAnchorPoint(EAnchorType inType, int inPos) :
					mType(inType),
					mParent0(cMaxUint),
					mParent1(cMaxUint),
					mPixels(0),
					mPercentage(0.0f),
					mResolvedPosition(inPos) {}

				PanelAnchorPoint(EAnchorType inType, uint inP0, uint inP1, int inPixels, float inPercentage) :
					mType(inType),
					mParent0(inP0),
					mParent1(inP1),
					mPixels(inPixels), 
					mPercentage(inPercentage),
					mResolvedPosition(0) {}
	EAnchorType	mType;
	uint		mParent0;
	uint		mParent1;
	int			mPixels;
	float		mPercentage;
	int			mResolvedPosition;
};


class AnchoredLayout
{
public:

	AnchoredLayout()
	{
		// setup the four base anchors (left, top, right, bottom)
		mAnchors.Append(PanelAnchorPoint(PanelAnchorPoint::atHorizontal, 0));
		mAnchors.Append(PanelAnchorPoint(PanelAnchorPoint::atVertical, 0));
		mAnchors.Append(PanelAnchorPoint(PanelAnchorPoint::atHorizontal, 0));
		mAnchors.Append(PanelAnchorPoint(PanelAnchorPoint::atVertical, 0));
	}

	void UpdateLayout(iquad inArea)
	{
		gAssert(mAnchors.GetLength() >= 4);

		mAnchors[0].mResolvedPosition = inArea.mLeft;
		mAnchors[1].mResolvedPosition = inArea.mTop;
		mAnchors[2].mResolvedPosition = inArea.mRight;
		mAnchors[3].mResolvedPosition = inArea.mBottom;

		for (PanelAnchorPoint& p : mAnchors)
		{
			if (p.mParent0 == cMaxUint) continue;
			int p0 = mAnchors[p.mParent0].mResolvedPosition;
			int p1 = mAnchors[p.mParent1].mResolvedPosition;
			if (p0 < p1)
			{
				p.mResolvedPosition = (int)gMax(float(p0 + p.mPixels), (float(p0) + float(p1 - p0) * p.mPercentage));
			}
			else
			{
				p.mResolvedPosition = (int)gMin(float(p0 - p.mPixels), (float(p0) + float(p1 - p0) * p.mPercentage));
			}
		}
	}

	uint AddAnchor(const PanelAnchorPoint& inAnchor)
	{
		mAnchors.Append(inAnchor);
		return mAnchors.GetLength();
	}

	const Array<PanelAnchorPoint> GetAnchors()
	{
		return mAnchors;
	}

	private:
	Array<PanelAnchorPoint>		mAnchors;
};


class MyWindow: public Window, IPaintHandler
{

public:
	MyWindow() : Window(), mCanvas(*this)
	{
		AddHandler(&mCanvas);
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atVertical, 1, 3, 150, .5f));
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atHorizontal, 0, 2, 50, .5f));
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atHorizontal, 5, 0, 50, 0.25f));
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atHorizontal, 2, 0, 50, 0.0f));
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atHorizontal, 2, 0, 60, 0.0f));
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atVertical, 1, 3, 50, 0.0f));
		mLayout.AddAnchor(PanelAnchorPoint(PanelAnchorPoint::atVertical, 1, 3, 100, 0.0f));
	}
	
	void OnUpdate(DIB& inDib, const iquad& inRegion) 
	{ 
		mLayout.UpdateLayout(iquad(0,0,GetWidth()-1, GetHeight()-1));
		inDib.SetAll(DIBColor::sCreateDefaultPaletteColor(dpcLightCyan));
		ColorPen<DIBColor> pen(inDib);
		pen.SetColor(DIBColor::sCreateDefaultPaletteColor(dpcBlack));
		for (const PanelAnchorPoint& a : mLayout.GetAnchors())
		{
			if (a.mType == PanelAnchorPoint::atVertical)
			{
				pen.DrawLine(ivec2(0, a.mResolvedPosition), ivec2(GetWidth()-1, a.mResolvedPosition));
			}
			else
			{
				pen.DrawLine(ivec2(a.mResolvedPosition, 0), ivec2(a.mResolvedPosition, GetHeight()-1));
			}
		}

	}

private:
	AnchoredLayout		mLayout;
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

