#pragma once

/**
Array.h - Standard Array implementation
(c)2017 Jaap van Muijden
**/

#include <CCore/Memory.h>
#include <CHash/MurmurHash3.h>
#include <CUtils/EnumMask.h>

// Array is the base linear array implementation with all the default stuff like resizing etc.
// Uses the LinearAllocator static functions
template<typename T, class TAllocator = LinearAllocator<T>>
class Array
{
public:
	// void constructor
	Array() : mData(nullptr), mEndValid(nullptr), mEndReserved(nullptr) { }
	// move operator
	Array(Array<T>&& inOther)								{ mData = inOther.mData; mEndValid = inOther.mEndValid; mEndReserved = inOther.mEndReserved; inOther.mData = nullptr; inOther.mEndReserved = nullptr; inOther.mEndValid = nullptr; }
	// copy constructor from another array
	Array(const Array<T>& inOther)							{ size64 l = inOther.GetLength();	mData = LinearAllocator<T>::sAllocAndCopyConstruct(l, inOther.GetData(), l); mEndValid = mData+l; mEndReserved=mData+l; }
	// copy constructor from a raw data/elementcount
	Array(const T* inArray, size64 inElementcount)			{ size64 l = inElementcount;		mData = LinearAllocator<T>::sAllocAndCopyConstruct(l, inArray, l);			mEndValid = mData + l; mEndReserved = mData + l; }
	// destructor (only frees any allocated memory, leaves invalid state!)
	~Array()												{ if (mData) LinearAllocator<T>::sFreeAndDestruct(mData, GetLength()); }

	size64 GetLength()								const	{ return mEndValid - mData; }						// Amount of active/initialized elements	
	size64 GetReserved()							const	{ return mEndReserved - mData; }					// Get Allocated elements (data size is sizeof(T)*GetReserved())
	T* GetData()											{ return mData; }									// Getter to raw data block
	const T* GetData()								const	{ return mData; }									// Const getter to raw data block

	T* _GetEndValid()										{ return mEndValid; }								// Getter to end of init. block
	T* _GetEndReserved()									{ return mEndReserved; }							// Getter to end of allocated mem

	T& At(size64 inIndex)									{ gAssert(IsValid()); return mData[inIndex]; }		// Const getter to element
	const T& At(size64 inIndex)						const	{ gAssert(IsValid()); return mData[inIndex]; }		// Const getter to element

	T& Back()												{ gAssert(IsValid()); return *(mEndValid - 1); }	// Getter to last element
	const T& Back()									const	{ gAssert(IsValid()); return *(mEndValid - 1); }	// Const getter to last element

	T& Front()												{ gAssert(IsValid()); return mData[0]; }			// Getter to first element
	const T& Front()								const	{ gAssert(IsValid()); return mData[0]; }			// Const getter to first element

	bool IsEmpty()									const	{ return mEndValid == mData; }						// returns true is element count is zero
	bool IsValid()									const	{ return (mData == nullptr) == (mEndValid == nullptr) && (mData == nullptr) == (mEndReserved == nullptr); }

	void Shrink(size64 inShrinkage)							{ Resize(GetLength() - inShrinkage); }				// Shrinks amount of elements by inShrinkage, destructing elements
	void Pop()												{ Resize(GetLength() - 1); }						// Shrinks elements by one, destructing last element
	void SwapRemove(size64 inIndex)							{ if (inIndex != GetLength() - 1) mData[inIndex] = Back(); Pop(); }

	size64 Find(const T& inElement) const
	{ 
		for (size64 i = 0; i < GetLength(); i++)
		{
			if (mData[i] == inElement)
			{
				return i;
			}
		}
		return cMaxSize64; 
	}

	bool operator==(const Array& inOther) const
	{
		if (GetLength() != inOther.GetLength())
			return false;

		for (size64 e = 0; e < GetLength(); e++)
		{
			if (!(mData[e] == inOther[e]))
				return false;
		}
		return true;
	}

	// Clear entire array of elements
	// In constrast to a Resize(0), this also releases memory if inKeepMemory is false
	void Clear(bool inKeepMemory = false)
	{
		if (inKeepMemory)
		{
			Resize(0);
		}
		else
		{
			if (mData != nullptr)												// If there is any allocation
			{
				gAssert(mEndReserved != mData);									// Sanity Check; reserved count should be mData + n, where m > 0
				TAllocator::sFreeAndDestruct(mData, mEndValid - mData);			// Free allocation
			}
			else
			{
				gAssert(mEndReserved == mData);									// without allocation, reserved should also be nullptr
			}
			mData = nullptr;													// No allocation so zero mData
			mEndValid = nullptr;												// Element count is no zero
			mEndReserved = nullptr;												// Reserved count is no zero
		}
	}

	// Resizes and copies data from [inData] of length [inLength] into buffer
	// Resizes entire array to size [inLength] (but does not shrink allocation length)
	void Set(const T* inData, size64 inLength)
	{
		if (inLength == 0)
		{
			Clear(true);
			return;
		}
		gAssert(IsValid());
		if (mData)
		{
			// resize to zero but keep current allocation
			gAssert(GetLength() > 0);
			TAllocator::sResizeWithinAlloc(mData, GetLength(), 0);
			mEndValid = mData;
		}
		if (GetReserved() < inLength)											// raw realloc if needed
		{
			gAssert(IsEmpty());													// We assume no active elements so realloc will not perform any extra work
			mData = TAllocator::sRealloc(mData, 0, inLength);
			mEndReserved = mData + inLength;									// We now have [inLength] buffer size reserved
			mEndValid = mData;
		}
		gAssert(IsEmpty());
		for (size64 c = 0; c < inLength; c++)
		{
			new (mData + c) T(inData[c]);										// Copy-construct the data inside the allocated data buffer
		}
		mEndValid = mData + inLength;											// All entries up to [inLength] are now valid
	}

	// Reserves a minimum of [inNewAllocCount] in the buffer, copying over any created elements to 
	// a new buffer if needed
	void Reserve(size64 inNewReserveCount)
	{
		if (mData == nullptr)																	// if there is no buffer yet, make a new one
		{
			gAssert(mEndReserved == nullptr);													// sanity check; there cannot be reserved data yet
			mData			= TAllocator::sRawAlloc(inNewReserveCount);							// Allocate new buffer without constructing anything
			mEndReserved	= mData + inNewReserveCount;										// inNewReserveCount reserved objects
			mEndValid		= mData;															// Zero valid objects
		}
		else if (GetReserved() != inNewReserveCount)											// otherwise simply realloc the old one
		{
			gAssert(inNewReserveCount >= GetLength());											// Cannot resize to a smaller size than the element count; this would implicitly delete elements
			size64 old_length	= GetLength();
			mData				= TAllocator::sRealloc(mData, old_length, inNewReserveCount);	// Realloc existing buffer, copying existing elements
			mEndReserved		= mData + inNewReserveCount;									// We have resized our buffer to a new size
			mEndValid			= mData + old_length;
		}
		gAssert(GetReserved() == inNewReserveCount);											// This should always be true if we did our job
	}

	// Resizes the actual allocated data inside the buffer; it will (void) construct and destruct
	// elements if needed(!)
	void Resize(size64 inNewElementCount)
	{
		if (inNewElementCount == GetLength()) return;										// No resize needed
		if (inNewElementCount > GetReserved())												// If the new size is bigger than the current buffer size we need to realloc
		{
			size64 old_length	= GetLength();
			mData				= TAllocator::sRealloc(mData, old_length, inNewElementCount);	// Realloc the buffer, copying all elements
			mEndReserved		= mData + inNewElementCount;									// Set reserved count to new buffer size
			mEndValid			= mData + old_length;											// We moved allocation, so update mEndValid properly
		}
		TAllocator::sResizeWithinAlloc(mData, GetLength(), inNewElementCount);				// Resize the amount of actual elements inside buffer (uses void constructor)
		mEndValid			= mData + inNewElementCount;									// Update element count to new value
	}


	void Insert(const T& inNewElement, size64 inBeforeElement)
	{
		// two cases: one where we do in-place move, one where we do a resize followed by a split copy
		if (mEndReserved - mEndValid >= 1)
		{
			// This breaks if *somehow* mBeginData < sizeof(T)
			// We increase mEndValid because we use one element more
			mEndValid++; 
			T* i = mEndValid;
			for (; i > mData + inBeforeElement; i--)
			{
				// move elements forwards
				i[1] == i[0];
			}
			i[0] = inNewElement;
		}
		else
		{
			size64 new_size = GetLength() + 1;
			// Full new alloc 
			T* new_data = TAllocator::sRawAlloc(new_size);
			// Copy over the "before" part
			for (size64 i = 0; i < inBeforeElement; i++)
			{
				new (new_data + i) T(mData[i]);
			}

			// Copy in the new element
			new (new_data + inBeforeElement) T(inNewElement);
			
			// Copy in the "after" part
			for (size64 i = inBeforeElement+1; i < GetLength()+1; i++)
			{
				new (new_data + i) T(mData[i]);
			}

			// Free old data
			TAllocator::sFreeAndDestruct(mData, GetLength());
			
			// Update members with newly allocated data
			mData = new_data;
			mEndValid = new_data + new_size;
			mEndReserved = mEndValid;
		}
	}

	// Adds an empty T at the end of the buffer
	// Resizes/Reallocs the buffer if needed.
	void AppendEmpty()
	{
		Resize(GetLength()+1);
	}

	// Appends data of length [inElementCount] from [inData] to the end of the existing elements.
	// Resizes the buffer if needed.
	void Append(const T* inAppendData, size64 inAppendElementCount)
	{
		Grow(GetLength() + inAppendElementCount + 1);					// grow/prealloc to the size needed
		for (size64 c = 0; c < inAppendElementCount; c++)				// Iterate over all elements in append buffer
		{
			new (mEndValid++) T(inAppendData[c]);	// Copy-construct new data into data buffer
			gAssert(mEndValid <= mEndReserved);
		}
	}

	// Grows the data buffer as needed.
	// In contrast to Reserve() this actually uses a doubling prealloc strategy(!)
	// Also this accepts a minimum size instead of an exact size like Reserve()
	void Grow(size64 inMinimumAllocCount)
	{
		if (inMinimumAllocCount <= GetReserved()) return;		// If the current buffer is big enough, early out
		size64 next_multiple_two = GetReserved() * 2;			// Calculate the next step in size, simply double the size
		if (inMinimumAllocCount > next_multiple_two)			// If the new amount is even bigger than twice the size...
		{
			Reserve(inMinimumAllocCount);						// call [Reserve()] to do an exact resize to the asked value
		}
		else													// ... else
		{
			Reserve(next_multiple_two);							// call [Reserve()] to do an exact resize to the next doubled value
		}
	}

	void _SetPointer(void* inData, void* inEndValid, void* inEndReserved) { mData = (T*) inData; mEndValid = (T*)inEndValid; mEndReserved = (T*)inEndReserved; }


	// Overloads
	void Set(const Array<T>& inOther)							{ Set(inOther.GetData(), inOther.GetLength()); }	// Resizes and copies data; overload of base [Set()]
	void Append(const T& inValue)								{ Append(&inValue, 1); }							// Appends [inValue] to the end of the array
	void Append(const Array<T>& inArray)						{ Append(inArray.mData, inArray.GetLength()); }		// Appends [inArray] to the end of existing array

	// Operators
	Array<T>&		operator=(const Array<T>& inOther)			{ Set(inOther); return *this; }	// Assignment operator. similar to [Set()]
	T&				operator[](size64 inIndex)					{ return mData[inIndex]; }		// Const getter to element
	const T&		operator[](size64 inIndex)			const	{ return mData[inIndex]; }		// Const getter to element

	const T*		begin() const								{ return mData; }
	const T*		end() const									{ return mEndValid; }
	T*				begin()										{ return mData; }
	T*				end()										{ return mEndValid; }

protected:
	// Internal data
	T*				mData;										// Array data buffer. Should be non-null if [mReservedCount] > 0
	T*				mEndValid;									// First uninitialized/invalid entry
	T*				mEndReserved;								// First entry outside reserved memory
};


