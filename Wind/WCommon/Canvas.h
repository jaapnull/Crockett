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
public:
						Canvas(IPaintHandler& inHandler)			{ mTarget = &inHandler; }
	const DIB&			GetDib()	const							{ return mDib; }
	DIB&				GetDib()									{ return mDib; }
	void				SetDefaultColor(const DIBColor& inColor)	{ mDefaultColor = inColor; }

	MessageReturnCode	HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam  inParamB);	// implementation outside in CPP

private:
	DIB					mDib;
	IPaintHandler*		mTarget										= nullptr;
	DIBColor			mDefaultColor								= DIBColor(255,255,255);
};