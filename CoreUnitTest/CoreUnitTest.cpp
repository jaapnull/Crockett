// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <CoreUnitTestPCH.h>
#include <CCore/String.h>
#include <CCore/RefCount.h>
#include <CReflection/Reflection.h>
#include <CResource/ObjectCollection.h>
#include <CUtils/Trace.h>
#include <CUtils/sort.h>
#include <CReflection/ObjectWriter.h>
#include <CReflection/ObjectReader.h>
#include <CReflection/Tokenizer.h>
#include <CFile/File.h>
#include <CMath/Math.h>

class Node;
class BranchNode;


class Node
{
public:
	virtual void	Print(uint inIndent) {} 
	String			mName;
	String			mLocation;

	void Inspect(ObjectInspector& inInspector)
	{
		inInspector.Inspect(mName, "!name");
		inInspector.Inspect(mLocation, "!location");
	}
};

class LeafNode : public Node
{
public:
	LeafNode() {}
	LeafNode(const String& inText) : mText(inText) {}

	void Inspect(ObjectInspector& inInspector)
	{
		Node::Inspect(inInspector);
		inInspector.Inspect(mText, "LeafText");
	}

	virtual void Print(uint inIndent)
	{
		std::cout << String(inIndent, ' ') << mText << std::endl;
	}

	String mText;
};

class BranchNode : public Node
{
public:
	BranchNode(const String& inText) : mText(inText) {}
	BranchNode() {}
	void Inspect(ObjectInspector& inInspector)
	{
		Node::Inspect(inInspector);
		inInspector.Inspect(mText,   "BranchText");
		inInspector.Inspect(mChildA, "ChildA");
		inInspector.Inspect(mChildB, "ChildB");
	}

	virtual void Print(uint inIndent)
	{
		String s(inIndent, ' ');
		std::cout << s << mText << std::endl;
		std::cout << s << '{' << std::endl;
		if (mChildA != nullptr) mChildA->Print(inIndent + 1);
		if (mChildB != nullptr) mChildB->Print(inIndent + 1);
		std::cout << s << '}' << std::endl;
	}

	String mText;
	Node* mChildA;
	Node* mChildB;
};

class Poop : public RefObject<Poop>
{
public:
	Poop() { std::cout << "CREATED POOP" << std::endl; }

	~Poop() { std::cout << "DELETED POOP" << std::endl; }
};

int main()
{

	Test_Sort();

	gDevices.RegisterDevice(new FileDevice("data", "."));
	gDevices.RegisterDevice(new FileDevice("folder", ".\\TestFolder"));

	// Makes all output show up in debug output screen
	TraceStream<char>::sHookStream(std::cout);
	TraceStream<wchar_t>::sHookStream(std::wcout);

	// Register classes for reflection
	ReflectionHost::sGetReflectionHost().RegisterClassType<Node>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<LeafNode>();
	ReflectionHost::sGetReflectionHost().RegisterClassType<BranchNode>();


	Array<Node*> nodes;
	int idx = 0;
	for (int x = 0; x < 10; x++)
	{
		Node* node = new LeafNode(String("leaf_text") + gToString(x));
		node->mLocation = String("data@outputfile") + gToString(gRand() % 4) + String(".txt");
		node->mName = String("L") + gToString(idx++);
		nodes.Append(node);
	}

	idx = 0;
	for (int x = 0; x < 20; x++)
	{
		BranchNode* branch = new BranchNode(String("branch_text") + gToString(x));
		branch->mLocation = String("data@outputfile") + gToString(gRand() % 4) + String(".txt");
		branch->mName = String("B") + gToString(idx++);

		branch->mChildA = (Node*) nodes[gRand()%nodes.GetLength()];
		branch->mChildB = (Node*) nodes[gRand()%nodes.GetLength()];
		nodes.Append(branch);
	}


	ObjectCollection oc_write;
	for (Node* n : nodes)
	{
		oc_write.AddObject(n);
	}
	oc_write.SaveToStreams();


	ObjectCollection oc_read;
	oc_read.LoadFromStream("data@outputfile0.txt");

	std::cout << "WRITE TO DISK" << std::endl;
	for (TypedCompoundPointer tc : oc_write.GetObjects())
	{
		((Node*)tc.mPointer)->Print(0);
	}

	std::cout << "READ FROM DISK" << std::endl;
	for (TypedCompoundPointer tc : oc_read.GetObjects())
	{
		((Node*)tc.mPointer)->Print(0);
	}

	//
	//TestClass* to = oc_read.FindObject<TestClass>("TestObject0");
	//TestClass* eo = oc_read.FindObject<TestClass>("ExternObject0");

	RefPtr<Poop> poop1 = new Poop();
	std::cout << "1" << std::endl;
	RefPtr<Poop> poop2 = poop1;
	std::cout << "2" << std::endl;
	poop1 = nullptr;
	std::cout << "3" << std::endl;
	poop2 = nullptr;
	std::cout << "4" << std::endl;
	return 0;
}



