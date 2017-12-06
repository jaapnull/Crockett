#pragma once
/**
(c)2016 Jaap van Muijden - Sort.h - Quicksort (3way)
**/

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
	if (inBegin == inEnd) 
		return;
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
		while (++q0 != inEnd && inFunctor(*(q0), *inBegin));
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
	}
}
