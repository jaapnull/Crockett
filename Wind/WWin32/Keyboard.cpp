// implementation headers
#include <PCHWind.h>

// compilation unit header
#include <WCommon/Message.h>
#include <WCommon/Window.h>
#include <WCommon/Keyboard.h>
#include <WWin32/Win32.h>

MessageReturnCode KeyboardHandler::HandleMessage(Window* inWindow, uint inMessage, MessageParam inParamA, MessageParam  inParamB)
{
	bool is_key_message = (inMessage&(WM_KEYDOWN | WM_KEYUP)) != 0;
	if (!is_key_message) return mrcUnhandled;

	uint code = (uint) (size64) inParamA;

	switch (inMessage)
	{
		case WM_KEYDOWN:		mTarget->OnKeyDown(code); break;
		case WM_KEYUP:			mTarget->OnKeyUp(code); break;
		default:
			return mrcUnhandled; // not a message that we use for this control atm
	}
	return mrcHandled;
}
