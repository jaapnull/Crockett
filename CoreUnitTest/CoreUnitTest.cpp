// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <PCHCoreUnitTest.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/Serializing.h>
#include <CFile/File.h>


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

	int			mPoep;
	int			mZand;
	static int	sCreateCount;
};

class TestClass
{
public:

	TestClass() : mHallo(sCreateCount), mDag(sCreateCount+1)
	{
		sCreateCount+=2;
	}

	void Inspect(ObjectInspector& inInspector)
	{
		inInspector.Inspect(mHallo, "Hallo");
		inInspector.Inspect(mDag , "Dag");
		inInspector.Inspect(mMemberClass, "Member");
	}

	TestMemberClass mMemberClass;
	int			mHallo;
	int			mDag;
	static int	sCreateCount;
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



