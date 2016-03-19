#pragma once

#include <CCore/types.h>
#include <WCommon/Message.h>
#include <WCommon/Window.h>


// mouse interface definition
class IKeyHandler
{
public:
	virtual void OnKeyDown(const uint inKeyCode) {}
	virtual void OnKeyUp(const uint inKeyCode) {}
};

//template <class C_Source, class C_Interface>
class KeyboardHandler : public IMessageHandler<Window>
{
private:
	IKeyHandler*		mTarget;			// target we call our callbacks on
public:
						KeyboardHandler(IKeyHandler& inTarget) : mTarget(&inTarget) {}
	MessageReturnCode	HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam  inParamB);	// implementation outside in CPP
};