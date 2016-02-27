// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include "PCHCoreUnitTest.h"
#include <CCore/String.h>
#include <CFile/File.h>

int main()
{

	File f;
	f.Open("./test.txt", fomRead);
	size64 size = f.GetLength();
	String ff; ff.Resize(size);
	f.GetBytes(ff.GetData(), size);
	std::wcout << ff<< std::endl;
	return 0;
}



