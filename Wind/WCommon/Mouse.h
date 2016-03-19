#pragma once

#include <CCore/types.h>
#include <CMath/Vector.h>
#include <WCommon/Message.h>

// definition of the various buttons (and mouse states)
// to be used in an EnumMask so masked values
enum MMouseButtons
{
	mbLeft			= 1,
	mbRight			= 2,
	mbMiddle		= 4,
	mbAlt			= 8,	//unsupported
	mbCtrl			=16,
	mbShift			=32
};

// inline std::wostream& operator<<(std::wostream& inStream, EnumMask<MMouseButtons> inButtons)
// {
// 	wchar_t base_text[7] = L"lrmacs";
// 	for (int x = 0; x < 8; x++)
// 	{
// 		if (inButtons.Contains(MMouseButtons(1 << x)))
// 			base_text[x] = ::toupper(base_text[x]);
// 	}
// 	inStream << base_text;
// 	return inStream;
// }

// mouse interface definition
class IMouseHandler
{
public:
	virtual void OnMouseMove(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseLeftUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseLeftDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseRightUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseRightDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseMiddleUp(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseMiddleDown(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons) {}
	virtual void OnMouseDragStop(const ivec2& inPosition, EnumMask<MMouseButtons> inButtons, bool inLeftWindow) {}
	virtual void OnMouseLeave() {}
};

//template <class C_Source, class C_Interface>
class MouseHandler : public IMessageHandler<Window>
{
private:
	IMouseHandler*		mTarget;			// target we call our callbacks on
	ivec2				mDragStart;			// start position of a dragging action
	bool				mDragging;			// are we dragging at this moment
	bool				mMouseLeaveTracking;// are we monitoring for mouse_leave events
public:
	bool				IsDragging(ivec2& outDragStart) const;
	ivec2				GetDragBegin() const;
						MouseHandler(IMouseHandler& inTarget) : mTarget(&inTarget), mDragging(false), mMouseLeaveTracking(false) {}
	MessageReturnCode	HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam  inParamB);	// implementation outside in CPP
};