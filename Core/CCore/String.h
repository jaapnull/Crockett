#pragma once
#include "stdafx.h"
#include "Array.h"

template<typename T>
class BaseString : protected LinearAllocator<T>
{
public:

	BaseString() : mData(0), mElementCount(0), mReservedCount(0){}
	BaseString(const Array<T>& inOther)							{ size64 l = inOther.GetLength();					mData = LinearAllocator<T>::sInitAlloc(l + 1, inOther.GetData(), l);	new (mData + l) T(0); mReservedCount = l + 1; mElementCount = l; }
	BaseString(const T* inArray, size64 inLength)				{ size64 l = inLength;								mData = LinearAllocator<T>::sInitAlloc(l + 1, inArray, l);			new (mData + l) T(0); mReservedCount = l + 1; mElementCount = l; }
	BaseString(const BaseString<T>& inOther)					{ size64 l = inOther.GetLength();					mData = LinearAllocator<T>::sInitAlloc(l + 1, inOther.GetData(), l);	new (mData + l) T(0); mReservedCount = l + 1; mElementCount = l; }
	BaseString(size64 inFillLength, const T& inFill)			{ size64 l = inFillLength;							mData = LinearAllocator<T>::sAlloc(l + 1); for (size64 c = 0; c < l; c++) { new (mData + c) T(inFill); } new (mData + l) T(0); mReservedCount = l + 1; mElementCount = l; }
	BaseString(const T* inCString)								{ size64 l = 0; while (inCString[l] != 0) { l++; }; mData = LinearAllocator<T>::sInitAlloc(l + 1, inCString, l); new (mData + l) T(0); mReservedCount = l + 1; mElementCount = l; }

	static void sConstructFromExternalBuffer(BaseString<T>& ioString, void* inData, int inElements)
	{
		ioString.Clear();
		ioString.mData = static_cast<T*>(inData);
		ioString.mElementCount = inElements;
		gAssert(inElments[mElementCount] == 0);
		ioString.mReservedCount = inElements - 1;
	}

	const T* GetCString() const
	{
		if (mData)
		{
			return mData;
		}
		else
		{
			assert(mElementCount == 0);
			return (const T*)&mElementCount;	// use elem_count as zero-terminator (!)
		}
	}

	~BaseString()												{ if (mData) LinearAllocator<T>::sFree(mData, mElementCount + 1); }

	size64 GetLength()									const	{ return mElementCount; }
	size64 GetReserved()									const	{ return mReserverdCount; }

	T* GetData()												{ return mData; }
	const T* GetData()									const	{ return mData; }

	T& operator[](size64 inIndex)									{ return mData[inIndex]; }
	const	T& operator[](size64 inIndex)					const	{ return mData[inIndex]; }

	T& At(size64 inIndex)											{ return mData[inIndex]; }
	const T& At(size64 inIndex)							const	{ return mData[inIndex]; }

	bool IsEmpty()										const	{ return mElementCount == 0; }
	void Shrink(size64 inShrinkage)								{ Resize(mElementCount - inShrinkage); }
	T Pop()														{ T temp = mData[mElementCount - 1]; Resize(inElementCount--);  return T; }

	bool operator== (const BaseString<T>& inOther)		const	{ return Equals(inOther); }
	bool operator!= (const BaseString<T>& inOther)		const	{ return !Equals(inOther); }

	BaseString& Append(const T& inValue)						{ return Append(&inValue, 1); }
	BaseString& Append(const BaseString<T>& inString)			{ return Append(inString.mData, inString.GetLength()); }
	BaseString& Append(const Array<T>& inArray)					{ return Append(inArray.mData, inArray.GetLength()); }
	BaseString& Append(const T* inZeroTerminated)				{ return Append(inZeroTerminated, sGetLength(inZeroTerminated)); }

	void Set(const BaseString<T>& inString)						{ Set(inString.mData, inString.mElementCount); }

	size64 sGetLength(const T* inZeroTerminated)
	{
		size64 i = 0;
		while (inZeroTerminated[i] != 0) i++;
		return i;
	}

	void Clear()
	{
		if (mData)
		{
			LinearAllocator<T>::sFree(mData, mElementCount + 1);
		}
		mData = 0;
		mElementCount = 0;
		mReservedCount = 0;
	}

	void Set(const T* inData, size64 inLength)
	{
		// delete old data including trailing zero
		for (size64 c = 0; c < mElementCount + 1; c++)
		{
			mData[c].~T();
		}
		// raw realloc if needed
		if (mReservedCount < inLength + 1)
		{
			if (mData)
				mData = LinearAllocator::sRealloc(mData, 0, inLength + 1);
			else
				mData = LinearAllocator::sAlloc(inLength + 1);
			mReservedCount = inLength + 1;
		}
		for (size64 c = 0; c < inLength; c++)
		{
			new (mData + c) T(inData[c]);
		}
		// construct new trailing zero
		new (mData + inLength)  T(0);
		mElementCount = inLength;
	}

	BaseString<T> Substring(size64 inStart, size64 inLength)
	{
		assert(inStart + inLength <= mElementCount);
		return BaseString<T>(mData + inStart, inLength);
	}

	bool Equals(const BaseString<T>& inOther) const
	{
		if (mElementCount != inOther.mElementCount) return false;
		for (size64 c = 0; c < mElementCount; c++)
		if (mData[c] != inOther.mData[c]) return false;
		return true;
	}

	void Reserve(size64 inNewReservedCount)	// not counting trailing zero(!)
	{
		size64 effective_reserve = inNewReservedCount + 1;				// one spot for trailing zero
		if (effective_reserve == mReservedCount) return;				// already on the requested reserved size
		assert(mElementCount <= inNewReservedCount);					// cannot reserve less than active elements
		if (mData == 0)													// nothing allocated, so allocate brand new mem
		{
			assert(mElementCount == 0);									// shouldn't have any active elements since mData is null=no allocated memory
			mData = LinearAllocator<T>::sAlloc(effective_reserve);		// Allocate memory of inNewReservedCount + 1 for trailing zero
		}
		else																					// Already have some memory allocated
		{
			mData = LinearAllocator<T>::sRealloc(mData, mElementCount + 1, effective_reserve);	// re-alloc and take all elements with you (including trailing zero)
		}
		mReservedCount = effective_reserve;
	}

	void Resize(size64 inNewElementCount)
	{
		if (inNewElementCount == mElementCount) return;
		mData[mElementCount].~T();	// destruct trailing zero

		// Resize and realloc without trailing zero
		if (inNewElementCount > mElementCount)
		{
			// realloc if needed
			mData = LinearAllocator<T>::sRealloc(mData, mElementCount, inNewElementCount + 1);
			mReservedCount = inNewElementCount;
		}
		LinearAllocator<T>::sResize(mData, mElementCount, inNewElementCount);

		// Reconstruct trailing zero
		new (mData + inNewElementCount) T(0);
		mElementCount = inNewElementCount;
	}

	BaseString& Append(const T* inValue, size64 inElementCount)
	{
		// grow/prealloc
		Grow(GetLength() + inElementCount);
		// copy construct new elements
		size64 first_free_element = GetLength();
		for (size64 c = 0; c < inElementCount; c++)
		{
			new (mData + (first_free_element + c)) T(inValue[c]);
		}
		// update size
		mElementCount += inElementCount;
		// construct trailing zero
		new (mData + mElementCount) T(0);
		return *this;
	}

	size_t GetHash() const
	{
		size_t hash[4]; // 128 bit
		MurmurHash3_x64_128(mData, int(sizeof(T)* mElementCount), 0xDEADBEEF, &(hash[0]));
		return hash[0];
	}

	bool operator<(const BaseString<T>& inOther) const
	{
		size64 min_size = gMin<size64>(inOther.GetLength(), GetLength());
		for (size64 i = 0; i < min_size; i++)
		{
			if (inOther.At(i) == At(i)) continue;
			return inOther.At(i) < At(i);
		}
		return GetLength() < inOther.GetLength();
	}

	// Operators
	BaseString<T>& operator=(const BaseString<T>& inOther)
	{
		Set(inOther); return *this; 
	}

protected:

	void Grow(size64 inNewReservedSize) // mot counting trailing zero(!)
	{
		// do we have enough for data and trailing zero
		if (inNewReservedSize + 1 <= mReservedCount) return;
		size64 next_multiple_two = mReservedCount * 2;
		if (inNewReservedSize > next_multiple_two)
		{
			Reserve(inNewReservedSize);
		}
		else
		{
			Reserve(next_multiple_two);
		}
	}

	T*		mData;			// elements including trailing zero
	size64	mElementCount;	// not including trailing zero
	size64	mReservedCount;	// including trailing zero
};


class String : public BaseString<char>
{
public:
	String()										: BaseString()						{}
	String(const Array<char>& inOther)				: BaseString(inOther)				{}
	String(const char* inArray, size64 inLength)	: BaseString(inArray, inLength)		{}
	String(const BaseString<char>& inOther)			: BaseString(inOther)				{}
	String(size64 inFillLength, const char& inFill) : BaseString(inFillLength, inFill)	{}
	String(const char* inCString) : BaseString(inCString) {}


	// Util functions
	void SubString(String& outString, offset64 inStart, size64 inLength)
	{
		gAssert(inStart + inLength < GetLength());
		outString.Set(mData + inStart, inLength);
	}

	void SplitOn(Array<String>& outSplits, const char& inSplit)
	{
		int last_split_idx = 0;
		for (uint c = 0; c < GetLength(); c++)
		{
			if (mData[c] == inSplit)
			{
				outSplits.Append(String(mData + last_split_idx, c - last_split_idx));
				last_split_idx = c + 1;
			}
		}
		outSplits.Append(String(mData + last_split_idx, GetLength() - last_split_idx));
	}

};



class WString : public BaseString<wchar_t>
{
public:
	WString() : BaseString()						{}
	WString(const Array<wchar_t>& inOther) : BaseString(inOther)				{}
	WString(const wchar_t* inArray, size64 inLength) : BaseString(inArray, inLength)		{}
	WString(const BaseString<wchar_t>& inOther) : BaseString(inOther)				{}
	WString(size64 inFillLength, const wchar_t& inFill) : BaseString(inFillLength, inFill)	{}
	WString(const wchar_t* inCString) : BaseString(inCString) {}
};


inline String gToString(int inInt)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%d", inInt);
	return String(buffer);
}

inline String gToString(uint inUint)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%u", inUint);
	return String(buffer);
}

inline String gToString(double inDouble)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%f", inDouble);
	return String(buffer);
}
inline String gToString(float inFloat)
{
	char buffer[10];
	sprintf_s(buffer, 10, "%f", inFloat);
	return String(buffer);
}
inline String gToString(bool inBool)
{
	return String(inBool ? "true" : "false");
}

inline std::wostream& operator<<(std::wostream& ioStream, const WString& inWstr)
{
	return ioStream << inWstr.GetCString();
}

inline std::wostream& operator<<(std::wostream& ioStream, const String& inStr)
{
	return ioStream << inStr.GetCString();
}


template<typename T>
size_t hash_value(const BaseString<T>& inArray)
{
	return inArray.GetHash();
}


float	gStringToFloat(const String& inString);
int		gStringToInt(const String& inString);
uint	gStringToUInt(const String& inString);
bool	gStringToBool(const String& inString);


