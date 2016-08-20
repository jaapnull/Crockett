#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>

#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif


struct Document
{
	Array<DIB*> mLayers;
};




class MyWindow : public Window, IPaintHandler
{

public:
	MyWindow() : Window(), mCanvas(*this)
	{
		AddHandler(&mCanvas);
	}

	void OnUpdate(DIB& inDib, const iquad& inRegion)
	{
		DIBColor c = DIBColor::sCreateDefaultPaletteColor(EDefaultPaletteColor(gRandRange(0, 15)));
		inDib.SetAll(c);
		ColorPen<DIBColor> pen(inDib);
	}

private:
	Canvas				mCanvas;

} mainwindow;

int main()
{

	Document doc;
	DIB* bg = new DIB();
	bg->LoadFromFile(L"./test.bmp");
	doc.mLayers.Append(bg);

	mainwindow.Create(L"ComeDither", 640, 480);
	mainwindow.Show(true);


	while (gDoMessageLoop(true)) {}

	std::cout << "test";
	return 0;
}

