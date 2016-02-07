// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CCore/Array.h"
#include "CCore/String.h"


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

	Test() 
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
	gAssert(Test::sAlive == 1);
	a.Resize(5);
	gAssert(Test::sAlive == 5);
	a.Reserve(6);
	gAssert(Test::sAlive == 5);
	a.Resize(2);
	gAssert(Test::sAlive == 2);
	b = a;
	gAssert(Test::sAlive == 4);
	b.Resize(11);
	gAssert(Test::sAlive == 13);


}
