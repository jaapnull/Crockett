// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <PCHCoreUnitTest.h>
#include <CCore/String.h>
#include <CReflection/Reflection.h>
#include <CUtils/Trace.h>
#include <CReflection/ObjectWriter.h>
#include <CReflection/Tokenizer.h>
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

	//TestClass tc0;
	//String* poep	= gGetDebugField<String>(tc0, String("!name"));

	//TestClass tc1;
	//tc1.mName		= "klaas";
	//tc1.mLocation	= "Flop.txt";
	//
	//tc0.mSibling = &tc1;

	//Resource* r = &tc0;
	//TypedPointer typed_pointer = gInspectObject(r);
	//std::cout << typed_pointer.mType.ToString() << std::endl;
	//
	//Array<Resource*> ra;
	//typed_pointer = gInspectObject(ra);
	//std::cout << typed_pointer.mType.ToString() << std::endl;
	//
	//Array<Resource**>* pra;
	//typed_pointer = gInspectObject(pra);
	//std::cout << typed_pointer.mType.ToString() << std::endl;

	//File f;
	//f.Open("./test.txt", fomWriteDiscard);
	//ObjectWriter ow(f);
	//ow.WriteResource(tc0, true);
	//f.Close();



	Array<TestMemberClass> tmc;

	TypedArrayPointer tap = gInspectObject(tmc);

	TypedPointer item = tap.CreateNewArrayItem();

	int* i = TypedCompoundPointer(item).GetCompoundMember<int>("Poep");

	*i = 10;

	item = tap.CreateNewArrayItem();
	int* j = TypedCompoundPointer(item).GetCompoundMember<int>("Poep");
	*j = 20;

	item = tap.CreateNewArrayItem();
	int* k  = TypedCompoundPointer(item).GetCompoundMember<int>("Poep");
	*k = 30;

	//f.Open("./test.txt", fomRead);
	//TokenReader reader;
	//reader.SetStream(f);
	//char buffer[100];
	//EStreamTokenType type;
	//while (int r = reader.GetToken(type, buffer, 100))
	//{
	//	buffer[r] = '\0';
	//	std::wcout << type << ": " << buffer << std::endl;
	//}
	//
	//f.Close();

	return 0;
}



