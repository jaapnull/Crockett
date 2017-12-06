#pragma once
#include <WCommon/OpenFileDialog.h>
#include <WWin32/Win32.h>
#include <Commdlg.h>

WString OpenFileDialog::RunSingle()
{
	WString mFilterstring;
	wchar_t buffer[255];
	buffer[0] = 0;
	for (unsigned int x = 0; x < mFilters.GetLength(); x++)
	{
		mFilterstring.Append(mFilters[x].mName);
		mFilterstring.Append(L'\0');
		mFilterstring.Append(mFilters[x].mExtension);
		mFilterstring.Append(L'\0');
	}
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = gHandleToHWND(mParent.GetHandle());
	ofn.hInstance = 0;
	ofn.lpstrCustomFilter = 0;
	ofn.nMaxCustFilter = 0;
	ofn.lpstrFilter = mFilterstring.GetCString();
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = buffer;
	ofn.nMaxFile = 255;
	ofn.lpstrFileTitle = 0;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = 0;
	ofn.lpstrTitle = mTitle.GetCString();
	ofn.Flags = 0;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = 0;
	ofn.lCustData = 0;
	ofn.lpfnHook = 0;
	ofn.lpTemplateName = 0;

	//		ofn.pvReserved = 0;
	//		ofn.dwReserved = 0;
	//		ofn.FlagsEx = 0;
	unsigned int result = GetOpenFileName(&ofn);
	return WString(buffer);
}
