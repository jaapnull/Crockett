// ------------------------------------------------------------------------------------------------------------
// Standard Array implementation
// ------------------------------------------------------------------------------------------------------------
#pragma once
#include "../CCore/Memory.h"
#include "../CHash/MurmurHash3.h"
#include "../CUtils/EnumMask.h"

// -----------------------------------------------------------------------------------------------------------
// Array is the base linear array implementation with all the default stuff like resizing etc.
// Uses the LinearAllocator static functions
// -----------------------------------------------------------------------------------------------------------
template<typename T, class TAllocator = LinearAllocator<T>>
class Array
{
public:
	// void constructor
	Array() : mData(0), mElementCount(0), mReservedCount(0)	{ }
	// copy constructor from another array
	Array(const Array<T>& inOther)							{ size64 l = inOther.GetLength();	mData = LinearAllocator<T>::sAllocAndCopyConstruct(l, inOther.GetData(), l);	mReservedCount = l; mElementCount = l; }
	// copy constructor from a raw data/elementcount
	Array(const T* inArray, size64 inElementcount)			{ size64 l = inElementcount;		mData = LinearAllocator<T>::sAllocAndCopyConstruct(l, inArray, l);				mReservedCount = l; mElementCount = l; }
	// destructor (only frees any allocated memory, leaves invalid state!)
	~Array()												{ if (mData) LinearAllocator<T>::sFreeAndDestruct(mData, mElementCount); }

	//static void sConstructFromExternalBuffer(void* ioArray, void* inData, size64 inElements)
	//{
	//	// use Array<byte> as placeholder to get the correct field offsets
	//	Array<byte>* dummy_array = static_cast<Array<byte>*>(ioArray);
	//	dummy_array->Clear();
	//	dummy_array->mData = (byte*)inData;
	//	dummy_array->mElementCount = inElements;
	//	dummy_array->mReservedCount = inElements;
	//}

	size64 GetLength()								const	{ return mElementCount; }					// Amount of active/initialized elements	
	size64 GetReserved()							const	{ return mReserverdCount; }					// Get Allocated elements (data size is sizeof(T)*GetReserved())
	T* GetData()											{ return mData; }							// Getter to raw data block
	const T* GetData()								const	{ return mData; }							// Const getter to raw data block

	T& At(size64 inIndex)									{ return mData[inIndex]; }					// Const getter to element
	const T& At(size64 inIndex)						const	{ return mData[inIndex]; }					// Const getter to element

	T& Back()												{ return mData[mElementCount - 1]; }		// Getter to last element
	const T& Back()									const	{ return mData[mElementCount - 1]; }		// Const getter to last element

	T& Front()												{ return mData[0]; }						// Getter to first element
	const T& Front()								const	{ return mData[0]; }						// Const getter to first element

	bool IsEmpty()									const	{ return mElementCount == 0; }				// returns true is element count is zero

	void Shrink(size64 inShrinkage)							{ Resize(mElementCount - inShrinkage); }	// Shrinks amount of elements by inShrinkage, destructing elements
	void Pop()												{ Resize(mElementCount - 1); }				// Shrinks elements by one, destructing last element

	bool operator==(const Array& inOther) const
	{
		if (GetLength() != inOther.GetLength())
			return false;

		for (size64 e = 0; e < mElementCount; e++)
		{
			if (!(mData[e] == inOther[e]))
				return false;
		}
		return true;
	}

	// --------------------------------------------------------------------------------------------------------
	// Clear entire array of elements
	// In constrast to a Resize(0), this also releases memory if inKeepMemory is false
	// --------------------------------------------------------------------------------------------------------
	void Clear(bool inKeepMemory = false)
	{
		if (inKeepMemory)
		{
			Resize(0);
		}
		else
		{
			if (mData != 0)														// If there is any allocation
			{
				gAssert(mReservedCount > 0);									// Sanity Check; reserved count should be zero
				TAllocator::sFreeAndDestruct(mData, mElementCount);				// Free allocation
			}
			else
			{
				gAssert(mReservedCount == 0);									// without allocation, reserved count should be zero
			}
			mData = 0;															// No allocation so zero mData
			mElementCount = 0;													// Element count is no zero
			mReservedCount = 0;													// Reserved count is no zero
		}
	}

	// --------------------------------------------------------------------------------------------------------
	// Resizes and copies data from [inData] of length [inLength] into buffer
	// Resizes entire array to size [inLength] (but does not shrink allocation length)
	// --------------------------------------------------------------------------------------------------------
	void Set(const T* inData, size64 inLength)
	{
		if (mData)
		{
			// resize to zero
			gAssert(mElementCount > 0);
			TAllocator::sResizeWithinAlloc(mData, mElementCount, 0);
		}
		if (mReservedCount < inLength)											// raw realloc if needed
		{
			gAssert(mElementCount == 0);										// We assume no active elements so realloc will not perform any extra work
			mData = TAllocator::sRealloc(mData, mElementCount, inLength);
			mReservedCount = inLength;											// Either way, we now have [inLength] buffer size
		}
		for (size64 c = 0; c < inLength; c++)
		{
			new (mData + c) T(inData[c]);										// Copy-construct the data inside the allocated data buffer
		}
		mElementCount = inLength;
	}

	// --------------------------------------------------------------------------------------------------------
	// Reserves a minimum of [inNewAllocCount] in the buffer, copying over any created elements to 
	// a new buffer if needed
	// --------------------------------------------------------------------------------------------------------
	void Reserve(size64 inNewReserveCount)
	{
		if (mData == 0)																			// if there is no buffer yet, make a new one
		{
			gAssert(mReservedCount== 0);														// sanity check; there cannot be reserved data yet
			mData = TAllocator::sRawAlloc(inNewReserveCount);									// Allocate new buffer without constructing anything
		}
		else if (mReservedCount != inNewReserveCount)											// otherwise simply realloc the old one
		{
			gAssert(inNewReserveCount >= mElementCount);										// Cannot resize to a smaller size than the element count; this would implicitly delete elements
			mData = TAllocator::sRealloc(mData, mElementCount, inNewReserveCount);				// Realloc existing buffer, copying existing elements
		}
		mReservedCount = inNewReserveCount;														// Either way we have resized our buffer to a new size
	}

	// --------------------------------------------------------------------------------------------------------
	// Resizes the actual allocated data inside the buffer; it will (void) construct and destruct
	// elements if needed(!)
	// --------------------------------------------------------------------------------------------------------
	void Resize(size64 inNewElementCount)
	{
		if (inNewElementCount == mElementCount) return;									// No resize needed
		if (inNewElementCount > mReservedCount)											// If the new size is bigger than the current buffer size we need to realloc
		{
			mData = TAllocator::sRealloc(mData, mElementCount, inNewElementCount);		// Realloc the buffer, copying all elements
			mReservedCount = inNewElementCount;											// Set reserved count to new buffer size
		}
		TAllocator::sResizeWithinAlloc(mData, mElementCount, inNewElementCount);		// Resize the amount of actual elements inside buffer (uses void constructor)
		mElementCount = inNewElementCount;												// Update element count to new value
	}

	// --------------------------------------------------------------------------------------------------------
	// Appends data of length [inElementCount] from [inData] to the end of the existing elements.
	// Resizes the buffer if needed.
	// --------------------------------------------------------------------------------------------------------
	void Append(const T* inAppendData, size64 inAppendElementCount)
	{
		Grow(GetLength() + inAppendElementCount + 1);					// grow/prealloc to the size needed
		size64 first_free_element = GetLength();						// copy construct new elements
		for (size64 c = 0; c < inAppendElementCount; c++)				// Iterate over all elements in append buffer
		{
			new (mData + (first_free_element + c)) T(inAppendData[c]);	// Copy-construct new data into data buffer
		}
		mElementCount += inAppendElementCount;							// update size
	}

	// --------------------------------------------------------------------------------------------------------
	// Grows the data buffer as needed.
	// In contrast to Reserve() this actually uses a doubling prealloc strategy(!)
	// Also this accepts a minimum size instead of an exact size like Reserve()
	// --------------------------------------------------------------------------------------------------------
	void Grow(size64 inMinimumAllocCount)
	{
		if (inMinimumAllocCount <= mReservedCount) return;		// If the current buffer is big enough, early out
		size64 next_multiple_two = mReservedCount * 2;			// Calculate the next step in size, simply double the size
		if (inMinimumAllocCount > next_multiple_two)			// If the new amount is even bigger than twice the size...
		{
			Reserve(inMinimumAllocCount);						// call [Reserve()] to do an exact resize to the asked value
		}
		else													// ... else
		{
			Reserve(next_multiple_two);							// call [Reserve()] to do an exact resize to the next doubled value
		}
	}

	// Overloads
	void Set(const Array<T>& inOther)							{ Set(inOther.GetData(), inOther.GetLength()); }	// Resizes and copies data; overload of base [Set()]
	void Append(const T& inValue)								{ Append(&inValue, 1); }							// Appends [inValue] to the end of the array
	void Append(const Array<T>& inArray)						{ Append(inArray.mData, inArray.GetLength()); }		// Appends [inArray] to the end of existing array

	// Operators
	Array<T>&		operator=(const Array<T>& inOther)			{ Set(inOther); return *this; }	// Assignment operator. similar to [Set()]
	T&				operator[](size64 inIndex)					{ return mData[inIndex]; }		// Const getter to element
	const T&		operator[](size64 inIndex)			const	{ return mData[inIndex]; }		// Const getter to element


	const T*		begin() const								{ return mData; }
	const T*		end() const									{ return mData + mElementCount; }

	T*				begin()										{ return mData; }
	T*				end()										{ return mData + mElementCount; }


protected:

	// Internal data
	T*				mData;										// Array data buffer. Should be non-null if [mReservedCount] > 0
	size64			mElementCount;								// The amount of constructed and valid elements within the buffer (in consecutive memory)
	size64			mReservedCount;								// Size of databuffer in elements (not bytes(!)). Should be zero if [mData] is null.
};


