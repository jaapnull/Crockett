#pragma once

#include <CCore/types.h>
#include <WCommon/DIB.h>


class IPaintHandler
{
public:
	virtual void OnUpdate(DIB& inDib, const IRect& inRegion) { }
};

class Canvas : public IMessageHandler<Window>
{
	DIB					mDib;
	IPaintHandler*		mTarget;
public:
						Canvas(IPaintHandler& inHandler)		{ mTarget = &inHandler; }
	const DIB&			GetDib()	const						{ return mDib; }
	DIB&				GetDib()								{ return mDib; }
	MessageReturnCode	HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam  inParamB);	// implementation outside in CPP
};