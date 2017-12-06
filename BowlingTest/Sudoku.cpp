#include <CCore/String.h>
#include <CMath/Vector.h>
#include <CReflection/Reflection.h>
#include <CReflection/ObjectWriter.h>
#include <CFile/File.h>
#include <CMath/Math.h>
#include <unordered_map>

#include <WCommon/Window.h>
#include <WCommon/Canvas.h>
#include <WCommon/ColorPen.h>
#include <WCommon/Font.h>
#include <WCommon/Mouse.h>
#include <WCommon/Keyboard.h>


struct Group
{
	Group() {}
	Group(uint64 inA, uint64 inB) : mMaskA(inA), mMaskB(inB)	{}
	inline Group operator&(const Group& inOther)  const 		{ return Group(mMaskA&inOther.mMaskA, mMaskB&inOther.mMaskB); }
	inline Group operator|(const Group& inOther)  const 		{ return Group(mMaskA|inOther.mMaskA, mMaskB|inOther.mMaskB); }
	inline Group operator~() const								{ return Group(~mMaskA, ~mMaskB); }
	inline bool operator[] (uint64 inField) const				{ return inField < 64 ? ((mMaskA&(1ull<<inField)) == 0 ? false : true) : ((mMaskB&(1ull<<(inField-64))) == 0 ? false : true) ;}
	
	uint64 mMaskA = 0;
	uint64 mMaskB = 0;


};

void gPrintLine(const Group& inGroup, uint inOffset)
{
	std::cout << "|" << (inGroup[inOffset + 0] ? "##" : "  ") << (inGroup[inOffset + 1] ? "##" : "  ") << (inGroup[inOffset + 2] ? "##" : "  ");
	std::cout << "|" << (inGroup[inOffset + 3] ? "##" : "  ") << (inGroup[inOffset + 4] ? "##" : "  ") << (inGroup[inOffset + 5] ? "##" : "  ");
	std::cout << "|" << (inGroup[inOffset + 6] ? "##" : "  ") << (inGroup[inOffset + 7] ? "##" : "  ") << (inGroup[inOffset + 8] ? "##" : "  ") << '|' << std::endl;
}



void gPrintGroup(const Group& inGroup)
{
	std::cout << ".------.------.------." << std::endl;
	gPrintLine(inGroup, 0);
	gPrintLine(inGroup, 9);
	gPrintLine(inGroup, 18);
	std::cout << "|------+------+------|" << std::endl;
	gPrintLine(inGroup, 27);
	gPrintLine(inGroup, 36);
	gPrintLine(inGroup, 45);
	std::cout << "|------+------+------|" << std::endl;
	gPrintLine(inGroup, 54);
	gPrintLine(inGroup, 63);
	gPrintLine(inGroup, 72);
	std::cout << "'------'------'------'" << std::endl;
}

int gSudokuTest()
{
	uint32 field[81] = { 0 };
	Group rows[9] = {
		{ 0x1FFull <<  0,	0x000		},
		{ 0x1FFull <<  9,	0x000		},
		{ 0x1FFull << 18,	0x000		},
		{ 0x1FFull << 27,	0x000		},
		{ 0x1FFull << 36,	0x000		},
		{ 0x1FFull << 45,	0x000		},
		{ 0x1FFull << 54,	0x000		},
		{ 0x1FFull << 63,	0x1FF >>  1 },
		{ 0x000,		0x1FF <<  8 } };

	Group columns[9] = {
		{ 0x8040201'008040201, 0x000000'00000100 },
		{ 0x0080402'010080402, 0x000000'00000201 },
		{ 0x0100804'020100804, 0x000000'00000402 },
		{ 0x0201008'040201008, 0x000000'00000804 },
		{ 0x0402010'080402010, 0x000000'00001008 },
		{ 0x0804020'100804020, 0x000000'00002010 },
		{ 0x1008040'201008040, 0x000000'00004020 },
		{ 0x2010080'402010080, 0x000000'00008040 },
		{ 0x4020100'804020100, 0x000000'00010080 } };

	Group blocks[9] = {
		{ 0x1C0E07ull << 0,		0x000000ull },
		{ 0x1C0E07ull << 3,		0x000000ull },
		{ 0x1C0E07ull << 6,		0x000000ull },
		{ 0x1C0E07ull << 27,	0x000000ull },
		{ 0x1C0E07ull << 30,	0x000000ull },
		{ 0x1C0E07ull << 33,	0x000000ull },
		{ 0x1C0E07ull << 54,	0x1C0E07ull >> 10},
		{ 0x1C0E07ull << 57,	0x1C0E07ull >> 7 },
		{ 0x1C0E07ull << 60,	0x1C0E07ull >> 4 },
	};

	for (int i = 0; i < 9; i++)
		gPrintGroup(rows[i]);

	for (int i = 0; i < 9; i++)
		gPrintGroup(columns[i]);

	for (int i = 0; i < 9; i++)
		gPrintGroup(blocks[i]);

	for (int i = 0; i < 9; i++)
		gPrintGroup(rows[i] | columns[i]);


	return 1;
}