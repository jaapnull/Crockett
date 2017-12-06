#include <PCHWind.h>

#include <CMath/Vector.h>
#include <WCommon/Message.h>
#include <WCommon/Window.h>
#include <WCommon/Mouse.h>
#include <WWin32/Win32.h>

MessageReturnCode MouseHandler::HandleMessage(Window* inWindow, uint inMessage, MessageParam inParamA, MessageParam  inParamB)
{
	bool is_mouse_message = (inMessage&(WM_MOUSEMOVE | WM_LBUTTONUP | WM_LBUTTONDOWN | WM_RBUTTONUP | WM_RBUTTONDOWN | WM_MBUTTONUP | WM_MBUTTONDOWN | WM_MOUSELEAVE)) != 0;
	if (!is_mouse_message) return mrcUnhandled;

	EnumMask<MMouseButtons> buttons;	
	uint button_mask = (uint) ((size64) inParamA);
	// transform Win32 mask to our EnumMask<MMouseButtons>
	if (button_mask&MK_LBUTTON)	buttons << mbLeft;
	if (button_mask&MK_MBUTTON)	buttons << mbMiddle;
	if (button_mask&MK_RBUTTON)	buttons << mbRight;
	if (button_mask&MK_SHIFT)	buttons << mbShift;
	if (button_mask&MK_CONTROL)	buttons << mbCtrl;

	ivec2 pos(LOWORD(inParamB), HIWORD(inParamB));

	bool contains_button_pressed	= (button_mask&(MK_MBUTTON | MK_LBUTTON | MK_RBUTTON)) != 0;
	bool drag_start_event			= false;	// first mouse button pressed
	bool drag_release_event			= false;	// last mouse button let go

	switch (inMessage)
	{
		case WM_MOUSEMOVE:		mTarget->OnMouseMove(pos, buttons); break;
		case WM_LBUTTONUP:		mTarget->OnMouseLeftUp(pos, buttons);		drag_release_event = !contains_button_pressed; break;
		case WM_LBUTTONDOWN:	mTarget->OnMouseLeftDown(pos, buttons);		drag_start_event = contains_button_pressed; break;
		case WM_RBUTTONUP:		mTarget->OnMouseRightUp(pos, buttons);		drag_release_event = !contains_button_pressed; break;
		case WM_RBUTTONDOWN:	mTarget->OnMouseRightDown(pos, buttons);	drag_start_event = contains_button_pressed; break;
		case WM_MBUTTONUP:		mTarget->OnMouseMiddleUp(pos, buttons);		drag_release_event = !contains_button_pressed; break;
		case WM_MBUTTONDOWN:	mTarget->OnMouseMiddleDown(pos, buttons);	drag_start_event = contains_button_pressed; break;
		case WM_MOUSELEAVE:		mTarget->OnMouseLeave();					drag_release_event = true; mMouseLeaveTracking = false; break;
		default:
			return mrcUnhandled; // not a message that we use for this control atm
	}

	// start tracking as soon as the mouse enters the window
	if (!mMouseLeaveTracking && (inMessage == WM_MOUSEMOVE))
	{
		// start tracking for mouse leave events
		TRACKMOUSEEVENT e;
		e.cbSize = sizeof(TRACKMOUSEEVENT);
		e.dwFlags = TME_LEAVE;
		e.hwndTrack = reinterpret_cast<HWND>(inWindow->GetHandle());
		e.dwHoverTime = 0;
		TrackMouseEvent(&e);
		mMouseLeaveTracking = true;
	}

	if (mDragging)
	{
		if (drag_release_event)
		{
			mTarget->OnMouseDragStop(pos, buttons, inMessage == WM_MOUSELEAVE);
			mDragging = false;
		}
		else
		{
			mTarget->OnMouseDrag(pos, pos - mDragStart);
			mDragPrevious = pos;
		}
	}
	else
	{
		if (drag_start_event)
		{
			mDragStart = pos;
			mDragPrevious = pos;
			mDragging = true;
		}
	}


	return mrcHandled;
}

bool MouseHandler::IsDragging(ivec2& outDragStart) const
{
	if (mDragging)
	{
		outDragStart = mDragStart;
	}
	return mDragging;
}

ivec2 MouseHandler::GetDragBegin() const
{
	return  mDragStart;
}

