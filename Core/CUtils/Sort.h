#pragma once
#include <CMath/Math.h>
// Three-way quick-sort

// based on Bentley-McIlroy 3-way partitioning, (with non-random pivot)
// https://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf

// data layout:
//
 //                       q1 <-    <- p1
//                           |      |
//       |      |            |      |
// eq_lo |  lo  |  unsorted  |  hi  | eq_hi
//       |      |            |      |
//       |      |
//    p0 ->    -> q0

// pivot is implicitly set as the 0th element, pre-sorted into the eq_lo span
// we swap out the middle element to the beginning as pivot, to make sure that inverse sorted sets do not hit the pathelogical recursive case (recurse depth = n)

// five memory spans:
// 1) eq_lo starts at 0 and will hold elements equal to pivot (EQ); will be holding 1 element at entry, being the pivot
// 2) eq_hi starts at p1 and will ALSO holds elements equal to pivot (EQ), will start out empty
// 3) unsorted starts at q0 and holds unsorted data
// 4) lo starts at p0 and holds all elements lower than pivot (LO), and starts out empty
// 5) hi starts at p1 and holds all elements higher than pivot (HI), and starts out empty

//A) q0 and q1 will slowly move towards the middle, swapping values between them until they meet - this effectively grows lo and hi towards each other
//B) Whenever a value is added to lo or hi span that equals pivot, a second swap is done to place it against the eq_hi or eq_lo span, then moving q1 or p1 to effectively move them into one of the eq spans
//Ca) When there is no unsorted data left, Cb) the eq_hi and eq_lo are swapped with hi and lo respectively, effectively merging the eq span and leaving three spans, less/equal/greater than pivot
//D) Recursively, the less and greater spans are sorted with recursively with gSort

// placing this helpful function here for the time being
template<typename T> void gSwap(T& A, T& B)
{ 
	T t(A);
	A = B;
	B = t;
}

// A simple, stateless functor object that simply calls the < operator between entries
template<typename T>
class SimpleFunctor
{
public:
	bool operator()(const T& inLeft, const T& inRight) { return inLeft < inRight; }
};


template<typename T, typename SmallerThanFunctor = SimpleFunctor<T>>
void gSort(T* inBegin, T* inEnd, SmallerThanFunctor& inFunctor = SmallerThanFunctor())
{
	// make the middle one pivot, just to have inverse sorted not be the pathological case in terms of recursion (recursion depth = n)
	gSwap(*inBegin, inBegin[(inEnd-inBegin)/2]);

	uint32 size = uint32 (inEnd-inBegin);
	
	// early outs (useful?)
	if (size <= 1)
		return;
	if (size <= 3)
	{
		if (size == 2)
		{
			if (inFunctor(inBegin[1], inBegin[0])) gSwap(inBegin[0], inBegin[1]);
		}
		if (size == 3)
		{
			if (inFunctor(inBegin[1], inBegin[0])) gSwap(inBegin[0], inBegin[1]);
			if (inFunctor(inBegin[2], inBegin[0])) gSwap(inBegin[0], inBegin[2]);
			if (inFunctor(inBegin[2], inBegin[1])) gSwap(inBegin[1], inBegin[2]);
		}
		return;
	}

	// move low to high
	T* p0 = inBegin;		// End of eq_lo / startof lo
	T* q0 = inBegin;		// End of lo / startof unsorted

	// move from high to low
	T* q1 = inEnd;			// End of eq_hi / startof hi
	T* p1 = inEnd;			// End of hi / startof unsorted

	while(true)
	{
		// A) move two cursors outside-in and find two values that need to swap/are on the wrong side
		while (inFunctor(*(++q0), *inBegin) && q0 != inEnd);
		while (inFunctor(*inBegin, *(--q1))); 
		// Ca) no swap found before the cursors intersect?, that means it is sorted; can exit; cursors hold "mid point"
		if (q0 >= q1) break;
		// Acont) do the swap
		gSwap(*q0, *q1);
		// B) if one of the swapped values equals the pivot, move them to the outside (left/right) of the array
		if (!inFunctor(*q0, *inBegin))	{ p0++; gSwap(*p0, *q0); }
		if (!inFunctor(*inBegin, *q1))	{ p1--; gSwap(*p1, *q1); }
	}

	// move pointers "out" because they are now effectively crossed (or are pointing to EQ entries between them) and we want them within the the lo/hi spans
	q0--;
	q1++;
	
	// Cb) swap left and right spans part to get the eq_hi and eq_lo merged
	// TODO swap is doing too much work when amount of eq_hi/eq_lo is larger than hi/lo, ending up swapping within an unordered span
	for (T* k = inBegin; k <= p0; k++, q0--) 
		gSwap(*k, *q0);
	for (T* k = (inEnd-1); k >= p1; k--, q1++)
		gSwap(*q1, *k);

	// D) further sort the side spans
	gSort(inBegin, q0+1, inFunctor);
	gSort(q1, inEnd, inFunctor);
}




void Test_Sort()
{
	const int sort_size = 20;
	int sort_range = 0;
	for (int ttry = 0; ttry < 1000; ttry++)
	{
		Array<uint32> sorts;
		sorts.Reserve(sort_size);

		sort_range = gRand()%200 + 1;

		for (int i = 0; i < sort_size; i++)
		{
			if (ttry == 0)
				sorts.Append(i);
			else if (ttry == 1)
				sorts.Append(sort_size - i);
			else if (ttry == 2)
				sorts.Append(0);
			else
				sorts.Append(gRandRange(0, sort_range));
		}
		
		struct InverseFunctor
		{
			bool operator()(const int& A, const int& B) { return A > B; }
		} inv;

		gSort(sorts.GetData(), sorts.end(),inv);

		for (int i = 0; i < sort_size-1; i++)
		{
			gAssert(sorts[i] >= sorts[i+1]);
		}

		gSort(sorts.GetData(), sorts.end());

		for (int i = 0; i < sort_size-1; i++)
		{
			gAssert(sorts[i] <= sorts[i+1]);
		}

		//struct InverseFunctor
		//{
		//	bool operator()(const int& A, const int& B) { return A > B; }
		//};

		//InverseFunctor inverse_functor;
		//Array<uint32> old_sorts = sorts;
		//
		//gSort(sorts.GetData(), sorts.end(), inverse_functor);

		

		//for (int i = 0; i < sort_size-1; i++)
		//{
		//	gAssert(sorts[i] >= sorts[i+1]);
		//}
	}
}

/*
template<typename T>
void gSortWorking(T* inData, uint32 inSize)
{
	gAssert(inSize > 0);
	if (inSize <= 1)
		return;
	// data layout:
	//           ptr_hi v  v hi_sm
	//      |  |        |  |     
	// eq_sm|lo|unsorted|hi|eq_hi
	//      |  |        |  |     
	// lo_sm^  ^ptr_lo

	// pivot is implicitly set as the 0th element, pre-sorted into the sm_eq span
	T& pivot = inData[0];
	T p = pivot;
	// five memory spans:
	// 1) eq_sm starts at 0 and will hold elements equal to pivot; will be holding 1 element at entry, being the pivot
	// 2) eq_hi starts at hi_sm and will ALSO holds elements equal to pivot, will start out empty
	// 3) unsorted starts at ptr_lo and holds unsorted data
	// 4) lo starts at lo_sm and holds all elements lower than pivot, and starts out empty
	// 5) hi starts at lo_sm and holds all elements higher than pivot, and starts out empty

	// ptr_lo and ptr_hi will slowly move towards the middle, swapping values between them until they meet - this effectively grows lo and hi towards each other
	// Whenever a value is added to lo or hi area that equals pivot, a second swap is done to place it in/against the eq_hi or eq_sm area (updating hi_sm / lo_sm)
	// When there is no unsorted data left, the eq_hi and eq_lo are swapped with hi and lo respectively, effectively merging the eq areas and leaving three spans, less/equal/greater than pivot
	// Recursively, the less and greater spans are sorted with recursively with gSort
	
	// starting (first_value_idx) of
	int lo_sm = 0;
	int hi_sm = inSize;
	int ptr_lo = 0; // we start with low pointer on pivot
	int ptr_hi = inSize;

	while(true)
	{
		// move two cursors outside-in and find two values that need to swap/are on the wrong side
		while (inData[++ptr_lo] < pivot && ptr_lo < int(inSize));
		while (inData[--ptr_hi] > pivot); 
		// no swap found before the cursors intersect?, that means it is sorted; can exit; cursors hold "mid point"
		if (ptr_lo >= ptr_hi) break;
		// do the swap
		gSwap(inData[ptr_lo], inData[ptr_hi]);
		// if one of the swapped values equals the pivot, move them to the outside (left/right) of the array
		if (inData[ptr_lo] == pivot) { lo_sm++; gSwap(inData[lo_sm], inData[ptr_lo]); }
		if (inData[ptr_hi] == pivot) { hi_sm--; gSwap(inData[hi_sm], inData[ptr_hi]); }
		gAssert(lo_sm < (int) inSize);
	}

	int r = inSize;
	int j = ptr_lo-1;
	int i = ptr_hi+1;
	int pp = lo_sm+1;
	int qq = hi_sm-1;


	for (int k = 0; k < pp; k++, j--) 
		gSwap(inData[k], inData[j]);
	for (int k = r-1; k > qq; k--, i++)
		gSwap(inData[i], inData[k]);

	for (int z = 0; z <= j; z++) 		gAssert(inData[z] < p);
	for (int z = j+1; z < i; z++)		gAssert(inData[z] == p);
	for (int z = i; z < r; z++)			gAssert(inData[z] > p);

	if (j > 0)
		gSortWorking(inData, j+1);

	for (int z = 0; z < j; z++)		gAssert(inData[z] <= inData[z+1]);

	if (r-i > 1)
		gSortWorking(inData + i, r-i);

	for (int z = i; z < r-i-1; z++)		gAssert(inData[z] <= inData[z+1]);
}
*/

/*
template<typename T>
void gSort(T* inData, uint32 inSize)
{
	gAssert(inSize > 0);

	if (inSize <= 3)
	{
		if (inSize == 2)
		{
			if (inData[1] < inData[0]) gSwap(inData[0], inData[1]);
		}
		if (inSize == 3)
		{
			if (inData[1] < inData[0]) gSwap(inData[0], inData[1]);
			if (inData[2] < inData[0]) gSwap(inData[0], inData[2]);
			if (inData[2] < inData[1]) gSwap(inData[1], inData[2]);
		}
		return;
	}
	// data layout:
	//           ptr_hi v  v hi_sm
	//      |  |        |  |     
	// eq_sm|lo|unsorted|hi|eq_hi
	//      |  |        |  |     
	// lo_sm^  ^ptr_lo

	// pivot is implicitly set as the 0th element, pre-sorted into the sm_eq span
	T& pivot = inData[0];
	T p = pivot;
	// five memory spans:
	// 1) eq_sm starts at 0 and will hold elements equal to pivot; will be holding 1 element at entry, being the pivot
	// 2) eq_hi starts at hi_sm and will ALSO holds elements equal to pivot, will start out empty
	// 3) unsorted starts at ptr_lo and holds unsorted data
	// 4) lo starts at lo_sm and holds all elements lower than pivot, and starts out empty
	// 5) hi starts at lo_sm and holds all elements higher than pivot, and starts out empty

	// ptr_lo and ptr_hi will slowly move towards the middle, swapping values between them until they meet - this effectively grows lo and hi towards each other
	// Whenever a value is added to lo or hi area that equals pivot, a second swap is done to place it in/against the eq_hi or eq_sm area (updating hi_sm / lo_sm)
	// When there is no unsorted data left, the eq_hi and eq_lo are swapped with hi and lo respectively, effectively merging the eq areas and leaving three spans, less/equal/greater than pivot
	// Recursively, the less and greater spans are sorted with recursively with gSort
	
	// starting (first_value_idx) of
	int lo_sm = 0;
	int hi_sm = inSize;
	int ptr_lo = 0; // we start with low pointer on pivot
	int ptr_hi = inSize;

	while(true)
	{
		// move two cursors outside-in and find two values that need to swap/are on the wrong side
		while (inData[++ptr_lo] < pivot && ptr_lo < int(inSize));
		while (inData[--ptr_hi] > pivot); 
		// no swap found before the cursors intersect?, that means it is sorted; can exit; cursors hold "mid point"
		if (ptr_lo >= ptr_hi) break;
		// do the swap
		gSwap(inData[ptr_lo], inData[ptr_hi]);
		// if one of the swapped values equals the pivot, move them to the outside (left/right) of the array
		if (inData[ptr_lo] == pivot) { gAssert(!(inData[ptr_lo] > pivot)); lo_sm++; gSwap(inData[lo_sm], inData[ptr_lo]); }
		if (inData[ptr_hi] == pivot) { gAssert(!(inData[ptr_hi] < pivot)); hi_sm--; gSwap(inData[hi_sm], inData[ptr_hi]); }
		gAssert(lo_sm < (int) inSize);
	}

	ptr_lo--;
	ptr_hi++;
	
	for (int k = 0; k <= lo_sm; k++, ptr_lo--) 
		gSwap(inData[k], inData[ptr_lo]);
	for (int k = inSize-1; k >= hi_sm; k--, ptr_hi++)
		gSwap(inData[ptr_hi], inData[k]);

	if (ptr_lo > 0)
		gSort(inData, ptr_lo+1);

	if (inSize-ptr_hi > 1)
		gSort(inData + ptr_hi, inSize-ptr_hi);

}
*/


/*
ref. implementation from
https://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf

void quicksort(Item a[], int l, int r)
{ int i = l-1, j = r, p = l-1, q = r; Item v = a[r];
if (r <= l) return;
for (;;)
{
while (a[++i] < v) ;
while (v < a[--j]) if (j == l) break;
if (i >= j) break;
exch(a[i], a[j]);
if (a[i] == v) { p++; exch(a[p], a[i]); }
if (v == a[j]) { q--; exch(a[j], a[q]); }
}
exch(a[i], a[r]); j = i-1; i = i+1;
for (k = l; k < p; k++, j--) exch(a[k], a[j]);
for (k = r-1; k > q; k--, i++) exch(a[i], a[k]);
quicksort(a, l, j);
quicksort(a, i, r);
}

*/



