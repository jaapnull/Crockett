#include <CCore/String.h>
#include <CMath/Math.h>
#include <CMath/Vector.h>
#include <CUtils/DebugFont.h>
#include <CGeo/HalfSpace2.h>
#include <CGeo/LineSegment2.h>
#include <CGeo/Polygon2.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CUtils/Sort.h>
#include <CUtils/Trace.h>
#include <CUtils/Perf.h>

#include <unordered_map>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>
#include <WCommon/Keyboard.h>


#include "Document.h"
#include "PaletteWindow.h"


Document gCurrentDocument;
Array<Palette*> gPalettes;


#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif

PaletteWindow gPaletteWindow;



class TilerdWindow : public Window, public IPaintHandler, IMouseHandler, IKeyHandler
{

public:

	TilerdWindow() : Window(), mCanvas(*this), mMouseHandler(*this), mKeyboard(*this)
	{
		AddHandler(&mCanvas);
		mCanvas.SetDefaultColor(DIBColor(255, 255, 255));
		AddHandler(&mMouseHandler);
		AddHandler(&mKeyboard);
	}

	virtual void OnSized(const ivec2& inNewSize) override
	{
		Invalidate();
	}


	virtual void OnUpdate(DIB& inDib, const IRect& inRegion) override
	{

		inDib.SetAll(DIBColor(255, 255, 255));

		HexCoordBase b = gCurrentDocument.GetCoordBase();
		IRect r = GetVisibleRange();

	}

	virtual void OnMouseDrag(const ivec2& inDragStart, const ivec2& inDragDelta) override
	{
		mOffsetPixelSpace += inDragDelta;
		Invalidate();
	}

	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		
	}

	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) override
	{
		mZoomFactor++;
		Invalidate();
	}

	ivec2 ClientPixelToWorldPixel(const ivec2& inClientPixel) const			{ return inClientPixel + mOffsetPixelSpace; }
	ivec2 WorldPixelToClientPixel(const ivec2& inWorldPixel) const			{ return inWorldPixel - mOffsetPixelSpace; }
	IRect GetVisibleRange() const											{ return IRect(ClientPixelToWorldPixel(ivec2(0,0)), ClientPixelToWorldPixel(GetDimensions())); }

private:
	KeyboardHandler				mKeyboard;
	MouseHandler				mMouseHandler;
	Canvas						mCanvas;

	ivec2						mOffsetPixelSpace = ivec2(0, 0);
	uint						mZoomFactor = 1;

} gMainWindow;

int main()
{
	TimeLine timeline(4000);

	timeline.Start();

	timeline.PushTimedEvent(TimedEventText("test"));
	TraceStream<char>::sHookStream(std::cout);
	TraceStream<wchar_t>::sHookStream(std::wcout);

	timeline.PushTimedEvent(TimedEventInteger(100));
	timeline.PushTimedEvent(TimedEventText("test2"));

	gPalettes.Append(Palette::sCreateFromFile(L".\\palette.bmp", ivec2(32,32)));


	gCurrentDocument.Resize(100, 100);
	for (int x = 0; x < 100; x++)
		for (int y = 0; y < 100; y++)
		{
			gCurrentDocument.GetMap().Get(x,y) = TileEntry(gPalettes.Back(), ivec2(gRandRange(0, 4), gRandRange(0, 4)));
		}


	timeline.PushTimedEvent(TimedEventInteger(100));
	timeline.PushTimedEvent(TimedEventText("test3"));

	gPaletteWindow.Create(L"Palette", 500, 500);
	gPaletteWindow.Show(true);
	gMainWindow.Create(L"Main", 500, 500);
	gMainWindow.Show(true);
	//gMainWindow.SetFocus({ 500, 500 });


	timeline.PushTimedEvent(TimedEventText("testbeforeleep"));
	Sleep(1000);
	timeline.PushTimedEvent(TimedEventText("testaftersleep"));


	timeline.Stop();
	Array<TimedEventBase*> events;
	timeline.GetPushedEvents(events);

	for (TimedEventBase* e : events)
	{
		std::cout << timeline.GetTimeInSeconds(*e) << " : " << std::endl;
	}


	while (gDoMessageLoop(true))
	{
	}
	return 0;
}

