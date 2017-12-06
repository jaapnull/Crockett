#pragma once
#include <CCore/String.h>
#include <CCore/Array.h>
#include <WCommon/Window.h>

class OpenFileDialog
{
public:
	class FileFilter
	{
		public:
		WString mName;
		WString mExtension;
		FileFilter(const WString name, WString extension) :mName(name), mExtension(extension) {}		
	};
private:
	Array<FileFilter> mFilters;
	WString mTitle;
	Window& mParent;
public:

	OpenFileDialog(Window& Parent) : mParent(Parent) {}
	OpenFileDialog(Window& Parent, const WString& title) : mParent(Parent), mTitle(title) {}

	void SetTitle(const WString& newtitle) { mTitle = newtitle;}
	WString GetTitle() {return mTitle;}

	Array<FileFilter>& GetFilters() {return mFilters;}
	const Array<FileFilter>& GetFilters() const {return mFilters;}
	void AddFilter(const WString name, const WString extension)
	{
		mFilters.Append(FileFilter(name, extension));
	}

	WString RunSingle();

	~OpenFileDialog(void)
	{
	}
};
