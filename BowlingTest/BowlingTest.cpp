
#include <CCoreTest.h>

#ifdef WIN32_IS_INCLUDED
#error Windows Header Slip
#endif

extern int gBowlingTest();
extern int gSudokuTest();
extern int gAdvent1a();
extern int gAdvent1b();
extern int gAdvent2a();
extern int gAdvent2b();
extern int gAdvent3a();
extern int gAdvent3b();
extern int gAdvent4();
extern int gAdvent5();
extern int gAdvent6();
extern int gAdvent7();
extern int gAdvent8();
extern int gAdvent9();
extern int gAdvent10();
extern int gAdvent11();
extern int gAdvent12();
extern int gAdvent13();
extern int gAdvent14();
extern int gAdvent15();
extern int gAdvent16();
extern int gAdvent17();
extern int gAdvent18();
extern int gAdvent20();



int main()
{

	gCoreUnitTest();

	int i = gAdvent20();
	return i;
}

