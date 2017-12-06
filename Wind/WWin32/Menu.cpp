#include <WCommon/Menu.h>
#include <CCore/String.h>
#include <WWin32/Win32.h>

Menu::Menu(_Handle inHandle)
{
	mMenuHandle = inHandle;
}

Menu::~Menu()
{
	DestroyMenu(gHandleToHMENU(GetHandle()));
}

_Handle Menu::GetHandle()
{
	return mMenuHandle;
}

void Menu::AddMenu(uint inPos, uint inID, const WString& inText, Menu& menu, EnumMask<EMenuState> inState)
{
	int state = 0;// = MF_BYPOSITION | MF_POPUP;
	if (inState.Contains(EMenuState::misInvalid)) state |= MFS_GRAYED | MFS_DISABLED;
	if (inState.Contains(EMenuState::misChecked)) state |= MFS_CHECKED;
	MENUITEMINFO mi;
	mi.cbSize = sizeof(mi);
	//		mi.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_DATA | MIIM_TYPE | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
	mi.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_ID | MIIM_STATE;
	mi.wID = inID;
	mi.fState = state;
	mi.fType = MFT_STRING;
	mi.hSubMenu = gHandleToHMENU(menu.GetHandle());
	mi.dwItemData = 0;//text.c_str();
	mi.dwTypeData = (LPWSTR)inText.GetCString();
	unsigned int succeed = InsertMenuItem(gHandleToHMENU(mMenuHandle), inPos, true, &mi);
	assert(succeed);
}

void Menu::AddSeperator(uint inPos)
{
	unsigned int succeed = InsertMenu(gHandleToHMENU(mMenuHandle), inPos, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
	assert(succeed);
}

void Menu::AddItem(uint inPos, uint inID, const WString& inText, EnumMask<EMenuState> inState)
{
	int flags = MF_STRING | MF_BYPOSITION;
	if (inState.Contains(EMenuState::misInvalid)) flags |= MF_GRAYED | MF_DISABLED;
	if (inState.Contains(EMenuState::misChecked)) flags |= MF_CHECKED;

	unsigned int succeed = InsertMenu(gHandleToHMENU(GetHandle()), inPos, flags, inID, inText.GetCString());
	assert(succeed);
}

void Menu::SetChecked(uint inID, bool inChecked)
{
	CheckMenuItem(gHandleToHMENU(GetHandle()), inID, MF_BYCOMMAND | (inChecked ? MF_CHECKED : MF_UNCHECKED));
}

void Menu::SetEnabled(uint inID, bool inEnabled)
{
	EnableMenuItem(gHandleToHMENU(GetHandle()), inID, MF_BYCOMMAND | (inEnabled ? MF_ENABLED : MF_DISABLED | MF_GRAYED));
}


WindowMenu::WindowMenu() : Menu(gHMENUToHandle(CreateMenu()))
{}

void WindowMenu::Update()
{
	DrawMenuBar(gHandleToHWND(mBoundWindow->GetHandle()));
}

void WindowMenu::BindToWindow(Window& inWindow)
{
	HWND window_handle = gHandleToHWND(inWindow.GetHandle());
	uint succeed = SetMenu(window_handle, gHandleToHMENU(mMenuHandle));
	assert(succeed);
	mBoundWindow = &inWindow;
}

WindowMenu::~WindowMenu()
{
	if (mBoundWindow != nullptr)
	{
		SetMenu(gHandleToHWND(mBoundWindow->GetHandle()), gHandleToHMENU(GetHandle()));
	}
}

PopupMenu::PopupMenu() : Menu(gHMENUToHandle(::CreatePopupMenu()))
{}

void PopupMenu::PopupWindow(Window& inWindow, const ivec2& inPos)
{
	RECT r; GetWindowRect(gHandleToHWND(inWindow.GetHandle()), &r);
	//todo alignment according to position in the window
	uint b = TrackPopupMenuEx(gHandleToHMENU(GetHandle()), TPM_LEFTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN, r.left + inPos.x, r.top + inPos.y, gHandleToHWND(inWindow.GetHandle()), 0);
	assert(b);
}

void PopupMenu::PopupScreen(Window& inWindow, const ivec2& inPos)
{
	RECT r; GetWindowRect(gHandleToHWND(inWindow.GetHandle()), &r);
	//todo alignment according to position in the window
	uint b = TrackPopupMenuEx(gHandleToHMENU(GetHandle()), TPM_RIGHTALIGN | TPM_BOTTOMALIGN, inPos.x, inPos.y, gHandleToHWND(inWindow.GetHandle()), 0);
	assert(b);
}

