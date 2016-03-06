// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <PCHCoreUnitTest.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
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

class TestClass : public Resource
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
	ReflectionHost::sGetReflectionHost().RegisterClassType<Resource>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<TestMemberClass>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<TestClass>();

	TestClass tc;
	tc.mName = "mytest";


	Resource* r = &tc;
	TypeDecl type = gInspectObject(r);
	std::cout << type.ToString() << std::endl;

	Array<Resource*> ra;
	type = gInspectObject(ra);
	std::cout << type.ToString() << std::endl;

	Array<Resource**>* pra;
	type = gInspectObject(pra);
	std::cout << type.ToString() << std::endl;



	File f;
	f.Open("./test.txt", fomWriteDiscard);
	ObjectWriter os(f);
	os.WriteResource(tc);
	f.Close();

	f.Open("./test.txt", fomRead);
	size64 size = f.GetLength();
	String ff; ff.Resize(size);
	f.GetBytes(ff.GetData(), size);
	f.Close();
	std::cout << ff<< std::endl;
	return 0;
}



