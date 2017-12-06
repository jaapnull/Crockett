// implementation headers
#include <PCHWind.h>
#include <WCommon/Window.h>
#include <WCommon/Message.h>
#include <WWin32/Win32.h>

// statics for one-time window registration
bool	Window::sClassRegistered = false;
_Atom	Window::sClassAtom = 0;

static LRESULT CALLBACK gDispatchMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// As window is created, set the given pointer from lParam into the Window handle structure thing
	if (uMsg == WM_CREATE)
	{
		Window* window = ((Window*)(((CREATESTRUCT*)lParam)->lpCreateParams));
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		window->SetHandle(gHWNDToHandle(hwnd));
		window->OnCreate();
	}
	if (uMsg == WM_CLOSE)
	{
		PostQuitMessage(0);
	}

	// Retrieve it from the Window handle structure thing when needed.
	Window* w = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	MessageReturnCode retval = w ? w->HandleMessage(w, uMsg, MessageParam(wParam), MessageParam(lParam)) : mrcUnhandled;
	//std::cout << "msg= " << uMsg << '\n';
	if (retval == mrcUnhandled) return DefWindowProc(hwnd, uMsg, wParam, lParam);
	return -1; // unhandled
}


_Atom sGetRegisteredClass()
{
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.style = 0;// CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = gDispatchMessage;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = GetModuleHandle(0);
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH) NULL;
	wcx.lpszMenuName = 0;
	wcx.lpszClassName = L"Wind_Window";
	wcx.hIconSm = (HICON)0;
	return RegisterClassEx(&wcx);
}

Window::Window() : mHandle(0)
{

}
Window::~Window(void)
{
	Destroy();
}

void Window::Create(const WString inTitle, unsigned int inWidth, unsigned int inHeight) 
{
	if (!sClassRegistered)
	{
		sClassAtom = sGetRegisteredClass();
		assert(sClassAtom);
		sClassRegistered = true;
	}

	RECT r = { 0, 0, (LONG) inWidth, (LONG) inHeight };
	AdjustWindowRectEx(&r, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND h = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, (LPCWSTR)sClassAtom, inTitle.GetCString(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top, NULL, NULL, GetModuleHandle(0), this);
	DWORD d = GetLastError();
	assert(h);
	// do an early handle set so that the window object already has a proper handle, todo: set WindowLongPtr already here instead of in WM_CREATE?
	mHandle = gHWNDToHandle(h);	
}

void Window::Invalidate()
{
	HWND hwnd = gHandleToHWND(mHandle);
	InvalidateRect(hwnd, 0, false);
}

void Window::Invalidate(const IRect& inQuad)
{
	HWND hwnd = gHandleToHWND(mHandle);
	RECT r = { inQuad.mLeft, inQuad.mTop, inQuad.mRight, inQuad.mBottom };
	InvalidateRect(hwnd, &r, false);
}

void Window::SetResizeable(bool inIsResizeable)
{
	LONG style = GetWindowLong(gHandleToHWND(mHandle), GWL_STYLE);
	if(!inIsResizeable)
		style &= ~(WS_THICKFRAME);
	else
		style |= WS_THICKFRAME;
	SetWindowLong(gHandleToHWND(mHandle), GWL_STYLE, style);
}

MessageReturnCode Window::HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam inParamB)
{
	MessageReturnCode rc = MessageSource<Window>::HandleMessage(inParent, inMessage, inParamA, inParamB);

	// internal messages
	if (inMessage == WM_CLOSE)
	{
		OnClose();
	}

	if (inMessage == WM_SIZE)
	{
		if (uint((size64) inParamA) != SIZE_MINIMIZED)
		{
			uint new_width = LOWORD(inParamB);
			uint new_height = HIWORD(inParamB);
			OnSized(ivec2(new_width, new_height));
		}
		else
		{
			OnSized(ivec2(0, 0));
		}
	}
	else if (inMessage == WM_SIZING)
	{
		// int corner = inParamA (wparam);
		RECT* window_rect = (RECT*)inParamB;
		IRect our_rect = { window_rect->left, window_rect->top, window_rect->right, window_rect->bottom };

		RECT r = { 0,0,0,0 };
		BOOL b = AdjustWindowRectEx(&r, GetWindowLong(gHandleToHWND(mHandle), GWL_STYLE), false, GetWindowLong(gHandleToHWND(mHandle), GWL_EXSTYLE));

		our_rect.mLeft		-= r.left;
		our_rect.mTop		-= r.top;
		our_rect.mRight		-= r.right;
		our_rect.mBottom	-= r.bottom;

		ERectEdge edge =	inParamA == WMSZ_BOTTOM ? ERectEdge::reBottom :
							inParamA == WMSZ_LEFT ? ERectEdge::reLeft :
							inParamA == WMSZ_TOP ? ERectEdge::reTop:
							inParamA == WMSZ_RIGHT ? ERectEdge::reRight:
							inParamA == WMSZ_BOTTOMLEFT ? ERectEdge::reBottomLeft :
							inParamA == WMSZ_BOTTOMRIGHT ? ERectEdge::reBottomRight :
							inParamA == WMSZ_TOPLEFT ? ERectEdge::reTopLeft :
							inParamA == WMSZ_TOPRIGHT ? ERectEdge::reTopRight : ERectEdge::reInvalid;

		OnSizing(EnumMask<ERectEdge>(edge), our_rect);
		window_rect->left = our_rect.mLeft			+ r.left;
		window_rect->top = our_rect.mTop			+ r.top;
		window_rect->right = our_rect.mRight		+ r.right;
		window_rect->bottom = our_rect.mBottom		+ r.bottom;

		//BOOL b = AdjustWindowRectEx(window_rect, GetWindowLong(gHandleToHWND(mHandle), GWL_STYLE), false, GetWindowLong(gHandleToHWND(mHandle), GWL_EXSTYLE));


	}
	return rc;
}

uint	Window::GetWidth() const					{ RECT r; GetClientRect(gHandleToHWND(mHandle), &r); return r.right - r.left; }
uint	Window::GetHeight() const					{ RECT r; GetClientRect(gHandleToHWND(mHandle), &r); return r.bottom - r.top; }
void	Window::Close()								{ if (mHandle) CloseWindow(gHandleToHWND(mHandle)); }
void	Window::Resize(uint inWidth, uint inHeight) 
{ 
	RECT r = { 0,0, (LONG) inWidth, (LONG) inHeight };
	BOOL b = AdjustWindowRectEx(&r, GetWindowLong(gHandleToHWND(mHandle), GWL_STYLE), false, GetWindowLong(gHandleToHWND(mHandle), GWL_EXSTYLE));
	SetWindowPos(gHandleToHWND(mHandle), 0, -1, -1, r.right-r.left, r.bottom-r.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE); 
}
void	Window::Destroy()							{ if (mHandle) DestroyWindow(gHandleToHWND(mHandle)); SetWindowLongPtr(gHandleToHWND(mHandle), GWLP_USERDATA, 0); mHandle = 0; }
void	Window::SetTitle(const WString& title)		{ SetWindowText(gHandleToHWND(mHandle), title.GetCString()); }
void	Window::Show(bool visible)					{ ShowWindow(gHandleToHWND(mHandle), visible); }
