// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <PCHCoreUnitTest.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/Serializing.h>
#include <CFile/File.h>
#include <CMath/Math.h>


class TestMemberClass
{
public:
	TestMemberClass() : mZand(sCreateCount), mPoep(sCreateCount + 1)
	{
		sCreateCount += 2;
	}

	void Inspect(ObjectInspector& inInspector)
	{
		inInspector.Inspect(mPoep, "Poep");
		inInspector.Inspect(mZand, "Zand");
	}

	int					mPoep;
	int					mZand;
	static int			sCreateCount;
};

class TestClass
{
public:

	TestClass() : mHallo(sCreateCount), mDag(sCreateCount+1)
	{
		sCreateCount+=2;
		mChildren.Append(TestMemberClass());
		mChildren.Append(TestMemberClass());
		mChildren.Append(TestMemberClass());

		mGetallen.Append(gRand() % 10);
		mGetallen.Append(gRand() % 10);
		mGetallen.Append(gRand() % 10);
	}

	void Inspect(ObjectInspector& inInspector)
	{
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
	ReflectionHost::sGetReflectionHost().RegisterClassType<TestMemberClass>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<TestClass>();
	Array<TestClass> tc;

	tc.Resize(20);
	tc.Reserve(25);

	tc[2].mHallo = 678;
	tc[1].mHallo = 345;
	tc[0].mHallo = 123;
	tc.Resize(3);
	{
		TypedPointer tp = gInspectObject(tc);
		TypedPointer pp = tp.GetObjectAtPath("[2].Children[1].Poep");
		std::cout << "ttttt" << std::endl;

	}

	/*
	{
		TestClass c;
		TypedPointer p = gInspectObject(c);
		TypedPointer m = p.GetCompoundMember("Hallo");
		std::cout << ((int*)m.mPointer)[0] << std::endl;
		m = p.GetCompoundMember("Dag");
		std::cout << ((int*)m.mPointer)[0] << std::endl;
	}
	*/

	File f;
	f.Open("./test.txt", fomWriteDiscard);
	TypedPointer tp = gInspectObject(tc);
	ObjectStreamer os(f);
	os.WriteInstance(tp);
	f.Close();

	




	f.Open("./test.txt", fomRead);
	size64 size = f.GetLength();
	String ff; ff.Resize(size);
	f.GetBytes(ff.GetData(), size);
	f.Close();
	std::cout << ff<< std::endl;
	return 0;
}



