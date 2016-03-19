// implementation headers
#include <PCHWind.h>

#include <WCommon/Message.h>
#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WWin32/Win32.h>


MessageReturnCode Canvas::HandleMessage(Window* inWindow, uint inMessage, MessageParam inParamA, MessageParam  inParamB)
{
	if (inMessage == WM_PAINT)
	{
		if (!mDib.IsValid())
		{
			mDib.Resize(inWindow->GetWidth(), inWindow->GetHeight());
			mDib.SetAll(DIBColor(100, 200, gRand() & 0xFF));
		}

		HWND hwnd = gHandleToHWND(inWindow->GetHandle());
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
			
		if(mTarget)
		{
			mTarget->OnUpdate(mDib, iquad(ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom));
		}
		
		BitBlt(ps.hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, HDC(mDib.GetHandle()), ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

		EndPaint(hwnd, &ps);
		return mrcHandled;
	}

	if (inMessage == WM_SIZE)
	{
		if (uint((size64) inParamA) != SIZE_MINIMIZED)
		{
			uint new_width = LOWORD(inParamB);
			uint new_height = HIWORD(inParamB);
			if (new_width < 200) new_width = 200;
			if (new_height < 200) new_height = 200;
			mDib.Resize(new_width, new_height);
			mDib.SetAll(DIBColor(100, 200, gRand() & 0xff));
		}
		return mrcUnhandled;
	}
	return mrcUnhandled;
}

