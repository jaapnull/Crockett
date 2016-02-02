// Memory util functions; at the moment builds upon RTL malloc functions
#include "../CCore/types.h"


/// Allocated piece of memory of [inDataSize] bytes, aligned on [inAlignment] bytes
inline void* gAllocAligned(size64 inDataSize, size64 inAlignment)
{
	return _aligned_malloc(inDataSize, inAlignment);
}

/// Templated wrapper to allocate enough memory to hold [inElementCount] objects of type T, aligned to the natural alignment of T
template<typename T>
T* gAllocAligned(size64 inElementCount)
{
	return (T*)_aligned_malloc(sizeof(T)*inElementCount, __alignof(T));
}

/// Templated wrapper to realloc to memory to hold [inElementCount] objects of type T, aligned to the natural alignment of T
template<typename T>
T* gReallocAligned(void* inPointer, uint inCount)
{
	return _aligned_realloc(inPointer, inCount*size(T), __alignof(T));
}

/// Frees aligned allocation of buffer [inPointer]
inline void gFreeAligned(void* inPointer)
{
	_aligned_free(inPointer);
}

/// Checks of a buffer is aligned to [inAlignment] (uses templates to allow compiler to optimize out 2^n alignment checks)
template <uint64 inAlignment>
inline bool gIsAlignedTo(void* inPointer)
{
	return size64(inPointer) % inAlignment == 0;
}

/// template helper to offset [T]pointer by [inBytes] bytes
template <class T>
inline T* gOffsetPointer(T* inPointer, offset64 inBytes)
{
	// check if offset and 
	T* offset_pointer = reinterpret_cast<T*>(uint64(inPointer) + inBytes);
	gAssert(gIsAlignedTo<__alignof(T)>(offset_pointer));
	return offset_pointer;
}

/// template helper to offset void pointer by [inBytes] bytes
template <>
inline void* gOffsetPointer<void>(void* inPointer, size64 inBytes)
{
	// check if offset and 
	void* offset_pointer = reinterpret_cast<void*>(uint64(inPointer) + inBytes);
	return offset_pointer;
}


// ------------------------------------------------------------------------------------------------------------
// Static util class with the (re)allocation and resizing of linear element arrays
// An allocation consists of three main properties:
// This allocator should fully call proper ctor/dtor on all elements at all times
// No objects will be implicitely moved in memory
// ------------------------------------------------------------------------------------------------------------
// The block of memory, aligned and sized as a multiple of sizeof(T)		[Data]
// The amount of initialized elements of type T								[ActiveCount]
// The amount of unitialized, preallocated/reserved elements of type T		[AllocCount]
// ------------------------------------------------------------------------------------------------------------
template<typename T>
class LinearAllocator
{
public:
	// --------------------------------------------------------------------------------------------------------
	// Raw memory alloc of [inElemCount] elements of type T
	// Returns block of aligned memory
	// --------------------------------------------------------------------------------------------------------
	static T* sRawAlloc(size64 inElementCount)	// allocs memory for T without CTOR
	{
		return (inElementCount == 0) ? 0 : (T*) gAllocAligned<T>(inElementCount);
	}

	// --------------------------------------------------------------------------------------------------------
	// Raw memory free of data allocated with sRawAlloc
	// --------------------------------------------------------------------------------------------------------
	static void sRawFree(T* inData)			// frees memory for T without DTOR
	{
		assert(inData != 0);
		gFreeAligned(inData);
	}

	// --------------------------------------------------------------------------------------------------------
	// Realloc set of items in memoryblock [inPrevData] to a new memory block of size [inNewCount]
	// It destruct/constructs [inPrevElementCount] into the new data
	// Returns a new block of data of size [inNewAllocCount]
	// This does _NOT_ use the realloc() function because afaik there is no proper way of predicting if realloc 
	// actually moves data around until after the fact (and the original memory has been freed)
	// This could cause objects to be moved in memory without them being aware of it until after the fact
	// Instead we will always take the "long way around" re-alloc at all times, with full destruction/construction 
	// of all elements
	// --------------------------------------------------------------------------------------------------------
	static T* sRealloc(T* inPrevData, size64 inActiveCount, size64 inNewAllocCount)
	{
		assert(inNewAllocCount > 0);								// cannot realloc a zero array to zero because by definition a zero-length allocation would be a null pointer
		assert(inActiveCount <= inNewAllocCount);					// sRealloc cannot realloc to a smaller space because it does by definition only move, not delete objects (no move operator as of yet)
		assert(inPrevData != 0 || inActiveCount == 0);				// previous elements should be zero only when 
		
		if (inActiveCount == inNewAllocCount) return inPrevData;	// no size change means we can early out
		T* new_array = sRawAlloc(inNewAllocCount);					// allocate array of bytes with appropriate new size
		for (size64 c = 0; c < inActiveCount; c++)					// Move over all active elements to new allocated buffer, destroying them at old location
		{
			new (new_array + c) T(inPrevData[c]);					// placement new copy-constructor (no assingment op used))
			inPrevData[c].~T();										// destruct old instance
		}
		if (inPrevData != 0) sRawFree(inPrevData);					// All elements moved over, so no implicit destructors needed, a straight memory free
		return new_array;											// Return new Array
	}

	// --------------------------------------------------------------------------------------------------------
	// A straight forward alloc of [inNewAllocCount].
	// All elements are constructed in place with void constructor.
	// --------------------------------------------------------------------------------------------------------
	static T* sAllocAndConstruct(size64 inNewAllocCount)
	{
		assert(inNewAllocCount > 0);					// cannot alloc zero size
		T* new_array = sRawAlloc(inNewAllocCount);		// allocate array of bytes with appropriate size
		for (size64 c = 0; c < inNewAllocCount; c++)		// Construct new elements	
		{
			new (new_array + c) T();						// placement new void constructor
		}
		return new_array;								// return newly created array of constructed elements
	}

	// --------------------------------------------------------------------------------------------------------
	// Straight forward free: frees memory and destructs [inElementCount] objects that reside in it
	// --------------------------------------------------------------------------------------------------------
	static void sFreeAndDestruct(T* inData, size64 inElementCount)
	{
		assert(inData != 0);							// cannot destruct null pointer
		for (size64 c = 0; c < inElementCount; c++)		// Delete all elements
		{
			inData[c].~T();									// Call destructor
		}
		sRawFree(inData);								// Free memory
	}

	// --------------------------------------------------------------------------------------------------------
	// Allocates a memory block and initalizes [inInitCount] from external array [inInitData]
	// Assumes inInitData has at least [inInitCount] elements
	// --------------------------------------------------------------------------------------------------------
	static T* sAllocAndCopyConstruct(size64 inNewAllocCount, const T* inInitData, size64 inInitCount)
	{
		assert(inInitCount <= inNewAllocCount);				// Cannot init with more than allocated
		assert(inInitData != 0 || inInitCount == 0);		// previous elements should be zero only when 

		T* new_array = sRawAlloc(inNewAllocCount);			// allocate array of bytes with appropriate size
		for (size64 c = 0; c < inInitCount; c++)				// Initalize elements
		{
			new (new_array + c) T(inInitData[c]);				// Placement new copy-constructor, faster than seperate construct and assign
		}
		return new_array;									// return new data block
	}

	// --------------------------------------------------------------------------------------------------------
	// Resize item list inside memory block using placement new and manual destructor calls where needed
	// This does not resize the allocated block
	// --------------------------------------------------------------------------------------------------------
	static void sResizeWithinAlloc(T* inElements, size64 inPrevCount, size64 inNewCount)
	{
		for (size64 c = inPrevCount; c < inNewCount; c++)		// iterating over new elements
		{
			new (inElements + c) T;								// placement new of new objects using void constructor
		}
		for (size64 c = inNewCount; c < inPrevCount; c++)		// iterating over elements that fall outside of new range
		{
			inElements[c].~T();									// calling destructor
		}
	}
};

