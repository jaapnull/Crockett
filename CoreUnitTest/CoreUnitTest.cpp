// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <PCHCoreUnitTest.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CUtils/Trace.h>
#include <CReflection/ObjectWriter.h>
#include <CReflection/ObjectReader.h>
#include <CReflection/Tokenizer.h>
#include <CFile/File.h>
#include <CMath/Math.h>

class TestClass;

class TestMemberClass
{
public:
	TestMemberClass() : mZand(sCreateCount), mPoep(sCreateCount + 1)
	{
		sCreateCount += 2;
	}

	void Inspect(ObjectInspector& inInspector)
	{
		inInspector.Inspect(mReference, "TestClassReference");
		inInspector.Inspect(mPoep, "Poep");
		inInspector.Inspect(mZand, "Zand");
	}

	TestClass*			mReference	= nullptr;
	int					mPoep		= 0;
	int					mZand		= 0;
	static int			sCreateCount;
};

class TestClass : public Resource
{
public:

	TestClass() : mHallo(sCreateCount), mDag(sCreateCount+1)
	{
		sCreateCount+=2;
	}

	void Inspect(ObjectInspector& inInspector)
	{
		Resource::Inspect(inInspector);
		inInspector.Inspect(mHallo,		"Hallo");
		inInspector.Inspect(mDag ,		"Dag");
		inInspector.Inspect(mChildren,	"Children");
		inInspector.Inspect(mSibling,	"Sibling");
		inInspector.Inspect(mGetallen,	"Getallen");
	}

	TestClass*					mSibling;
	Array<TestMemberClass>		mChildren;
	int							mHallo;
	int							mDag;
	Array<int>					mGetallen;
	static int					sCreateCount;
};

int	TestMemberClass::sCreateCount = 0;
int	TestClass::sCreateCount = 0;


int main()
{
	// Makes all output show up in debug output screen
	TraceStream<char>::sHookStream(std::cout);
	TraceStream<wchar_t>::sHookStream(std::wcout);

	// Register classes for reflection
	ReflectionHost::sGetReflectionHost().RegisterClassType<Resource>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<TestMemberClass>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<TestClass>();


	TestClass tc1;
	tc1.mName		= "TestObject0";
	tc1.mLocation	= "test.txt";

	tc1.mChildren.Append(TestMemberClass());
	tc1.mChildren.Append(TestMemberClass());
	tc1.mChildren.Append(TestMemberClass());
	

	tc1.mGetallen.Append(gRand() % 10);
	tc1.mGetallen.Append(gRand() % 10);
	tc1.mGetallen.Append(gRand() % 10);

	TestClass tc2;
	tc2.mName		= "ExternObject0";
	tc2.mLocation	= "test2.txt";

	tc1.mChildren[2].mReference = &tc2;

	tc1.mSibling = &tc2;
	
	File f;
	f.Open("./test.txt", fomWriteDiscard);
	ObjectWriter ow(f);
	ow.WriteResource(tc1);
	f.Close();
	f.Open("./test2.txt", fomWriteDiscard);
	ow.WriteResource(tc2);
	f.Close();


	f.Open("./test.txt", fomRead);

	ObjectReader reader;

	Array<TypedPointer> file_objects;
	reader.ReadFile(f, file_objects);
	
	f.Close();


	for (const UnresolvedLink& link : reader.GetLinks())
	{
		TypedPointer obj = gInspectObject(tc1);
		std::cout << obj.ResolvePathToString(link.mReflectionPath) << std::endl;
	}

	return 0;
}



