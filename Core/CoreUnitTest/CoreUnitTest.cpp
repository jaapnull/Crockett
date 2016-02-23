// CoreUnitTest.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <CFile/File.h>
#include <CCore/Array.h>
#include <CCore/String.h>
#include <CUtils/StringUtils.h>
#include <CReflection/Reflection.h>

class Test
{
public:
				Test(const Test& inSource)				{ mIdentifier = inSource.mIdentifier; sAlive++; }
				Test(int inID) : mIdentifier(inID)		{ sAlive++; }
				Test() : mIdentifier(0)					{ sAlive++; }
				~Test()									{ sAlive--; }
	void		Inspect(ObjectInspector& ioI)			{ ioI.Inspect(mIdentifier, "Identifier"); }
	void		operator=(const Test& inSource)			{ mIdentifier= inSource.mIdentifier; }
	bool		operator==(const Test& inSource) const	{ return mIdentifier == inSource.mIdentifier; }
	static int	sAlive;
private:
	int			mIdentifier;
};

int Test::sAlive		= 0;


int main(int argc, char* argv[])
{
	ReflectionHost::sGetReflectionHost().RegisterClassType<Test>();
	const CompoundReflectionInfo* cfi = ReflectionHost::sGetReflectionHost().FindClassInfo<Test>();


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
		std::cout << '|' << p << '|'<< std::endl;

	File f;
	f.Open("./test.txt", fomRead);
	size64 file_length = f.GetLength();
	String file_contents;
	file_contents.Resize(file_length);
	f.GetBytes(file_contents.GetData(), file_length);

	std::cout << '|' << file_contents << '|' << std::endl;




}
