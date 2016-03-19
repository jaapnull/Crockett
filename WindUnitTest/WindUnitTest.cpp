#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>

class MyWindow: public Window, IPaintHandler
{
public:
	MyWindow() : Window(), mCanvas(*this)
	{
		AddHandler(&mCanvas);
	}

	virtual void OnUpdate(DIB& inDib, const iquad& inRegion) override
	{
		//ColorPen<DIBColor> pen(inDib);
		//pen.SetColor(DIBColor::sCreateDefaultPaletteColor(EDefaultPaletteColor(gRand() % 16)));
		//pen.FillSquare(inRegion.mLeft, inRegion.mTop, inRegion.mRight, inRegion.mBottom);
		DIBColor c = DIBColor::sCreateDefaultPaletteColor(EDefaultPaletteColor(gRand() % 16));
		inDib.SetRegion(inRegion, c);
	}

private:
	Canvas mCanvas;
	
} mainwindow;

int main()
{
	mainwindow.Create(L"Crockett", 640, 480);
	mainwindow.Show(true);

	while (gDoMessageLoop(true)) {}

	std::cout << "test";
	return 0;
}

