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

class MyWindow : public Window, IPaintHandler
{

public:
	MyWindow() : Window(), mCanvas(*this)
	{
	
	}

	void OnUpdate(DIB& inDib, const iquad& inRegion)
	{
		inDib.SetAll(DIBColor::sCreateDefaultPaletteColor(dpcLightCyan));
		ColorPen<DIBColor> pen(inDib);
	}

private:
	Canvas				mCanvas;

} mainwindow;

int main()
{
	mainwindow.Create(L"ComeDither", 640, 480);
	mainwindow.Show(true);

	while (gDoMessageLoop(true)) {}

	std::cout << "test";
	return 0;
}

