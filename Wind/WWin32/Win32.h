#pragma once

#include <WWin32/targetver.h>

#define WIN32_IS_INCLUDED
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <CCore/types.h>
#include <CCore/Assert.h>

// Win32 placeholder member types to use in headers for instancing
gStaticAssert(sizeof(_Handle) == sizeof(HWND));
gStaticAssert(sizeof(_Handle) == sizeof(HANDLE));
gStaticAssert(sizeof(_Handle) == sizeof(HDC));
gStaticAssert(sizeof(_Atom) == sizeof(ATOM));

inline HWND		gHandleToHWND(const _Handle& inHandle) { return reinterpret_cast<HWND>(inHandle); }
inline HANDLE	gHandleToHANDLE(const _Handle& inHandle) { return reinterpret_cast<HANDLE>(inHandle); }
inline HDC		gHandleToHDC(const _Handle& inHandle) { return reinterpret_cast<HDC>(inHandle); }
inline HBITMAP	gHandleToHBITMAP(const _Handle& inHandle) { return reinterpret_cast<HBITMAP>(inHandle); }

