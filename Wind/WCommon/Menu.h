#pragma once
#include <CCore/String.h>
#include "Window.h"
#include <WCommon/Message.h>

class Menu
{
public:

	enum class EMenuState
	{
		misNormal = 0,
		misInvalid = 1,
		misChecked = 2
	};

	Menu(_Handle inHandle);
	~Menu();
	_Handle GetHandle();

	void AddMenu(uint inPos, uint inID, const WString& inText, Menu& menu, EnumMask<EMenuState> inState);
	void AddSeperator(uint inPos);
	void AddItem(uint inPos, uint inID, const WString& inText, EnumMask<EMenuState> inState);
	void SetChecked(uint inID, bool inChecked);
	void SetEnabled(uint inID, bool inEnabled);

protected:
	_Handle mMenuHandle;

};

class WindowMenu : public Menu
{
public:
	WindowMenu();
	void Update();
	void BindToWindow(Window& window);
	~WindowMenu();

private:
	Window* mBoundWindow = nullptr;
};

class PopupMenu : public Menu
{
public:
	PopupMenu();
	void PopupWindow(Window& inWindow, const ivec2& inPos);
	void PopupScreen(Window& inWindow, const ivec2& inPos);
};