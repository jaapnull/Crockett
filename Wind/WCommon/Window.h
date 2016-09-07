#pragma once

#include <CCore/types.h>
#include <CCore/String.h>
#include <CGeo/Quad.h>
#include <WCommon/Message.h>

class Window : public MessageSource<Window>
{
public:
								Window();
	virtual						~Window(void);

	virtual MessageReturnCode	HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam inParamB);
	virtual void				Create(const WString title, unsigned int width, unsigned int height);
	virtual void				OnSize(const ivec2& inNewSize)			{ }
	void						Close();
	void						Destroy();
	void						Show(bool visible);
	void						SetTitle(const WString& title);
	uint						GetWidth() const;
	uint						GetHeight() const;

	_Handle						GetHandle()						const	{ return mHandle; }
	void						SetHandle(_Handle inHandle)				{ mHandle = inHandle; }
	void						Invalidate();
	void						Invalidate(const IRect& inQuad);

private:
	static bool					sClassRegistered;
	static _Atom				sClassAtom;	
	_Handle						mHandle;

	Window(const Window &) { assert(false); } // cannot create from another Window object}			

};
