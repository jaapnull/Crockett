// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CCore/Array.h"
#include "CCore/String.h"
#include "CUtils/StringUtils.h"


class Test
{
public:

	Test(const Test& inSource)
	{
		mIdentifier = inSource.mIdentifier;
		sAlive++;
	}

	void operator=(const Test& inSource) 
	{
		mIdentifier= inSource.mIdentifier;
	}

	bool operator==(const Test& inSource) const
	{
		return mIdentifier == inSource.mIdentifier;
	}

	Test(int inIdentifier) : mIdentifier(inIdentifier)
	{
		sAlive++;
	}

	Test() : mIdentifier(0)
	{ 
		sAlive++; 
	}

	~Test() 
	{ 
		sAlive--; 
	}
	static int		sAlive;

private:
	int				mIdentifier;
};

int Test::sAlive		= 0;


int main(int argc, char* argv[])
{
	Array<Test> a;
	Array<Test> b;

	a.Reserve(3);
	gAssert(Test::sAlive == 0);
	a.Resize(1);
	a[0] = Test(1);
	gAssert(Test::sAlive == 1);
	for (int i = 2; i <= 5; i++)
		a.Append(Test(2));
	gAssert(Test::sAlive == 5);
	a.Reserve(6);
	gAssert(Test::sAlive == 5);
	a.Resize(2);
	gAssert(Test::sAlive == 2);
	b = a;
	gAssert(Test::sAlive == 4);
	b.Resize(11);
	gAssert(Test::sAlive == 13);


	String s(".teS..T123.");
	s.Append(".abc()_+..");
	gToUpper(s);
	gAssert(s == ".TES..T123..ABC()_+..");
	gToLower(s);
	gAssert(s == ".tes..t123..abc()_+..");
	Array<String> parts;
	gExplodeString<char>(parts, s, '.');
	gExplodeString<char>(parts, "111", '.');
	gExplodeString<char>(parts, ".11.1", '.');
	gAssert(parts.GetLength() == 6);
	gAssert(parts[0] == "tes");
	gAssert(parts[1] == "t123");
	gAssert(parts[2] == "abc()_+");
	gAssert(parts[3] == "111");
	gAssert(parts[4] == "11");
	gAssert(parts[5] == "1");

	for (const String& p : parts)
		std::wcout << '|' << p << '|'<< std::endl;



}
