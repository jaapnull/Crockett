// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CCore/Array.h"

// Test object for recording events happening during the unittests (creation/deletion etc)
struct TestEvent
{
	enum class Type
	{
		teAssignment,
		teCopyConstruct,
		teDefaultConstruct,
		teDestroy,
		teInvalid
	};
	
	TestEvent() : mValue(-1) , mType(Type::teInvalid) {}
	TestEvent(Type inType, int inValue) 
		: mValue(inValue), 
		mType(inType) {}

	int		mValue;
	Type	mType;

};
Array<TestEvent> gEvents;

class Test
{
public:

	Test(const Test& inSource) : mCreationID(inSource.mCreationID) 
	{
		gEvents.Append(TestEvent(TestEvent::Type::teCopyConstruct, mCreationID)); 
	}

	void operator=(const Test& inSource) 
	{
		mCreationID = inSource.mCreationID;
		gEvents.Append(TestEvent(TestEvent::Type::teAssignment, mCreationID)); 
	}

	Test() 
	{ 
		mCreationID = sCreated; 
		gEvents.Append(TestEvent(TestEvent::Type::teDefaultConstruct, mCreationID)); 
		sCreated++; 
		sAlive++; 
	}
	~Test() 
	{ 
		sAlive--; 
		sDeleted++; 
		gEvents.Append(TestEvent(TestEvent::Type::teDestroy, mCreationID)); 
	}
	static int		sAlive;
	static int		sDeleted;
	static int		sCreated;
private:
	int				mCreationID;
};

int Test::sAlive		= 0;
int Test::sDeleted		= 0;
int Test::sCreated		= 0;

int main( int argc, char* argv[] )
{
	Array<Test> a;
	Array<Test> b;

	a.Reserve(6);
	a.Resize(3);
	a.Resize(8);
	a.Reserve(10);
	a.Resize(2);
	b = a;
	b.Resize(11);
	


	std::wcout << L"Test";
}
