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
		window ->SetHandle(hwnd);
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
	mHandle = h;	
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


MessageReturnCode Window::HandleMessage(Window* inParent, uint inMessage, MessageParam inParamA, MessageParam inParamB)
{
	MessageReturnCode rc = MessageSource<Window>::HandleMessage(inParent, inMessage, inParamA, inParamB);

	// internal messages
	if (inMessage == WM_SIZE)
	{
		if (uint((size64) inParamA) != SIZE_MINIMIZED)
		{
			uint new_width = LOWORD(inParamB);
			uint new_height = HIWORD(inParamB);
			OnSize(ivec2(new_width, new_height));
		}
	}
	return rc;
}

uint	Window::GetWidth() const					{ RECT r; GetClientRect(gHandleToHWND(mHandle), &r); return r.right - r.left; }
uint	Window::GetHeight() const					{ RECT r; GetClientRect(gHandleToHWND(mHandle), &r); return r.bottom - r.top; }
void	Window::Close()								{ if (mHandle) CloseWindow(gHandleToHWND(mHandle)); }
void	Window::Destroy()							{ if (mHandle) DestroyWindow(gHandleToHWND(mHandle)); SetWindowLongPtr(gHandleToHWND(mHandle), GWLP_USERDATA, 0); mHandle = 0; }
void	Window::SetTitle(const WString& title) { SetWindowText(gHandleToHWND(mHandle), title.GetCString()); }
void	Window::Show(bool visible)					{ ShowWindow(gHandleToHWND(mHandle), visible); }
